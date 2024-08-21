#include <string.h>

#include "reparser.h"
#include "reNFA.h"

ASTNode * re_pass(Production * prod, Parser * parser, ASTNode * node) {
    if (node->children[0]->rule->id == LPAREN) { // of form '(', choice, ')'
        *node->children[1] = *node; // copy node metadata into choice result (which is really an NFANode)
        node->children[1]->rule = (Rule *)prod;
        return node->children[1];
    }
    node->rule = (Rule *)prod;
    return node; // symbol should already be reduced
}

ASTNode * re_build_symbol(Production * prod, Parser * parser, ASTNode * node) {
    // not sure if these two static arrays are going to be used anymore
    static char const reESCAPED_CHARS[] = "'\"?\\abfnrtv";
    static char const ESCAPED_CHARS[] = "\'\"\?\\\a\b\f\n\r\t\v";
    RegexBuilder * reb = (RegexBuilder *)parser;
    reSymbol * new_sym;
    // build a new symbol
    if (node->rule->id == CHAR_CLASS) {
        reCharClass * class = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(*class), _Alignof(reCharClass));
        class->sym.match = reCharClass_match;
        class->sym.next = reb->nfa->symbols;
        class->inv = node->children[1]->nchildren > 0;
        if (class->inv) {
            class->sym.sym = node->children[1]->token_start->string;
        } else {
            class->sym.sym = node->children[2]->token_start->string;
        }
        class->sym.sym_len = (size_t)(node->children[3]->token_start->string - class->sym.sym); 
        reb->nfa->symbols = (reSymbol *)class;
        new_sym = (reSymbol *)class;
    } else if (node->rule->id == CHARACTER) { // single_char
        reChar * ch = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(*ch), _Alignof(reChar));
        ch->match = reChar_match;
        ch->next = reb->nfa->symbols;
        reb->nfa->symbols = (reSymbol *)ch;
        ch->sym = node->token_start->string;
        ch->sym_len = 1; // TODO: to add multibyte characters. Need to change this
        new_sym = (reSymbol *)ch;
    } else {
        reChar * ch = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(*ch), _Alignof(reChar));
        ch->match = reChar_match;
        ch->next = reb->nfa->symbols;
        reb->nfa->symbols = (reSymbol *)ch;
        ch->sym = node->children[1]->token_start->string;
        ch->sym_len = 1; // TODO: to add multibyte characters. Need to change this
        new_sym = (reSymbol *)ch;
    }
    // record signal in nfa
    reb->nfa->symbols = new_sym;
    reb->nfa->nsymbols++;

    NFATransition * trans = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFATransition), _Alignof(NFATransition));
    
    // build and initialize start and final state for a transition using the symbol
    NFAState * final = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    NFAState * start = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    *start = (NFAState) {.n_out = 1, .n_in = 0, .out = trans};
    *final = (NFAState) {.n_out = 0, .n_in = 1, .in = trans};
    reb->nfa->nstates += 2;

    // initialize transition for start to final state
    *trans = (NFATransition) {.final = final, .next_in = NULL, .next_out = NULL, .start = start, .symbol = new_sym};

    // memory allocation with Mempool...entire node follows reNFA output rather than parser
    //NFANode * subnfa = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFANode), _Alignof(NFANode));
    // memory allocation with Parser...node is released with parser
    NFANode * subnfa = (NFANode *)Parser_add_node(parser, (Rule *)prod, node->token_start, node->token_end, 0, 0, sizeof(NFANode));
    *subnfa = (NFANode) {.node = *node, .start = start, .final = final};
    subnfa->node.rule = (Rule *)prod;
    return (ASTNode *)subnfa;
}

NFATransition * re_build_empty_transition(RegexBuilder * reb, NFAState * start, NFAState * final) {
    NFATransition * trans = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFATransition), _Alignof(NFATransition));
    *trans = (NFATransition) {.final = final, .start = start, .next_in = final->in, .next_out = start->out, .symbol= &empty_symbol};
    final->in = trans;
    final->n_in++;
    start->out = trans;
    start->n_out++;
    return trans;
}

ASTNode * re_build_choice(Production * prod, Parser * parser, ASTNode * node) {
    if (node->nchildren == 1) {
        return node->children[0];
    }

    RegexBuilder * reb = (RegexBuilder *)parser;
    NFAState * final = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    NFAState * start = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    *start = (NFAState) {0};
    *final = (NFAState) {0};

    size_t i = node->nchildren;
    //NFATransition * cur_trans = NULL;
    while (i--) {
        NFANode * subnfa = (NFANode *)node->children[i];
        re_build_empty_transition(reb, start, subnfa->start);
        re_build_empty_transition(reb, subnfa->final, final);
        if (!i) {
            break;
        }
        i--;
    }
    reb->nfa->nstates += 2;

    NFANode * nextnfa = (NFANode *)node->children[0];
    nextnfa->node = *node; // copy metadata from the node into an NFANode.node
    nextnfa->start = start;
    nextnfa->final = final;
    nextnfa->node.rule = (Rule *)prod;
    return (ASTNode *)nextnfa;
}

ASTNode * re_build_sequence(Production * prod, Parser * parser, ASTNode * node) {
    if (node->nchildren == 1) {
        return node->children[0];
    }

    RegexBuilder * reb = (RegexBuilder *)parser;
    
    NFANode * subnfa = NULL;
    NFANode * nextnfa = (NFANode *)node->children[0];
    NFAState * start = nextnfa->start;
    for (size_t i = 1; i < node->nchildren; i++) {
        subnfa = nextnfa;
        nextnfa = (NFANode *)node->children[i];
        re_merge_states(subnfa->final, nextnfa->start);
    }

    nextnfa->node = *node; // copy metadata from the node into an NFANode.node
    nextnfa->start = start;
    nextnfa->node.rule = (Rule *)prod;
    return (ASTNode *)nextnfa;
}

ASTNode * re_build_optional(Production * prod, Parser * parser, ASTNode * node) {
    RegexBuilder * reb = (RegexBuilder *)parser;
    NFAState * final = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    NFAState * start = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    *final = (NFAState){0};
    *start = (NFAState){0};
    NFANode * subnfa = (NFANode *)node->children[0];
    re_build_empty_transition(reb, start, final);
    re_build_empty_transition(reb, start, subnfa->start);
    re_build_empty_transition(reb, subnfa->final, final);
    subnfa->start = start;
    subnfa->final = final;
    subnfa->node = *node;
    subnfa->node.rule = (Rule *)prod;
    return (ASTNode *)subnfa;
}

ASTNode * re_build_kleene(Production * prod, Parser * parser, ASTNode * node) {
    RegexBuilder * reb = (RegexBuilder *)parser;
    NFAState * final = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    NFAState * start = MemPoolManager_aligned_alloc(reb->nfa->pool, sizeof(NFAState), _Alignof(NFAState));
    *final = (NFAState){0};
    *start = (NFAState){0};
    NFANode * subnfa = (NFANode *)node->children[0];
    re_build_empty_transition(reb, start, final);
    re_build_empty_transition(reb, start, subnfa->start);
    re_build_empty_transition(reb, subnfa->final, final);
    re_build_empty_transition(reb, subnfa->final, subnfa->start);
    subnfa->start = start;
    subnfa->final = final;
    subnfa->node = *node;
    subnfa->node.rule = (Rule *)prod;
    return (ASTNode *)subnfa;
}

void re_merge_states(NFAState * start, NFAState * final) {
    // the start is the final node in a subNFA, which should have no transitions out
    // the final is the start node of a subsequence NFA, which should have all the transitions
    // for each transition in final, set the start state to start

    // handle final->out by setting the starts to start
    NFATransition * trans = final->out;
    NFATransition * trans_start = trans;
    trans->start = start;
    while (trans->next_out) {
        trans = trans->next_out;
        trans->start = start;        
    }
    trans->next_out = start->out;
    start->out = trans_start;
    start->n_out += final->n_out;

    // handle final->in by setting the finals to start
    if (final->in) {
        trans = final->in;
        trans_start = trans;
        trans->final = start;
        while (trans->next_in) {
            trans = trans->next_in;
            trans->final = start;
        }
        trans->next_in = start->in;
        start->in = trans_start;
        start->n_in += final->n_in;
    }
    
}

ASTNode * re_build_nfa(Production * prod, Parser * parser, ASTNode * node) {
    RegexBuilder * reb = (RegexBuilder *)parser;
    NFANode * nfanode = (NFANode *)node;
    reb->nfa->start = nfanode->start;
    reb->nfa->final = nfanode->final;
    return node;
}

char const * get_last_element(char const * start, char const * end) {
    end--;
    if (*end == ')') {
        int open = 1;
        while (open && end != start) {
            end--;
            switch (*end) {
                case ')': {
                    open++;
                    break;
                }
                case '(': {
                    open--;
                    break;
                }
            }
        }
        return end;
    }
    return end;
}

char * RegexBuilder_realloc_buffer(RegexBuilder * reb, char * buf, size_t * buf_len, size_t new_size) {
    if (new_size <= *buf_len) {
        return buf;
    }
    char * new_buf = MemPoolManager_malloc(reb->nfa->pool, new_size * sizeof(*buf));
    if (!new_buf) {
        return buf;
    }
    memcpy(new_buf, buf, *buf_len);
    *buf_len = new_size;
    return new_buf; // not a leak since buf should be allocated by MemPoolManager
}

char * RegexBuilder_preprocess_repeatopt(RegexBuilder * reb, char * buf, size_t * buf_len, char const * base, size_t base_length, size_t nrepeats, size_t * loc) {
    size_t space_needed = nrepeats * (base_length + 3) + *loc + 1; // add one for null-terminator
    if (*buf_len < space_needed) {
        space_needed = space_needed < 2 *(*buf_len) ? 2 * (*buf_len) : space_needed;
        buf = RegexBuilder_realloc_buffer(reb, buf, buf_len, space_needed);
    }
    for (size_t i = 0; i < nrepeats; i++) {
        *loc += snprintf(buf + *loc, *buf_len - *loc, "(%.*s", (int)base_length, base);
    }
    for (size_t i = 0; i < nrepeats; i++) {
        *loc += snprintf(buf + *loc, *buf_len - *loc, ")?");
    }
    return buf;
}

char * RegexBuilder_preprocess_repeat(RegexBuilder * reb, char * buf, size_t * buf_len, char const * base, size_t base_length, size_t nrepeats, size_t * loc) {
    size_t space_needed = nrepeats * base_length + *loc + 1; // add one for null-terminator
    if (*buf_len < space_needed) {
        space_needed = space_needed < 2 *(*buf_len) ? 2 * (*buf_len) : space_needed;
        buf = RegexBuilder_realloc_buffer(reb, buf, buf_len, space_needed);
    }
    for (size_t i = 0; i < nrepeats; i++) {
        *loc += snprintf(buf + *loc, *buf_len - *loc, "%.*s", (int)base_length, base);
    }
    return buf;
}

void RegexBuilder_preprocess(RegexBuilder * reb, char const * regex_s, size_t regex_len) {
    size_t buf_len = regex_len * 2; // allocate 2x times the size
    char * buf = MemPoolManager_malloc(reb->nfa->pool, buf_len * sizeof(*buf));
    size_t i = 0, j = 0;
    while (i < regex_len) {
        size_t write = i;
        while (i < regex_len && *(regex_s + i) != '{' && *(regex_s + i) != '+') {
            i++;
        }
        if (i > write) {
            buf = RegexBuilder_realloc_buffer(reb, buf, &buf_len, j + (i - write));
            j += snprintf(buf + j, buf_len - j, "%.*s", (int)(i - write), regex_s + write);
        }
        if (i >= regex_len) {
            break;
        }
        if (*(regex_s + i) == '{') {
            i++; // skip past '{'
            if (*(regex_s + i - 2) == '\\') { // check if '{' was escaped
                j--; // overwrite '{' as it loses syntactic meaning
                j += snprintf(buf + j, buf_len - j, "{");
                
                continue;
            }
            size_t min_rep = 0, max_rep = 0;
            char * next;
            if (*(regex_s + i) != ',') {
                min_rep = (size_t)strtoull(regex_s + i, &next, 10);
                i = (size_t)(next - regex_s);
                if (*next == '}') {// {min_rep}
                    max_rep = min_rep;
                } else { // must have ',' next
                    i++; // skip comma
                    while (*(regex_s + i) == ' ') {
                        i++;
                    }
                    if (*(regex_s + i) != '}') { // {min_rep, max_rep}
                        max_rep = (size_t)strtoull(regex_s + i, &next, 10);
                        i = (size_t)(next - regex_s);
                    }
                }
                i++; // skip closing brace
            } else { // min_rep = 0
                i++; // skip comma
                while (*(regex_s + i) == ' ') {
                    i++;
                }
                if (*(regex_s + i) != '}') { // {, max_rep}
                    max_rep = (size_t)strtoull(regex_s + i, &next, 10);
                    i = (size_t)(next - regex_s);
                }
                i++;
            }
            // TODO get last element should grab from what has already been printed to buffer to allow recursion
            char const * start = get_last_element(buf, buf + j);
            if (max_rep == 0) {
                // replace A{min_rep,} with A1 A2 ... Am A*
                buf = RegexBuilder_preprocess_repeat(reb, buf, &buf_len, start, (size_t)(buf + j - start), min_rep, &j);
                buf = RegexBuilder_realloc_buffer(reb, buf, &buf_len, j + 1);
                j += snprintf(buf + j, buf_len - j, "*"); // one rep was already printed, need one more (covered above with min_rep nrepeats) and add asterisk
            } else if (max_rep == 1) {
                if (min_rep == 1) { // replace with A
                    // do nothing, this is already done
                } else if (min_rep == 0) { // replace with A?
                    buf = RegexBuilder_realloc_buffer(reb, buf, &buf_len, j + 1);
                    j += snprintf(buf + j, buf_len - j, "?");
                } else {
                    // ERROR!
                }
            } else { // replace A{m,n} with A1 A2...Am(Am+1(Am+2(...An-1(An)?)?)?)?
                size_t base_len = (size_t)(buf + j - start);
                buf = RegexBuilder_preprocess_repeat(reb, buf, &buf_len, start, base_len, min_rep - 1, &j); // min_rep - 1 because one should already have been printed
                buf = RegexBuilder_preprocess_repeatopt(reb, buf, &buf_len, start, base_len, max_rep - min_rep, &j);
            }
        } else if (*(regex_s + i) == '+') {
            i++; // skip past '+'
            if (*(regex_s + i - 2) == '\\') { // check if '+' was escaped
                j--; // overwrite '+' as it loses syntactic meaning
                j += snprintf(buf + j, buf_len - j, "+");
                continue;
            }
            
            char const * start = get_last_element(buf, buf + j);
            // replace A+ with AA*
            buf = RegexBuilder_realloc_buffer(reb, buf, &buf_len, j + (size_t)(buf + j - start) + 1);
            j += snprintf(buf + j, buf_len - j, "%.*s*", (int)(regex_s - start + i), start);
        }
    }
    reb->nfa->regex_s = buf;
    reb->nfa->regex_len = j;
}

void RegexBuilder_init(RegexBuilder * reb, reNFA * nfa) {
    // build and initialize an empy symbol transition
    Parser_init(&reb->parser, (Rule *)&re_token, (Rule *)&re_re, RE_NRULES, 0);
    Parser_set_log_file((Parser *)reb, "re.log", LOG_LEVEL_DEBUG);
    reb->nfa = nfa;
}

reNFA * RegexBuilder_build(RegexBuilder * reb, char const * regex_in, size_t regex_in_len) {
    RegexBuilder_preprocess(reb, regex_in, regex_in_len);
    Parser_parse(&reb->parser, reb->nfa->regex_s, reb->nfa->regex_len);
    //Parser_print_ast((Parser *)reb, stdout); infinite loop somewhere. need to reset children somehow
    return reb->nfa;
}

// the nfa remains!
void RegexBuilder_dest(RegexBuilder * reb) {
    Parser_dest(&reb->parser);
}


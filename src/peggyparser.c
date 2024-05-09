/* C std lib */
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* lib includes */
#include <logger.h>

/* peggy includes */
#include <peggy/token.h>
#include <peggy/rule.h>
#include <peggy/stack.h>

/* local includes */
#include "peggyparser.h"
#include "peggy.h"

typedef struct STACK(char) StringBuilder;

#define ELEMENT_TYPE char
#include <peggy/stack.h>

const PeggyString NULL_STRING = {.str = NULL, .len = 0};

struct PeggyParserType PeggyParser_class = {
    .Parser_class = {
        .type_name = PeggyParser_NAME,
        .new = &Parser_new,
        .init = &Parser_init,
        .dest = &Parser_dest, 
        .del = &Parser_del,
        .tell = &Parser_tell, 
        .seek = &Parser_seek, 
        .log = &Parser_log, 
        .log_check_fail_ = &Parser_log_check_fail_, 
        .get_line_col_end = &Parser_get_line_col_end, 
        .gen_final_token_ = &Parser_gen_final_token_, 
        .skip_token = &Parser_skip_token, 
        .estimate_final_ntokens_ = &Parser_estimate_final_ntokens,
        .add_token = &Parser_add_token, 
        .add_node = &Parser_add_node,
        .gen_next_token_ = &Parser_gen_next_token_, 
        .get = &Parser_get, 
        .get_tokens = &Parser_get_tokens, 
        .parse = &PeggyParser_parse,
        .check_cache = &Parser_check_cache,
        .cache_check = &Parser_cache_check,
        .traverse = &Parser_traverse, 
        .print_ast = &Parser_print_ast 
    },
    .init = &PeggyParser_init,
    .dest = &PeggyParser_dest,
};

struct PeggyParser peggy = {
    .Parser = {._class = &(PeggyParser_class.Parser_class),
        .token_rule = NULL,
        .root_rule = NULL,
        .name = "",
        .log_file = NULL,
        .logger = DEFAULT_LOGGER_INIT,
        .loc_ = 0,
        .disable_cache_check = false,
        .ast = NULL
    },
    ._class = &PeggyParser_class,
    .header_file = NULL,
    .source_file = NULL,
    .header_name = NULL,
    .source_name = NULL,
};

int StringBuilder_comp(StringBuilder a, StringBuilder b) {
    if (b.fill != a.fill) {
        return 1;
    }
    return strncmp(a.bins, b.bins, a.fill);
}

bool PeggyString_startswith(PeggyString a, PeggyString prefix) {
    if (a.len < prefix.len) {
        return false;
    }
    size_t i = 0;
    while (i < prefix.len) {
        if (a.str[i] != prefix.str[i]) {
            return false;
        }
        i++;
    }
    return true;
}

#define PSFO_NONE 0
#define PSFO_LOWER 1
#define PSFO_UPPER 2
#define PSFO_CASE_MASK = 3
#define PSFO_STRIP 4
#define PSFO_RESERVED_BITS 4
#define PSFO_LOFFSET(x) ((x) << PSFO_RESERVED_BITS)
#define PSFO_GET_LOFFSET(flags) ((flags) >> PSFO_RESERVED_BITS)

void PeggyString_fwrite(PeggyString a, FILE * output, unsigned int flags) {
    if (!flags) {
        fwrite(a.str, sizeof(char), a.len, output);
        return;
    }
    size_t start = PSFO_GET_LOFFSET(flags);
    size_t end = a.len;
    if (PSFO_STRIP & flags) {
        while (start < end && isspace(a.str[start])) {
            start++;
        }
        while (end > start && isspace(a.str[end-1])) {
            end--;
        }
        PeggyString_fwrite((PeggyString) {.str = a.str + start, .len = end-start}, output, (flags & ~PSFO_STRIP));
        return;
    }
    if (end <= start) {
        return;
    }
    if (flags & PSFO_LOWER) {
        for (size_t i = start; i < end; i++) {
            fputc(tolower(a.str[i]), output);
        }
    } else {
        for (size_t i = start; i < end; i++) {
            fputc(toupper(a.str[i]), output);
        }
    }
    
}

int PeggyString_comp(PeggyString a, PeggyString b) {
    if (a.len != b.len) {
        return 1;
    }
    int res = strncmp(a.str, b.str, a.len);
    return res;
}

size_t PeggyString_hash(PeggyString a, size_t bin_size) {
    unsigned long long hash = 5381;
    size_t i = 0;
    unsigned char * str = (unsigned char *) a.str;

    while (i < a.len) {
        hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
        str++;
        i++;
    }
    return hash % bin_size;
}

PeggyString get_string_from_parser(PeggyParser * parser, ASTNode * node) {
    size_t ntokens = node->ntokens;
    Token * toks = parser->_class->Parser_class.get_tokens(&parser->Parser, node);
    return (PeggyString){.str = (char *)toks[0].string, .len = toks[ntokens-1].length + (size_t)(toks[ntokens-1].string - toks[0].string)};
}

PeggyString get_rule_pointer(PeggyParser * parser, PeggyString name) {
    PeggyProduction prod = {0};
    if (parser->productions._class->get(&parser->productions, name, &prod)) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - retrieval of production %.*s failed.\n", __func__, name.len, name.str);
    }
    char * rule_resolve = strchr(prod.type_name, '.');
    size_t length = 0;
    if (rule_resolve) {
        length = strlen(rule_resolve);
    }
    PeggyString arg = {.len = 1 + prod.identifier.len + length};
    arg.str = malloc(sizeof(char) * arg.len);
    arg.str[0] = '&';
    memcpy((void*)(arg.str + 1), (void*)prod.identifier.str, prod.identifier.len);
    if (length) {
        memcpy((void*)(arg.str + 1 + prod.identifier.len), rule_resolve, length);
    }
    return arg;
}

PeggyString get_rule_resolution(char const * type_name) {
    size_t N = strlen(type_name);
    char * first_period = strchr(type_name, '.');
    return (PeggyString) {.len = N - (size_t)(first_period - type_name), .str = first_period};
}

int PeggyProduction_write_arg(void * data, PeggyString arg) {
    PeggyParser * parser = (PeggyParser *) data;
    fwrite(",\n\t", sizeof(char), 3, parser->source_file);
    PeggyString_fwrite(arg, parser->source_file, PSFO_NONE);
    return 0;
}

// signature is so that it can be used by for_each in the hash map
int PeggyProduction_define(void * parser_, PeggyString name, PeggyProduction prod) {
    PeggyParser * parser = (PeggyParser *)parser_;

    char * main_type_end = strchr(prod.type_name, '.');
    PeggyString type_main = {.len = main_type_end ? (size_t)(main_type_end - prod.type_name) : strlen(prod.type_name), .str = (char *)prod.type_name};
    PeggyString_fwrite(type_main, parser->source_file, PSFO_UPPER);

    fputc('(', parser->source_file);
    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_NONE);
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building definition for production %.*s\n", __func__, (int)prod.identifier.len, prod.identifier.str);
    fputc(',', parser->source_file);

    unsigned int offset = 0;
    if (PeggyString_startswith(prod.identifier, parser->export)) {
        offset = (unsigned int)(parser->export.len + 1);
    }
    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_UPPER | PSFO_LOFFSET(offset));

    prod.args._class->for_each(&prod.args, &PeggyProduction_write_arg, (void*)parser);

    fwrite(");\n", sizeof(char), strlen(");\n"), parser->source_file);

    return 0;
}

int build_export_rules_resolved(void * parser_, PeggyString name, PeggyProduction prod) {
    PeggyParser * parser = (PeggyParser *) parser_;
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building export rules for %.*s\n", __func__, prod.name.len, prod.name.str);
    char * rule_resolve = strchr(prod.type_name, '.');
    size_t length = rule_resolve ? strlen(rule_resolve) : 0;
    PeggyString arg = {.len = 1 + prod.identifier.len + length};
    arg.str = malloc(sizeof(char) * arg.len);
    arg.str[0] = '&';
    memcpy((void*)(arg.str + 1), (void*)prod.identifier.str, prod.identifier.len);
    if (length) {
        memcpy((void*)(arg.str + 1 + prod.identifier.len), rule_resolve, length);
    }

    PeggyString_fwrite(arg, parser->source_file, PSFO_NONE);
    char * buffer = ",\n\t";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->source_file);

    free(arg.str);

    return 0;
}

void build_export_rules(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - building export rules\n", __func__);
    char const * buffer = NULL;
    FILE * file = parser->source_file;
    buffer = "\n\nRule * ";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_UPPER);

    buffer = "_NRULES + 1] = {\n\t";
    fwrite(buffer, sizeof(char), strlen(buffer), file);

    parser->productions._class->for_each(&parser->productions, &build_export_rules_resolved, (void*)parser);

    buffer = "NULL\n};\n\n";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    
}

void build_destructor(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - building destructor\n", __func__);
    char const * buffer = NULL;
    FILE * file = parser->source_file;
    buffer = "void ";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "_dest(void) {\n\tint i = 0;\n\twhile (";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[i]) {\n\t\t";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[i]->_class->dest(";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
    PeggyString_fwrite(parser->export, file, PSFO_NONE);

    buffer = "rules[i]);\n\t\ti++;\n\t}\n}\n";
    fwrite(buffer, sizeof(char), strlen(buffer), file);
}

void PeggyProduction_declare(PeggyParser * parser, PeggyProduction prod) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - declaring type %.*s\n", __func__, prod.name.len, prod.name.str);
    //printf("declaring type: %s\n", prod.type_name);

    char * main_type_end = strchr(prod.type_name, '.');
    PeggyString type = {.len = main_type_end ? (size_t)(main_type_end - prod.type_name) : strlen(prod.type_name), .str = (char *)prod.type_name};
    PeggyString_fwrite(type, parser->source_file, PSFO_NONE);
    fputc(' ', parser->source_file);
    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_NONE);
    // for testing
    fwrite("; // ", 1, strlen("; // "), parser->source_file);
    
    unsigned int offset = 0;
    if (PeggyString_startswith(prod.identifier, parser->export)) {
        offset = (unsigned int)(parser->export.len + 1);
    }

    PeggyString_fwrite(prod.identifier, parser->source_file, PSFO_UPPER | PSFO_LOFFSET(offset));
    fputc('\n', parser->source_file);

    PeggyString_fwrite(prod.identifier, parser->header_file, PSFO_UPPER | PSFO_LOFFSET(offset));
    fwrite(",\n\t", 1, strlen(",\n\t"), parser->header_file);
    fflush(parser->header_file);
    fflush(parser->source_file);
}

void production_init(PeggyParser * parser, PeggyString name, PeggyProduction * prod) {
    STACK_INIT(PeggyString)(&prod->args, 0);
    prod->name = name;
}

char * copy_export(PeggyParser * parser, char * buffer);
void handle_string_literal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id);
void handle_regex_literal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id);
void handle_simplified_rules(PeggyParser * parser, ASTNode * node, const PeggyString parent_id, PeggyString name);
void handle_base_rule(PeggyParser * parser, ASTNode * node, const PeggyString parent_id, PeggyString name);
unsigned char size_t_strlen(size_t val){
    char buffer[64]; // a buffer of 64 will cover any crazy sized architectures
    int length = sprintf(buffer, "%zu", val);
    if (length < 0 || length > 63) {
        printf("what the hell did you do\?\?\?!!!\n");
        exit(EXIT_FAILURE);
    }
    return length;
}


void handle_terminal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id) {
    if (node->rule->id == STRING_LITERAL) {
        handle_string_literal(parser, node->children[0], parent_id);
    } else {
        handle_regex_literal(parser, node->children[0], parent_id);
    }
}


void handle_lookahead_rule(PeggyParser * parser, ASTNode * node, PeggyString name) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling lookahead rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    
    PeggyProduction prod;
    production_init(parser, name, &prod);

    ASTNode * lookahead_type = node->children[0]->children[0];
    switch (lookahead_type->rule->id) {
        case AMPERSAND: {
            prod.type_name = ((RuleType *) &PositiveLookahead_class)->type_name;
            prod.identifier.len = parser->export.len + 1 + strlen("pos") + 1 + size_t_strlen(parser->productions.fill);
            prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
            char * buffer = copy_export(parser, prod.identifier.str);
            size_t written = (size_t)(buffer - prod.identifier.str);
            written += sprintf(buffer, "_pos_%zu", parser->productions.fill);
            prod.identifier.str[written] = '\0';

            // add arguments
            break;
        }
        case EXCLAIM: { // repeat 0 or more
            prod.type_name = ((RuleType *) &NegativeLookahead_class)->type_name;
            prod.identifier.len = parser->export.len + 1 + strlen("neg") + 1 + size_t_strlen(parser->productions.fill);
            prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
            char * buffer = copy_export(parser, prod.identifier.str);
            size_t written = (size_t)(buffer - prod.identifier.str);
            written += sprintf(buffer, "_neg_%zu", parser->productions.fill);
            prod.identifier.str[written] = '\0';

            PeggyProduction_declare(parser, prod);
            parser->productions._class->set(&parser->productions, prod.name, prod);

            handle_base_rule(parser, node->children[0], NULL_STRING, get_string_from_parser(parser, node->children[1]));

            // add arguments
            break;
        }
        default: { // repeat m to n
            LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - syntax error. lookahead rule with leading rule %d not understood\n", __func__, lookahead_type->rule->id);
            return ;
        }
    }

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    PeggyString look_name = get_string_from_parser(parser, node->children[1]);
    handle_base_rule(parser, node->children[1], NULL_STRING, look_name);
    prod.args._class->push(&prod.args, get_rule_pointer(parser, look_name));

    parser->productions._class->set(&parser->productions, prod.name, prod);
} 

void handle_list_rule(PeggyParser * parser, ASTNode * node, PeggyString name) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling list rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    PeggyProduction prod;
    production_init(parser, name, &prod);
    size_t nchildren = (node->nchildren + 1) / 2;
    prod.identifier.len = parser->export.len + 1 + strlen("list") + 1 + size_t_strlen((size_t)nchildren) + 1 + size_t_strlen(parser->productions.fill);
    prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
    char * buffer = copy_export(parser, prod.identifier.str);
    size_t written = (size_t)(buffer - prod.identifier.str);
    written += sprintf(buffer, "_list_%zu_%zu", nchildren, parser->productions.fill);
    prod.identifier.str[written] = '\0';

    prod.type_name = ((RuleType *) &ListRule_class)->type_name;

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    
    for (size_t i = 0; i < node->nchildren; i += 2) {
        PeggyString name = get_string_from_parser(parser, node->children[i]);
        handle_simplified_rules(parser, node->children[i], NULL_STRING, name);
        prod.args._class->push(&prod.args, get_rule_pointer(parser, name));
    }
    
    parser->productions._class->set(&parser->productions, prod.name, prod);
}

void handle_repeated_rule(PeggyParser * parser, ASTNode * node, PeggyString name) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling repeated rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);

    PeggyProduction prod;  
    production_init(parser, name, &prod);
    prod.type_name = ((RuleType *) &RepeatRule_class)->type_name;

    ASTNode * repeat_type = node->children[1]->children[0];

    PeggyString m = {0}, n = {0};
    
    switch (repeat_type->rule->id) {
        case PLUS: { // repeat 1 or more
            //printf("one or more\n");
            m.str = malloc(sizeof(char));
            memcpy((void*)m.str, "1", 1);
            m.len = 1;

            prod.identifier.len = parser->export.len + 1 + strlen("rep_1_0") + 1 + size_t_strlen(parser->productions.fill);
            prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
            char * buffer = copy_export(parser, prod.identifier.str);
            size_t written = (size_t)(buffer - prod.identifier.str);
            written += sprintf(buffer, "_rep_1_0_%zu", parser->productions.fill);
            prod.identifier.str[written] = '\0';

            // add arguments
            break;
        }
        case ASTERISK: { // repeat 0 or more
            //printf("zero or more\n");
            prod.identifier.len = parser->export.len + 1 + strlen("rep_0_0") + 1 + size_t_strlen(parser->productions.fill);
            prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
            char * buffer = copy_export(parser, prod.identifier.str);
            size_t written = (size_t)(buffer - prod.identifier.str);
            written += sprintf(buffer, "_rep_0_0_%zu", parser->productions.fill);
            prod.identifier.str[written] = '\0';

            // add arguments
            break;
        }
        case QUESTION: { // repeat 0 or 1
            //printf("optional\n");
            m.str = malloc(sizeof(char));
            memcpy((void*)m.str, "0", 1);
            m.len = 1;
            n.str = malloc(sizeof(char));
            memcpy((void*)n.str, "1", 1);
            n.len = 1;

            prod.identifier.len = parser->export.len + 1 + strlen("rep_0_1") + 1 + size_t_strlen(parser->productions.fill);
            prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
            char * buffer = copy_export(parser, prod.identifier.str);
            size_t written = (size_t)(buffer - prod.identifier.str);
            written += sprintf(buffer, "_rep_0_1_%zu", parser->productions.fill);
            prod.identifier.str[written] = '\0';
            
            break;
        }
        default: { // repeat m to n
            PeggyString mstr = get_string_from_parser(parser, repeat_type->children[1]);
            PeggyString nstr = get_string_from_parser(parser, repeat_type->children[3]);
            printf("mstr: %zu, %.*s, nstr: %zu, %.*s\n", mstr.len, (int)mstr.len, mstr.str, nstr.len, (int)nstr.len, nstr.str);
            if (mstr.len) {
                m.str = malloc(sizeof(char) * mstr.len);
                memcpy((void*)m.str, (void*)mstr.str, mstr.len);
                m.len = mstr.len;   
            } else {
                m.str = malloc(sizeof(char));
                m.str[0] = '0';
                m.len = 1;
            }

            if (nstr.len) {
                n.str = malloc(sizeof(char) * nstr.len);
                memcpy((void*)n.str, (void*)nstr.str, nstr.len);
                n.len = nstr.len;
            } else {
                n.str = malloc(sizeof(char));
                n.str[0] = '0';
                n.len = 1;
            }

            prod.identifier.len = parser->export.len + 1 + strlen("rep__") + mstr.len + nstr.len + 1 + size_t_strlen(parser->productions.fill);
            prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
            char * buffer = copy_export(parser, prod.identifier.str);
            size_t written = (size_t)(buffer - prod.identifier.str);
            written += sprintf(buffer, "_rep_");
            if (mstr.len) {
                memcpy((void*)(prod.identifier.str + written), (void*)mstr.str, mstr.len);
            }
            written += mstr.len;
            prod.identifier.str[written++] = '_';
            if (nstr.len) {
                memcpy((void*)(prod.identifier.str + written), (void*)nstr.str, nstr.len);
            }
            written += nstr.len;
            written += sprintf((prod.identifier.str + written), "_%zu", parser->productions.fill);
            prod.identifier.str[written] = '\0';

            // add arguments
        }
    }

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    PeggyString rep_name = get_string_from_parser(parser, node->children[0]);
    handle_simplified_rules(parser, node->children[0], NULL_STRING, rep_name);
    prod.args._class->push(&prod.args, get_rule_pointer(parser, rep_name));
    if (m.len) {
        prod.args._class->push(&prod.args, m);
        if (n.len) {
            prod.args._class->push(&prod.args, n);
        }
    }   

    parser->productions._class->set(&parser->productions, prod.name, prod);
}

void handle_sequence(PeggyParser * parser, ASTNode * node, PeggyString name) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling sequence rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    PeggyProduction prod;
    production_init(parser, name, &prod);
    size_t nchildren = (node->nchildren + 1) / 2;
    prod.identifier.len = parser->export.len + 1 + strlen("seq") + 1 + size_t_strlen((size_t)nchildren) + 1 + size_t_strlen(parser->productions.fill);
    prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
    char * buffer = copy_export(parser, prod.identifier.str);
    size_t written = (size_t)(buffer - prod.identifier.str);
    written += sprintf(buffer, "_seq_%zu_%zu", nchildren, parser->productions.fill);
    prod.identifier.str[written] = '\0';
    prod.type_name = ((RuleType *) &SequenceRule_class)->type_name;

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    for (size_t i = 0; i < node->nchildren; i += 2) {
        PeggyString name = get_string_from_parser(parser, node->children[i]);
        handle_simplified_rules(parser, node->children[i], NULL_STRING, name);
        prod.args._class->push(&prod.args, get_rule_pointer(parser, name));
    }

    parser->productions._class->set(&parser->productions, prod.name, prod);
}

void handle_choice(PeggyParser * parser, ASTNode * node, PeggyString name) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling choice rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    PeggyProduction prod;
    production_init(parser, name, &prod);
    size_t nchildren = (node->nchildren + 1) / 2;
    prod.identifier.len = parser->export.len + 1 + strlen("choice") + 1 + size_t_strlen((size_t)nchildren) + 1 + size_t_strlen(parser->productions.fill);
    prod.identifier.str = malloc(sizeof(char) * (prod.identifier.len + 1));
    char * buffer = copy_export(parser, prod.identifier.str);
    size_t written = (size_t)(buffer - prod.identifier.str);
    written += sprintf(buffer, "_choice_%zu_%zu", nchildren, parser->productions.fill);
    prod.identifier.str[written] = '\0';
    prod.type_name = ((RuleType *) &ChoiceRule_class)->type_name;

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);


    for (size_t i = 0; i < node->nchildren; i += 2) {
        PeggyString name = get_string_from_parser(parser, node->children[i]);
        handle_simplified_rules(parser, node->children[i], NULL_STRING, name);
        prod.args._class->push(&prod.args, get_rule_pointer(parser, name));
    }

    parser->productions._class->set(&parser->productions, prod.name, prod);
}

PeggyProduction PeggyProduction_build(PeggyParser * parser, ASTNode * id, char const * type) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building PeggyProduction rule id %d from line %u, col %u\n", __func__, id->rule->id, parser->Parser.tokens.bins[id->token_key].coords.line, parser->Parser.tokens.bins[id->token_key].coords.col);
    PeggyProduction prod;
    STACK_INIT(PeggyString)(&prod.args, 0);

    prod.name = get_string_from_parser(parser, id);
    prod.identifier.len = parser->export.len + 1 + prod.name.len;
    prod.identifier.str = malloc(sizeof(char) * prod.identifier.len); // +1 for underscore
    char * buffer = copy_export(parser, prod.identifier.str);
    buffer[0] = '_';
    buffer++;
    memcpy(buffer, prod.name.str, prod.name.len);
    if (type) {
        prod.type_name = type;
    } else {
        prod.type_name = ((RuleType *) &Production_class)->type_name;
    }    

    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - adding build of production rule id %d with name %.*s from line %u, col %u to production map\n", __func__, id->rule->id, prod.name.len, prod.name.str, parser->Parser.tokens.bins[id->token_key].coords.line, parser->Parser.tokens.bins[id->token_key].coords.col);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    return prod;
}

void handle_base_rule(PeggyParser * parser, ASTNode * node, const PeggyString parent_id, PeggyString name) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling base rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    switch (node->children[0]->rule->id) {
        case TERMINAL: {
            // terminals are only built if parent_id is not empty
            if (parent_id.len) {
                handle_terminal(parser, node->children[0], parent_id);
            }
            break;
        }
        case NONTERMINAL: { // do nothing. Don't make a new production
            /* forward declare the nonterminal */
            PeggyString name = get_string_from_parser(parser, node->children[0]);
            
            if (!parser->productions._class->in(&parser->productions, name)) {
                if (strncmp(name.str, "punctuator", name.len) && strncmp(name.str, "keyword", name.len)) {
                    PeggyProduction_build(parser, node->children[0], NULL);
                } else {
                    PeggyProduction_build(parser, node->children[0], ((RuleType *) &LiteralRule_class)->type_name);
                }
            }
            
            break;
        }
        default: {// make a new production based on the choice expression
            handle_simplified_rules(parser, node->children[1], NULL_STRING, name);
        }
    }
}

void handle_simplified_rules(PeggyParser * parser, ASTNode * node, const PeggyString parent_id, PeggyString name) {
    
    // This check is very critical
    // If the name already exists (which is any rule other than production definitions themselves), 
    // this will leak memory, cause double-frees and all sorts of havoc that is hard to troubleshoot
    PeggyProduction prod; // may not be used
    if (!parser->productions._class->get(&parser->productions, name, &prod)) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - repeated rule id %d with name %.*s already exists, skipping\n", __func__, node->rule->id, (int)name.len, name.str);
        return;
    }

    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling simplified rule rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    switch (node->rule->id) {
        case CHOICE: {
            handle_choice(parser, node, name);
            break;
        }
        case SEQUENCE: {
            handle_sequence(parser, node, name);
            break;
        }
        case REPEATED_RULE: {
            handle_repeated_rule(parser, node, name);
            break;
        }
        case LIST_RULE: {
            handle_list_rule(parser, node, name);
            break;
        }
        case LOOKAHEAD_RULE: {
            handle_lookahead_rule(parser, node, name);
            break;
        }
        case BASE_RULE: { // BASE_RULE
            handle_base_rule(parser, node, parent_id, name);
            break;
        }
        default: {
            LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - trying to simplify an unknown rule id %d with type %s from line %u, col %u\n", __func__, node->rule->id, node->rule->_class->type_name, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
        }
    }
    
}

#define HANDLE_PRODUCTION_NONE 0
#define HANDLE_PRODUCTION_TOKEN 1
void handle_production_(PeggyParser * parser, ASTNode * id, ASTNode * transforms, ASTNode * rule_def, unsigned int flags) {
    PeggyProduction prod;
    PeggyString prod_name = get_string_from_parser(parser, id);
    if (parser->productions._class->get(&parser->productions, prod_name, &prod)) {
        // if retrieval fails, build a new one
        prod = PeggyProduction_build(parser, id, NULL);
    }
    PeggyProduction_declare(parser, prod);
    
    PeggyString name = get_string_from_parser(parser, rule_def);
    handle_simplified_rules(parser, rule_def, prod.identifier, get_string_from_parser(parser, rule_def));
    prod.args._class->push(&prod.args, get_rule_pointer(parser, name));

    if (flags & HANDLE_PRODUCTION_TOKEN) {
        char const * action = "token_action";
        size_t action_length = strlen(action);
        char * buffer = malloc(sizeof(char) * action_length);
        memcpy((void *)buffer, action, action_length);
        prod.args._class->push(&prod.args, (PeggyString){.str = buffer, .len = action_length});
    } else {
        if (transforms) {
            for (size_t i = 0; i < transforms->nchildren; i += 2) {
                PeggyString transform_name = get_string_from_parser(parser, transforms->children[i]);
                
                char * buffer = malloc(sizeof(char) * (transform_name.len + 1));
                memcpy((void *)buffer, transform_name.str, transform_name.len);
                prod.args._class->push(&prod.args, (PeggyString){.str = buffer, .len = transform_name.len});
            }
        }
    }

    // since productions are not allocated objects on the hash map, have to push to map again
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - adding production rule id %d from line %u, col %u to production map\n", __func__, id->rule->id, parser->Parser.tokens.bins[id->token_key].coords.line, parser->Parser.tokens.bins[id->token_key].coords.col);
    parser->productions._class->set(&parser->productions, prod.name, prod);
}

void handle_production(PeggyParser * parser, ASTNode * node) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling production rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);

    handle_production_(parser, node->children[0], node->children[1]->nchildren ? node->children[1]->children[0]->children[1] : NULL, node->children[3], HANDLE_PRODUCTION_NONE);
}

#define REGEX_LIB_STRING_LEFT "\""
#define REGEX_LIB_OFFSET_LEFT 1
#define REGEX_LIB_STRING_RIGHT "\""
#define REGEX_LIB_OFFSET_RIGHT 1

// returns one after last written character or dest if error
PeggyString format_regex(char const * src, size_t src_length) {
    static char const * const escaped_chars = ".^$*+?()[{\\|";

    size_t buffer_size = src_length + 1 + REGEX_LIB_OFFSET_LEFT + REGEX_LIB_OFFSET_RIGHT;
    char * buffer = malloc(sizeof(char) * buffer_size);
    size_t len = 0;

    buffer[len++] = '"';

    for (size_t i = 0; i < src_length; i++) {
        if (len >= buffer_size - 2) {
            buffer_size += 2 * (src_length - i) + 3;
            buffer = realloc(buffer, sizeof(char) * buffer_size);
        }
        if (strchr(escaped_chars, *src)) {
            buffer[len++] = '\\';
            buffer[len++] = '\\';
        }
        buffer[len++] = *src;
        src++;
    }
    if (len > buffer_size - 3) {
        buffer_size = len + 3;
        buffer = realloc(buffer, sizeof(char) * buffer_size);
    }

    buffer[len++] = '"';
    buffer[len] = '\0';

    return (PeggyString){.str = buffer, .len = len};
}

char * PUNCTUATION_LOOKUP[][2] = {
    {"!", "exclaim"},
    {"\"", "dquote"},
    {"#", "pound"},
    {"$", "dollar"},
    {"%", "modulus"},
    {"&", "ampersand"},
    {"'", "squote"},
    {"(", "lparen"},
    {")", "rparen"},
    {"*", "asterisk"},
    {"+", "plus"},
    {",", "comma"},
    {"-", "minus"},
    {".", "period"},
    {"/", "slash"},
    {":", "colon"},
    {";", "semicolon"},
    {"<", "langle"},
    {"=", "equals"},
    {">", "rangle"},
    {"?", "question"},
    {"@", "at_sign"},
    {"[", "lbracket"},
    {"\\", "backslash"},
    {"]", "rbracket"},
    {"^", "caret"},
    {"_", "underscore"},
    {"`", "backtick"},
    {"{", "lbrace"},
    {"|", "vbar"},
    {"}", "rbrace"},
    {"~", "tilde"},
    {"...", "ellipsis"},
    {"<<=", "lshifteq"},
    {">>=", "rshifteq"},
    {"->", "rarrow"},
    {"++", "increment"},
    {"--", "decrement"},
    {"&=", "andeq"},
    {"##", "dpound"},
    {"*=", "timeseq"},
    {"+=", "pluseq"},
    {"-=", "minuseq"},
    {"/=", "diveq"},
    {"%=", "modeq"},
    {"<=", "leq"},
    {">=", "req"},
    {">>", "rshift"},
    {"==", "equiv"},
    {"!=", "neq"},
    {"^=", "xoreq"},
    {"||", "logor"},
    {"&&", "logand"},
    {"|=", "oreq"},
    {"<<", "lshift"},
    {NULL, NULL},
};

char * punctuator_lookup(char * punctuation, size_t len) {
    size_t i = 0;
    char * punc;
    while ((punc = PUNCTUATION_LOOKUP[i][0])) {
        if (strlen(punc) == len && !strncmp(punc, punctuation, len)) {
            break;
        }
        i++;
    }
    if (punc) {
        return PUNCTUATION_LOOKUP[i][1];
    }
    // eww global variable direct access in low-level function
    LOG_EVENT(&peggy.Parser.logger, LOG_LEVEL_WARN, "WARN: %s - punctuator lookup failed\n", __func__);
    return NULL;
}

void handle_string_literal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling string literal from line %u, col %u\n", __func__, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    PeggyProduction prod;
    STACK_INIT(PeggyString)(&prod.args, 0);
    
    prod.name = get_string_from_parser(parser, node);
    
    if (!strncmp("punctuator", parent_id.str, parent_id.len)) { // this is punctuation
        char * lookup_result = punctuator_lookup(prod.name.str + 1, prod.name.len - 2);
        if (lookup_result) {
            prod.identifier.len = strlen(lookup_result);
            prod.identifier.str = malloc(sizeof(char) * prod.identifier.len);
            memcpy((void*)prod.identifier.str, (void*)lookup_result, prod.identifier.len);    
        } else {
            prod.identifier.len = (prod.name.len - 2)*4 + 5;
            prod.identifier.str = malloc(sizeof(char) * prod.identifier.len);
            prod.identifier.str[0] = 'p';
            prod.identifier.str[1] = 'u';
            prod.identifier.str[2] = 'n';
            prod.identifier.str[3] = 'c';
            size_t j = 4; // target index in prod.identifier.str
            for (size_t i = 1; i < prod.name.len - 1; i++) {
                prod.identifier.str[j++] = '_';
                if (isalnum(prod.name.str[i]) || prod.name.str[i] == '_') {
                    prod.identifier.str[j++] = prod.name.str[i];
                } else {
                    sprintf(prod.identifier.str + j, "%.3d", prod.name.str[i]);
                    j += 3;
                }
            }
            prod.identifier.len = j;
        }
        
    } else if (!strncmp("keyword", parent_id.str, parent_id.len)) {
        prod.identifier.len = (prod.name.len - 2) + 3;
        prod.identifier.str = malloc(sizeof(char) * prod.identifier.len);
        memcpy((void*)prod.identifier.str, (void*)(prod.name.str + 1), prod.name.len-2);
        prod.identifier.str[prod.identifier.len-3] = '_';
        prod.identifier.str[prod.identifier.len-2] = 'k';
        prod.identifier.str[prod.identifier.len-1] = 'w';
    } else {
        prod.identifier.len = parent_id.len + 3;
        prod.identifier.str = malloc(sizeof(char) * prod.identifier.len);
        memcpy((void*)prod.identifier.str, (void*)parent_id.str, parent_id.len);
        prod.identifier.str[parent_id.len] = '_';
        prod.identifier.str[parent_id.len+1] = 'r';
        prod.identifier.str[parent_id.len+2] = 'e';
    } 
    
    prod.type_name = ((RuleType *) &LiteralRule_class)->type_name;

    // don't need to declare LiteralRules
    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    PeggyString arg = format_regex(prod.name.str + 1, prod.name.len - 2);

    // assign args
    prod.args._class->push(&prod.args, arg);

    parser->productions._class->set(&parser->productions, prod.name, prod);
}

void handle_regex_literal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling regex from line %u, col %u\n", __func__, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    PeggyProduction prod;
    STACK_INIT(PeggyString)(&prod.args, 0);
    
    prod.name = get_string_from_parser(parser, node);
    
    prod.identifier.len = parent_id.len + 3;
    prod.identifier.str = malloc(sizeof(char) * prod.identifier.len);
    memcpy((void*)prod.identifier.str, (void*)parent_id.str, parent_id.len);
    prod.identifier.str[parent_id.len] = '_';
    prod.identifier.str[parent_id.len+1] = 'r';
    prod.identifier.str[parent_id.len+2] = 'e';

    prod.type_name = ((RuleType *) &LiteralRule_class)->type_name;

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    PeggyString arg = {.str = malloc(sizeof(char) * (prod.name.len  - 1 + REGEX_LIB_OFFSET_LEFT + REGEX_LIB_OFFSET_RIGHT)), .len = 0};
    memcpy((void*)arg.str, REGEX_LIB_STRING_LEFT, REGEX_LIB_OFFSET_LEFT);
    arg.len += REGEX_LIB_OFFSET_LEFT;
    memcpy((void*)(arg.str + arg.len), (void*)(prod.name.str + 1), prod.name.len - 2);
    arg.len += prod.name.len - 2;
    memcpy((void*)(arg.str + arg.len), REGEX_LIB_STRING_RIGHT, REGEX_LIB_OFFSET_RIGHT + 1);
    arg.len += REGEX_LIB_OFFSET_RIGHT;

    // assign args
    prod.args._class->push(&prod.args, arg);

    parser->productions._class->set(&parser->productions, prod.name, prod);
}

char * copy_export(PeggyParser * parser, char * buffer) {
    memcpy((void *)buffer, (void *)parser->export.str, parser->export.len);
    return buffer + parser->export.len;
}

void handle_punctuator_keyword(PeggyParser * parser, ASTNode * node) {
    PeggyProduction prod;
    STACK_INIT(PeggyString)(&prod.args, 0);
    PeggyString parent_id = {.str = (node->children[0]->rule->id == PUNCTUATOR_KW) ? "punctuator" : "keyword"};
    parent_id.len = strlen(parent_id.str);
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling %s from line %u, col %u\n", __func__, parent_id.str, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);

    prod.name = get_string_from_parser(parser, node->children[0]);

    prod.identifier.len = parser->export.len + 1 + prod.name.len;
    prod.identifier.str = malloc(sizeof(char) * prod.identifier.len); // +1 for underscore
    char * buffer = copy_export(parser, prod.identifier.str);
    buffer[0] = '_';
    buffer++;
    memcpy(buffer, prod.name.str, prod.name.len);
    prod.type_name = ((RuleType *) &LiteralRule_class)->type_name;

    PeggyProduction_declare(parser, prod);
    parser->productions._class->set(&parser->productions, prod.name, prod);

    PeggyString arg;
    size_t ntokens = node->ntokens;
    Token * toks = parser->_class->Parser_class.get_tokens(&parser->Parser, node->children[2]);
    arg.len = toks[ntokens - 1].length + (size_t)(toks[ntokens - 1].string - toks[0].string);
    arg.len = 4 * arg.len + 1 + REGEX_LIB_OFFSET_LEFT + REGEX_LIB_OFFSET_RIGHT; 
    arg.str = malloc(sizeof(char) * arg.len); 
    size_t written = 0;
    arg.str[written++] = '"';

    size_t N = node->children[2]->nchildren;

    for (size_t i = 0; i < N; i+=2) {
        handle_string_literal(parser, node->children[2]->children[i], parent_id);

        PeggyString str_lit_name = get_string_from_parser(parser, node->children[2]->children[i]);
        PeggyProduction str_lit_prod;
        parser->productions._class->get(&parser->productions, str_lit_name, &str_lit_prod);

        char const * re = str_lit_prod.args.bins[0].str + 1; // remove starting "^
        size_t length = str_lit_prod.args.bins[0].len - 2; // remove starting "^ and ending "

        memcpy((void*)(arg.str + written), (void*)re, length);
        written += length;
        if (N > 1 && i < N - 2) {
            arg.str[written++] = '|';
        }
    }
    arg.str[written++] = '"';
    arg.len = written;
    arg.str[written] = '\0';

    // make args
    prod.args._class->push(&prod.args, arg);

    parser->productions._class->set(&parser->productions, prod.name, prod);
}

void handle_special_production(PeggyParser * parser, ASTNode * node) {
    switch (node->children[0]->rule->id) {
        case TOKEN_KW: {
            handle_production_(parser, node->children[0], NULL, node->children[2], HANDLE_PRODUCTION_TOKEN);
            break;
        }
        case PUNCTUATOR_KW: { }
            __attribute__((fallthrough)); // otherwise gcc -pedantic complains about fallthrough
        case KEYWORD_KW: {
            handle_punctuator_keyword(parser, node);
            break;
        }
        default: {
            printf("special prodution case of %d not yet implemented\n", node->children[0]->rule->id);
        }
    }
}

#define PREP_OUTPUT_VAR_BUFFER_SIZE 256
void prep_output_files(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - prepping output files\n", __func__);
    char var_buffer[PREP_OUTPUT_VAR_BUFFER_SIZE];
    char const * buffer = NULL;
    int nbytes = 0;

    // write header in .h file
    buffer = "/* this file is auto-generated, do not modify */\n#ifndef ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);
    buffer = "_H\n#define ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);
    buffer = "_H\n\n#include <peggy/utils.h>\n#include <peggy/rule.h>\n#include <peggy/parser.h>\n\ntypedef enum ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);
    buffer = "rule {\n\t";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);

    // write header in .c file

    nbytes = snprintf(var_buffer, PREP_OUTPUT_VAR_BUFFER_SIZE, "/* this file is auto-generated, do not modify */\n#include <peggy/parser_gen.h>\n#include \"%s\"\n", parser->header_name);
    fwrite(var_buffer, 1, nbytes, parser->source_file);
    fflush(parser->header_file);
}

err_type open_output_files(PeggyParser * parser) {
    size_t name_length = parser->export.len;
    parser->header_name = malloc(2* (sizeof(*parser->header_name) * (name_length + 3)));
    if (!parser->header_name) {
        return PEGGY_MALLOC_FAILURE;
    }
    
    memcpy((void*)parser->header_name, (void*)parser->export.str, name_length);
    parser->header_name[name_length] = '.';
    parser->header_name[name_length+1] = 'h';
    parser->header_name[name_length+2] = '\0';
    parser->source_name = parser->header_name + name_length + 3;
    memcpy((void*)parser->source_name, (void*)parser->export.str, name_length);
    parser->source_name[name_length] = '.';
    parser->source_name[name_length+1] = 'c';
    parser->source_name[name_length+2] = '\0';

    if (!(parser->header_file = fopen(parser->header_name, "w"))) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to open header file %s\n", __func__, parser->header_name);
        return PEGGY_FILE_IO_ERROR;
    }
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - opened header file %s\n", __func__, parser->header_name);
    if (!(parser->source_file = fopen(parser->source_name, "w"))) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to open source file %s\n", __func__, parser->source_name);
        return PEGGY_FILE_IO_ERROR;
    }
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - opened source file %s\n", __func__, parser->source_name);
    return PEGGY_SUCCESS;
}

void handle_import(PeggyParser * parser, ASTNode * node) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling import from line %u, col %u\n", __func__, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    PeggyString import = get_string_from_parser(parser, node);
    parser->imports._class->push(&parser->imports, import);

    if (!parser->source_file) { // export config not found
        open_output_files(parser);
        prep_output_files(parser);
    }

    char * buffer = "#include \"";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->source_file);
    PeggyString_fwrite(import, parser->source_file, PSFO_NONE);

    buffer = ".h\"\n";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->source_file);
    fflush(parser->source_file);
}

void cleanup_header_file(PeggyParser * parser) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - cleaning up header file %s\n", __func__, parser->header_name);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);
    char * buffer = "_NRULES\n} ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);

    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);
    buffer = "rule;";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);

    buffer = "\n\nextern Production ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = "_token;\nextern Production ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = "_";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = ";\n\nvoid ";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_NONE);

    buffer = "_dest(void);\n\n#endif //";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    PeggyString_fwrite(parser->export, parser->header_file, PSFO_UPPER);

    buffer = "_H\n";
    fwrite(buffer, sizeof(char), strlen(buffer), parser->header_file);
    fflush(parser->header_file);
}

void handle_export(PeggyParser * parser, ASTNode * node) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling export from line %u, col %u\n", __func__, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    ParserType * parser_class = parser->Parser._class;
    // node is a nonws_printable production
    if (parser->productions.fill || parser->source_file){ // || parser->keywords.fill || parser->punctuators.fill) {
        Token tok;
        parser_class->get(&parser->Parser, node->token_key, &tok);
        LOG_EVENT(&((Parser *)parser)->logger, LOG_LEVEL_WARN, "WARN: %s - cannot change export name after any productions have been defined or modules imported: line %u, col %u...ignoring\n", __func__, tok.coords.line, tok.coords.col);
        return;
    }
    if (parser->export_found) {
        Token tok;
        parser_class->get(&parser->Parser, node->token_key, &tok);
        LOG_EVENT(&((Parser *)parser)->logger, LOG_LEVEL_WARN, "WARN: %s - cannot have multiple exports in a grammar specification: line %u, col %u...ignoring\n", __func__, tok.coords.line, tok.coords.col);
        return;
    }
    size_t ntokens = node->ntokens;
    Token * toks = parser_class->get_tokens(&parser->Parser, node);
    parser->export = (PeggyString){.str = (char *)toks[0].string, .len = toks[ntokens-1].length};

    open_output_files(parser);
    prep_output_files(parser);
}


/*
Should probably include a hash map of handlers mapping PeggyString identifier -> void handle_[identifier](PeggyParser * parser, ASTNode *)
*/
void handle_config(PeggyParser * parser, ASTNode * node) {
    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_DEBUG, "DEBUG: %s - handling config at line %u, col %u\n", __func__, parser->Parser.tokens.bins[node->token_key].coords.line, parser->Parser.tokens.bins[node->token_key].coords.col);
    Token * tok = parser->Parser._class->get_tokens(&parser->Parser, node->children[0]);
    if (!tok) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to retrieve config name for rule id: %d\n", __func__, node->rule->id);
    }
    if (!strncmp("import", tok->string, tok->length)) {
        handle_import(parser, node->children[2]);
    } else if (!strncmp("export", tok->string, tok->length)) {
        handle_export(parser, node->children[2]);
    } else {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - config type not understood with rule id: %d\n", __func__, node->rule->id);
    }
}

ASTNode * handle_peggy(Production * peggy_prod, Parser * parser_, ASTNode * node) {
    LOG_EVENT(&parser_->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - Found %zu document elements\n", __func__, node->nchildren);
    PeggyParser * parser = (PeggyParser *)parser_;
    parser_->ast = node;
    
    // node is a peggy document production
    for (size_t i = 0; i < node->nchildren; i++) {
        ASTNode * child = node->children[i];
        
        switch (child->rule->id) {
            case CONFIG: {
                handle_config(parser, child);
                break;
            }
            case SPECIAL_PRODUCTION: {
                handle_special_production(parser, child);
                break;
            }
            case PRODUCTION: {
                handle_production(parser, child);
                break;
            }
            default: {
                LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - document element not understood: %d. Expected a config or production.\n", __func__, child->rule->id);
            }
        }
        
    }

    cleanup_header_file(parser);

    LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_INFO, "INFO: %s - building definitions\n", __func__);
    parser->productions._class->for_each(&parser->productions, &PeggyProduction_define, (void*)parser);

    build_export_rules(parser);

    build_destructor(parser);
    
    return node;
}

ASTNode * simplify_rule(Production * simplifiable_rule, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - trying to simplifying rule id %d from line %u, col %u\n", __func__, node->rule->id, parser->tokens.bins[node->token_key].coords.line, parser->tokens.bins[node->token_key].coords.col);
    rule_id_type cur_rule = ((Rule *)simplifiable_rule)->id;
    switch (cur_rule) {
        case LOOKAHEAD_RULE: {
            if (!(node->children[0]->nchildren)) {
                return node->children[1];
                
            }
            break;
        }
        case REPEATED_RULE: {
            //printf("repeated rule found with %zu children. is actually repeated rule? %s\n", node->nchildren, node->children[1]->nchildren ? "yes": "no");
            if (!node->children[1]->nchildren) {
                return node->children[0];
            }
            break;
        }
        case LIST_RULE: {
        }
            __attribute__((fallthrough)); // otherwise gcc -pedantic complains about fallthrough
        case SEQUENCE: {
        }
            __attribute__((fallthrough)); // otherwise gcc -pedantic complains about fallthrough
        case CHOICE: {
            if (node->nchildren == 1) {
                return node->children[0];
            }
            break;
        }
        default: {
            LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - rule id %d was not simplified\n", __func__, node->rule->id);
            return node;
        }
    }    

    LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - re-initializing node rule from id %d to id %d; no node generated\n", __func__, node->rule->id, cur_rule);
    node->_class->init(node, (Rule *)simplifiable_rule, node->token_key, node->ntokens, node->str_length, node->nchildren, node->children);
    return node;

}



#define PeggyParser_DEFAULT_INIT {._class = &PeggyParser_class, \
								.Parser = {._class = &(PeggyParser_class.Parser_class), \
									.token_rule = NULL, \
									.root_rule = NULL, \
									.name = NULL, \
									.log_file = NULL, \
                                    .logger = DEFAULT_LOGGER_INIT, \
									.loc_ = 0, \
									.tokens_length = 0, \
									.tokens_capacity = 0, \
									.disable_cache_check = false, \
									.ast = NULL \
								}, \
                                root = {0}, \
                                token = {0}, \
                                keyword = {0}, \
                                punctuator = {0}, \
                                header_name = NULL, \
                                source_name = NULL, \
                                export = NULL \
								}

err_type PeggyParser_init(PeggyParser * parser, char const * name, size_t name_length, char const * string, size_t string_length, char const * log_file, unsigned char log_level) {
    err_type err_status = parser->Parser._class->init(&(parser->Parser), name, name_length, string, 
		string_length, (Rule *)&peggy_token, 
		(Rule *)&peggy_peggy, PEGGY_NRULES, 0, 0, PARSER_LAZY, log_file, log_level);
    LOG_EVENT(&((Parser*)parser)->logger, LOG_LEVEL_INFO, "INFO: %s - initializing peggy parser attributes\n", __func__);
    if (err_status) {
        return err_status;
    }
    hash_map_err status = HM_SUCCESS;
    if ((STACK_INIT(PeggyString)(&parser->imports, 0))) {
        status = HM_MALLOC_PAIR_FAILURE;
        goto parser_import_stack_fail;
    }
    if ((status = HASH_MAP_INIT(PeggyString, PeggyProduction)(&parser->productions, 0))) {
        goto parser_productions_map_fail;
    }
    parser->export.str = (char *)name;
    parser->export.len = name_length;
    parser->Parser._class->parse((Parser *)parser);
    if (parser->Parser.ast == &ASTNode_fail) {
        return PEGGY_FAILURE;
    }
    return PEGGY_SUCCESS;

parser_productions_map_fail:
    parser->imports._class->dest(&parser->imports);
parser_import_stack_fail:
	return PEGGY_INIT_FAILURE;
}

int PeggyProduction_cleanup(void * data, PeggyString name, PeggyProduction prod) {
    if (prod.identifier.len) {
        free(prod.identifier.str);
    }
    PeggyString res;
    err_type (*pop)(STACK(PeggyString) *, PeggyString *) = prod.args._class->pop;
    while (prod.args.fill) {
        pop(&prod.args, &res);
        free(res.str);
    }
    prod.args._class->dest(&prod.args);
    return 0;
}

void PeggyParser_dest(PeggyParser * parser) {
    LOG_EVENT(&((Parser*)parser)->logger, LOG_LEVEL_INFO, "INFO: %s - destroying peggy parser attributes\n", __func__);
    if (parser->header_file) {
        fclose(parser->header_file);
        parser->header_file = NULL;
    }
    if (parser->source_file) {
        fclose(parser->source_file);
        parser->source_file = NULL;
    }
    if (parser->header_name) {
        free(parser->header_name);
        parser->header_name = NULL;
    }
    
    parser->productions._class->for_each(&parser->productions, &PeggyProduction_cleanup, NULL);
	parser->productions._class->dest(&(parser->productions));
    parser->imports._class->dest(&parser->imports);
    parser->_class->Parser_class.dest(&parser->Parser);
}

void PeggyParser_parse(Parser * self) {
	LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - initiating parse\n", __func__);
    self->ast = self->root_rule->_class->check(self->root_rule, self);
    if (self->ast == NULL) {
        LOG_EVENT(&self->logger, LOG_LEVEL_ERROR, "ERROR: %s - parser failure. null node returned.\n", __func__);
    } else if (self->ast == &ASTNode_fail) {
        LOG_EVENT(&self->logger, LOG_LEVEL_ERROR, "ERROR: %s - parser failure\n", __func__); // TODO: need better messaging
    } else if (self->ast != &ASTNode_fail && self->tokens.bins[self->tokens.fill - 1].length) {
        LOG_EVENT(&self->logger, LOG_LEVEL_ERROR, "ERROR: %s - tokenizer failure. string was not fully tokenized. Remaining string of length %zu at line %u, col %u\n", __func__, self->tokens.bins[self->tokens.fill - 1].length, self->tokens.bins[self->tokens.fill - 1].coords.line, self->tokens.bins[self->tokens.fill - 1].coords.col);
    } else {
        LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - parsing successful\n", __func__);
    }
}

err_type from_string(char const * string, size_t string_length, char const * name, size_t name_length, char const * log_file, unsigned char log_level) {
    return peggy._class->init(&peggy, name, name_length, string, string_length, log_file, log_level);
}

err_type from_file(char const * filename, char const * log_file, unsigned char log_level) {
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to open file %s\n", __func__, filename);
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return PEGGY_MALLOC_FAILURE;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to read file: %s\n", __func__, filename);
        free(string);
        return PEGGY_FILE_IO_ERROR;
    }
    string[file_size] = '\0';

    fclose(pfile);

    size_t name_length = strlen(filename);
    char const * name = filename;

    // set name of the parser
    if (strchr(name, '/')) {
        name = strrchr(name, '/');
    }
    if (strchr(name, '\\')) {
        name = strrchr(name, '\\');
    }
    if (strstr(name, ".grmr")) {
        name_length = (size_t)(strstr(name, ".grmr") - name);
    }
    err_type status = from_string(string, (size_t) file_size, filename, name_length, log_file, log_level);
    if (status) {
        return status;
    }

    free(string);
    return PEGGY_SUCCESS;
}


int main(int narg, char ** args) {
    char const * input_file = NULL;
    char const * log_file = NULL;
    unsigned char log_level = LOG_LEVEL_INFO;
    if (narg > 1) {
        input_file = args[1];
    }
    if (narg > 2) {
        log_file = args[2];
    }
    if (narg > 3) {
        log_level = Logger_level_to_uchar(args[3], strlen(args[3]));
    }
    from_file(input_file, log_file, log_level);
    peggy._class->dest(&peggy);   
    peggy_dest();
    
    Logger_tear_down();
    return 0;
}
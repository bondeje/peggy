#ifndef PEGGY_PARSER_H
#define PEGGY_PARSER_H

/* C std lib includes */
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/* lib includes */
#include "peggy/mempool.h"

/* peggy include */
#include "peggy/utils.h"
#include "peggy/token.h"
#include "peggy/rule.h"
#include "peggy/packrat_cache.h"

#define PARSER_DEFAULT_NTOKENS 256
#define PARSER_DEFAULT_NNODES 4096

/******************* Parser definitions and declarations *********************/

#ifdef PACKRAT_HASH
#define ELEMENT_TYPE pASTNode
#include <peggy/stack.h>
#endif

#define Parser_tell(pparser) (((Parser *)pparser)->token_cur)
#define Parser_seek(pparser, ptoken) (((Parser *)pparser)->token_cur = ptoken)
#define Parser_fail_node(pparser) (((Parser *)pparser)->fail_node)
#define Parser_fail(pparser, irule) do { \
    if (Parser_tell(pparser) && (Parser_tell(pparser)->id >= Parser_fail_node(pparser)->token_start->id)) {\
        Parser_fail_node(pparser)->rule = irule;\
        Parser_fail_node(pparser)->token_start = Parser_tell(pparser);\
    } } while (0)
//#define Parser_fail(pparser, prule) Parser_fail_node(pparser)
#define Parser_is_fail_node(pparser, node) (node == Parser_fail_node(pparser))

struct Parser {
    struct ParserType * _class; //!< the Parser vtable
    PackratCache cache;         //!< the memoization cache for packrat parsing
    MemPoolManager * node_mgr;      //!< a memory pool manager for the ASTNode
                                    //!<    instances
    MemPoolManager * token_mgr;     //!< a memory pool manager for the Token
                                    //!<    instances
    MemPoolManager * childarr_mgr;  //!< a memory pool manager for the ASTNode
                                    //!<    child arrays.
    Token * token_head;     //!< a dummy Token * instance to serve as head of 
                            //!<    linked list of Tokens
    Token * token_cur;      //!< the current token being tokenized/parsed
    size_t ntokens;         //!< the number of tokens tokenized (serves as UID
                            //!<    generator. DO NOT MODIFY unless you are 
                            //!<    overriding "add_token" or "tokenize"
    Token * token_tail;     //!< the sentinel final Token in the linked list.
                            //!<    It is never a valid Token for parsing
    Rule ** rules;          //!< the rules in the parser
    Rule * token_rule;      //!< the "Rule *" applied iteratively to generate
                            //!<    the token stream
    Rule * root_rule;       //!< the "Rule *"" instance that initiates parsing
    bool tokenizing;        //!< a flag used during tokenization to control
                            //!<    cache handling. DO NOT MODIFY unless you
                            //!<    are overriding "tokenize"
    ASTNode * ast;          //!< Where the resulting AST root node is stored
                            //!<    It is NULL when parsing has not taken place
                            //!<    Parser_is_fail_node(&parser, parser.ast)
                            //!<        is true if parse failed without AST
                            //!<    It is a valid node if any partial AST exists
    ASTNode * fail_node;    //!< A pointer to a sentinel node used to indicate
                            //!<    failure of a rule
    unsigned int flags;     //!< Flags to control certain aspects of parsing.
                            //!<    not currently in use.
};

typedef struct ParserType ParserType;

/**
 * vtable for Parser. These are the function that I anticipate can be overridden
 * by users
 */
extern struct ParserType {
    // tokenizer interface
    size_t (*tokenize)(Parser * self, char const * string, size_t string_length, 
        Token ** start, Token ** end);
    // parser interface
    void (*parse)(Parser * parser, char const * string, size_t string_length);
    
} Parser_class;

/**
 * @brief allocate a new Parser instance
 * @param[in] token_rule the "Rule *" instance that is called to initiate the 
 *      the tokenizer/lexer to split input string into tokens
 * @param[in] root_rule the "Rule *" instance that initiates the parse upon
 *      its success stores the final node in ast
 * @param[in] flags to be used to control parsing
 */
Parser * Parser_new(Rule * rules[], rule_id_type nrules, rule_id_type token_rule, 
    rule_id_type root_rule, unsigned int flags);

/**
 * @brief initialize a Parser instance. Only call once per instance
 * @param[in] token_rule the "Rule *" instance that is called to initiate the 
 *      the tokenizer/lexer to split input string into tokens
 * @param[in] root_rule the "Rule *" instance that initiates the parse upon
 *      its success stores the final node in ast
 * @param[in] flags to be used to control parsing
 */
err_type Parser_init(Parser * parser, Rule * rules[], rule_id_type nrules, 
    rule_id_type token_rule, rule_id_type root_rule, unsigned int flags);

/**
 * @brief destroy and reclaim memory in parser. Use directly if initialized 
 *      with Parser_inint
 */
void Parser_dest(Parser * parser);

/**
 * @brief destroy and delete a Parser created with Parser_new
 */
void Parser_del(Parser * parser);

/**
 * @brief run the tokenizer on the input string. A linked list starting from 
 *      "start" to "end" (inclusive) is created
 * @param[in] self Parser instance
 * @param[in] string the input string to tokenize
 * @param[in] string_length the length of the input string
 * @param[out] start the resulting starting token
 * @param[out] end the resulting ending token.
 * @returns the number of tokens created
 */
size_t Parser_tokenize(Parser * self, char const * string, size_t string_length, Token ** start, Token ** end);

/**
 * @brief add a token for the given node. Only used during tokenization
 * @param[in] self Parser instance
 * @param[in] node node encapsulating the token to be added to the stream
 * @returns non-zero on error, else 0
 */
err_type Parser_add_token(Parser * self, ASTNode * node);

/**
 * @brief wrapper to retrieve number of tokens in parser. This is a more 
 *      accurate measure of the current state of the parser tokens than 
 *      Parser.ntokens
 */
size_t Parser_get_ntokens(Parser * self);

/**
 * @brief prints the tokens after tokenization in the parser
 */
void Parser_print_tokens(Parser * self, FILE * stream);

/**
 * @brief initiate the parsing stage. wraps the tokenizing stage so a separate 
 *      call to Parser_tokenize is not necessary
 * @param[in] self Parser instance
 * @param[in] string the input string to parser
 * @param[in] string_length the length of the input string
 */
void Parser_parse(Parser * parser, char const * string, size_t string_length);

/**
 * @brief create a new node and add its chidlren to parse tree. For internal
 *      use or overriding purposes only.
 * @param[in] self Parser instance
 * @param[in] rule Rule * instance to associate to node for its creation
 * @param[in] start The start Token at which the Rule * or node succeeded
 * @param[in] end The end Token at which the Rule completes.
 *      For "nonempty" nodes that represent at least 1 Token, the end Token 
 *          should be reachable traversing to the "right"/"next" from the 
 *          start Token, but it can be equal to the start node
 *      For nodes that do not consume tokens (Lookahead rules or RepeatRule 
 *          with 0 minimum repeats), end should be equal to start->prev (so 
 *          that the next rule starts at end->next == start)
 * @param[in] str_length During tokenization, this represents how much of the
 *      input string is captured by the node. Otherwise it is used as a flag
 *      and may not have this meaning anymore. Best not to use it. If you 
 *      need to estimate how much of a string to allocate, follow the Token
 *      linked list through a node accountingfor the fact that 
 *      ASTNode.token_end may be equal to ASTNode.token_start or in the case
 *      of parsing, may actually be one before ASTNode.token_start
 * @param[in] size This is used for the allocation size of the node. This 
 *      allows for the creation and memory management of custom nodes. To
 *      use the default ASTNode provided by peggy, this value may be 0 or
 *      sizeof(ASTNode). For creating a custom node, the custom node MUST
 *      have an ASTNode instance as its firts member and pass the value
 *      sizeof(MyCustomASTNode). Any initializtion or overriding of variables
 *      must be done after this function call
 * @returns a new pointer to an ASTNode *.
 */
ASTNode * Parser_add_node(Parser * self, rule_id_type rule, Token * start, Token * end, size_t str_length, size_t nchildren, size_t size);

/**
 * @brief check for a (Rule, Token) pair in the Parser's PackratCache
 * @param[in] self the Parser
 * @param[in] rule_id the integer ID of the Rule corresponding to the query
 *      usually this is Rule.id
 * @param[in] tok pointer to Token of the corresponding query
 * @returns The ASTNode at the (Rule, Token) key. NULL if not present
 */
ASTNode * Parser_check_cache(Parser * self, rule_id_type rule_id, Token * tok);

/**
 * @brief store a resulting ASTNode in the Parser's PackratCache
 * @param[in] self the Parser
 * @param[in] rule_id the integer ID of the Rule corresponding to the query
 *      usually this is Rule.id
 * @param[in] tok pointer to Token of the corresponding query
 * @param[in] node the ASTNode pointer to be stored
 */
void Parser_cache_check(Parser * self, rule_id_type rule_id, Token * tok, ASTNode * node);

/**
 * @brief print the available Abstract Syntax Tree to stream
 */
err_type Parser_print_ast(Parser * parser, FILE * stream);

/**
 * @brief get the status of a call to Parser_parse
 * @returns PEGGY_TOKENIZE_FAILURE, PEGGY_PARSER_FAILURE on corresponding errors
 *      or 0/PEGGY_SUCCESS on success
 */
err_type Parser_parse_status(Parser * parser);

/**
 * @brief get the status of a call to Parser_parse as well as print the AST and
 *      any error information to stream
 * @param[in] self the Parser
 * @param[in] stream the stream to print output to
 * @returns result of Parser_parse_status
 */
err_type Parser_print_parse_status(Parser * self, FILE * stream);

// Not yet implemented
err_type Parser_traverse(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);

/**
 * @brief the build action to be use by Rule instances that result in skipping 
 *      tokens during tokenization
 * @param[in] production The pointer to the Production instance that is 
 *      assigned skip_token build action
 * @param[in] parser the pointer to the running Parser instance
 * @param[in] node the resulting node of the production Rule's dependent rule
 * @returns On failure of the production or dependent rule, returns a node for 
 *      which Parser_is_fail_node(parser, return) evaluates to true or a node
 *      for which is_skip_token(return) evaluates to true. It should never 
 *      return NULL
 */
ASTNode * skip_token(Production * production, Parser * parser, ASTNode * node);

/**
 * @brief the build action to be used only by the special "token" Production
 * @param[in] production The pointer to the Production instance that is 
 *      assigned skip_token build action
 * @param[in] parser the pointer to the running Parser instance
 * @param[in] node the resulting node of the production Rule's dependent rule
 * @returns On failure of the production or dependent rule, returns a node for 
 *      which Parser_is_fail_node(parser, return) evaluates to true or a valid
 *      node. It should never return NULL
 */
ASTNode * token_action(Production * production, Parser * parser, ASTNode * node);

#endif // PEGGY_PARSER_H

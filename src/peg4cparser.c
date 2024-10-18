// TODO: fix Token/ASTNode APIs

/* C std lib */
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* lib includes */
#include "peg4c/mempool.h"

/* peg4c includes */
#include "peg4c/token.h"
#include "peg4c/rule.h"
#include "peg4c/stack.h"

/* local includes */
#include "peg4cbuild.h"
#include "peg4cparser.h"
#include "peg4c.h"
#include "peg4ctransform.h"

#define REGEX_SYMBOLS 128


struct P4CParserType P4CParser_class = {
    .Parser_class = {
        .parse = &Parser_parse,
        .tokenize = &Parser_tokenize
    }
};

struct P4CParser peg4c = {
    .Parser = {._class = &(P4CParser_class.Parser_class),
        .token_rule = NULL,
        .root_rule = NULL,
        .tokenizing = false,
        .ast = NULL
    },
    ._class = &P4CParser_class,
    .header_file = NULL,
    .source_file = NULL,
    .header_name = NULL,
    .source_name = NULL,
};

P4CString get_string_from_parser(P4CParser * parser, ASTNode * node) {
    Token * cur = node->token_start;
    Token * end = node->token_end;
    size_t length = 1 + cur->length;
    while (cur != end) {
        cur = cur->next;
        length += cur->length;
    }
    if (!length) { // don't malloc if no length
        return (P4CString) {0};
    }
    char * str = MemPoolManager_malloc(parser->str_mgr, length);
    P4CString out = {.str = str, .len = length - 1};
    length = 0;
    cur = node->token_start;
    length += snprintf(str + length, out.len + 1 - length, "%.*s", (int)cur->length, cur->string);
    while (cur != end) {
        cur = cur->next;
        length += snprintf(str + length, out.len + 1 - length, "%.*s", (int)cur->length, cur->string);
    }
    return out;
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
    {"\\\r\\\n", "CRLF"},
    {"\\\r", "CR"},
    {"\\\n", "LF"},
    {"\\\f", "FF"},
    {"\\\t", "TAB"},
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
    return NULL;
}

err_type P4CParser_init(P4CParser * parser, char const * name, size_t name_length) {
    err_type err_status = Parser_init((Parser *)parser, peg4crules, PEG4C_NRULES, PEG4C_TOKEN, PEG4C_PEG4C, 0);
    if (err_status) {
        return err_status;
    }
    parser->str_mgr = MemPoolManager_new(P4CSTRING_MEMPOOL_COUNT, P4CSTRING_BUFFER_SIZE, 1);
    hash_map_err status = HM_SUCCESS;
    if ((STACK_INIT(P4CString)(&parser->imports, 0))) {
        status = HM_MALLOC_PAIR_FAILURE;
        goto parser_import_stack_fail;
    }
    if ((status = HASH_MAP_INIT(P4CString, P4CProduction)(&parser->productions, 0))) {
        goto parser_productions_map_fail;
    }
    parser->export.str = (char *)name;
    parser->export.len = name_length;
    return P4C_SUCCESS;
parser_productions_map_fail:
    parser->imports._class->dest(&parser->imports);
parser_import_stack_fail:
	return P4C_INIT_FAILURE;
}

void P4CParser_dest(P4CParser * parser) {
    if (parser->header_file) {
        fclose(parser->header_file);
        parser->header_file = NULL;
    }
    if (parser->source_file) {
        fclose(parser->source_file);
        parser->source_file = NULL;
    }
    if (parser->header_name) {
        //free(parser->header_name);
        parser->header_name = NULL;
    }
    
    parser->productions._class->for_each(&parser->productions, &P4CProduction_cleanup, NULL);
	parser->productions._class->dest(&(parser->productions));
    parser->imports._class->dest(&parser->imports);
    //parser->symbol_map._class->dest(&parser->symbol_map);
    Parser_dest((Parser *)parser);
    MemPoolManager_del(parser->str_mgr);
}

err_type from_string(char const * string, size_t string_length, char const * name, size_t name_length) {
    err_type status = P4CParser_init(&peg4c, name, name_length);
    if (status) {
        return status;
    }
    Parser * parser = (Parser *)&peg4c;
    Parser_parse(parser, string, string_length);
    if (Parser_is_fail_node(parser, parser->ast)) {
        status = P4C_FAILURE;
    }
    return status;
}

err_type from_file(char const * filename) {
    FILE * pfile = fopen(filename, "rb");
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return P4C_MALLOC_FAILURE;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        free(string);
        return P4C_FILE_IO_ERROR;
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
    err_type status = from_string(string, (size_t) file_size, filename, name_length);
    if (status) {
        return status;
    }

    free(string);
    return P4C_SUCCESS;
}


int main(int narg, char ** args) {
    char const * input_file = NULL;
    char const * log_file = NULL;
    if (narg > 1) {
        input_file = args[1];
    }
    if (narg > 2) {
        log_file = args[2];
    }
    from_file(input_file);

    P4CParser_dest(&peg4c);
    peg4c_dest();
    
    return 0;
}


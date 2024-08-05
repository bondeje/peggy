// TODO: fix Token/ASTNode APIs

/* C std lib */
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

/* lib includes */
#include "peggy/logger.h"
#include "peggy/mempool.h"

/* peggy includes */
#include <peggy/token.h>
#include <peggy/rule.h>
#include <peggy/stack.h>

/* local includes */
#include "peggybuild.h"
#include "peggyparser.h"
#include "peggy.h"
#include "peggytransform.h"


struct PeggyParserType PeggyParser_class = {
    .Parser_class = {
        .type_name = PeggyParser_NAME,
        .add_token = &Parser_add_token, 
        .add_node = &Parser_add_node, 
        .parse = &Parser_parse,
        .tokenize = &Parser_tokenize
    }
};

struct PeggyParser peggy = {
    .Parser = {._class = &(PeggyParser_class.Parser_class),
        .token_rule = NULL,
        .root_rule = NULL,
        .name = "",
        .log_file = NULL,
        .logger = DEFAULT_LOGGER_INIT,
        .tokenizing = false,
        .ast = NULL
    },
    ._class = &PeggyParser_class,
    .header_file = NULL,
    .source_file = NULL,
    .header_name = NULL,
    .source_name = NULL,
};

PeggyString get_string_from_parser(PeggyParser * parser, ASTNode * node) {
    Token * cur = node->token_start;
    Token * end = node->token_end;
    size_t length = 1 + cur->length;
    while (cur != end) {
        cur = cur->next;
        length += cur->length;
    }
    if (!length) { // don't malloc if no length
        return (PeggyString) {0};
    }
    char * str = MemPoolManager_malloc(parser->str_mgr, length);
    PeggyString out = {.str = str, .len = length - 1};
    length = 0;
    cur = node->token_start;
    length += snprintf(str + length, out.len + 1 - length, "%.*s", (int)cur->length, cur->string);
    while (cur != end) {
        cur = cur->next;
        length += snprintf(str + length, out.len + 1 - length, "%.*s", (int)cur->length, cur->string);
    }
    return out;
}

char * copy_export(PeggyParser * parser, char * buffer) {
    memcpy((void *)buffer, (void *)parser->export.str, parser->export.len);
    return buffer + parser->export.len;
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
    LOG_EVENT(&peggy.Parser.logger, LOG_LEVEL_WARN, "WARN: %s - punctuator lookup failed: %.*s\n", __func__, len, punctuation);
    return NULL;
}

err_type PeggyParser_init(PeggyParser * parser, char const * name, size_t name_length, char const * log_file, unsigned char log_level) {
    err_type err_status = Parser_init((Parser *)parser, name, name_length, (Rule *)&peggy_token, (Rule *)&peggy_peggy, PEGGY_NRULES, PARSER_LAZY, log_file, log_level);
    //err_type err_status = parser->Parser._class->init(&(parser->Parser), name, name_length, (Rule *)&peggy_token, 
	//	(Rule *)&peggy_peggy, PEGGY_NRULES, PARSER_LAZY, log_file, log_level);
    LOG_EVENT(&((Parser*)parser)->logger, LOG_LEVEL_INFO, "INFO: %s - initializing peggy parser attributes\n", __func__);
    if (err_status) {
        return err_status;
    }
    parser->str_mgr = MemPoolManager_new(PEGGYSTRING_MEMPOOL_COUNT, PEGGYSTRING_BUFFER_SIZE, 1);
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
    return PEGGY_SUCCESS;

parser_productions_map_fail:
    parser->imports._class->dest(&parser->imports);
parser_import_stack_fail:
	return PEGGY_INIT_FAILURE;
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
        //free(parser->header_name);
        parser->header_name = NULL;
    }
    
    parser->productions._class->for_each(&parser->productions, &PeggyProduction_cleanup, NULL);
	parser->productions._class->dest(&(parser->productions));
    parser->imports._class->dest(&parser->imports);
    Parser_dest((Parser *)parser);
    MemPoolManager_del(parser->str_mgr);
}

err_type from_string(char const * string, size_t string_length, char const * name, size_t name_length, char const * log_file, unsigned char log_level) {
    err_type status = PeggyParser_init(&peggy, name, name_length, log_file, log_level);
    if (status) {
        return status;
    }
    Parser * parser = (Parser *)&peggy;
    Parser_parse(parser, string, string_length);
    if (parser->ast == NULL) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - parser failure. null node returned.\n", __func__);
    } else if (Parser_is_fail(parser, parser->ast)) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - parser failure\n", __func__); // TODO: need better messaging
    } else if (Parser_tell(parser)->next && Parser_tell(parser)->next->length) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - tokenizer failure. string was not fully tokenized. Remaining string of length %zu at line %u, col %u\n", __func__, Parser_tell(parser)->next->length, Parser_tell(parser)->next->coords.line, Parser_tell(parser)->next->coords.col);
    } else {
        LOG_EVENT(&parser->logger, LOG_LEVEL_INFO, "INFO: %s - parsing successful\n", __func__);
    }
    if (Parser_is_fail(parser, parser->ast)) {
        status = PEGGY_FAILURE;
    }
    return status;
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
    /*
    FILE * tokens = fopen("tokens.txt", "w");
    Parser_print_tokens((Parser *)&peggy, tokens);
    fclose(tokens);
    */
    PeggyParser_dest(&peggy);
    peggy_dest();
    
    Logger_tear_down();
    return 0;
}


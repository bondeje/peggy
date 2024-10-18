#ifndef P4CPARSER_H
#define P4CPARSER_H

#include "peg4c/parser.h"
#include "peg4cstring.h"

#define MAX_N_TRANSFORM_FUNCTIONS 1
#define MAX_FUNCTION_NAME_LEN 255
#define MAX_PRODUCTION_NAME_LEN 63

/* TODO: add a field to note where the production was defined for the case of troubleshooting duplicate productions */
typedef struct P4CProduction { 
    P4CString name; // the string identifying its definition. taken from input. prefixed by export name
    P4CString identifier; // allocated later to be an appropriate identifier for the associated Rule. Allocated. The enum name is the same name without a prefix and capitalized
    RuleTypeID type; // name of the type. This gives both the constructor and the resolution of the Rule
    STACK(P4CString) args; // arguments for generator constructors. All allocated. Any empty args indicates an incomplete definition but only Literals can be complete in first pass
} P4CProduction;

#define KEY_TYPE P4CString
#define VALUE_TYPE P4CProduction
#define HASH_FUNC P4CString_hash
#define KEY_COMP P4CString_comp
#include "peg4c/hash_map.h"

typedef struct P4CParser P4CParser;

err_type P4CParser_init(P4CParser * parser, char const * name, size_t name_length);
void P4CParser_dest(P4CParser * parser);

extern struct P4CParserType {
    struct ParserType Parser_class;
} P4CParser_class;

#define ELEMENT_TYPE P4CProduction
#include "peg4c/stack.h"

struct P4CParser {
	Parser Parser;
    struct P4CParserType * _class;
    HASH_MAP(P4CString, P4CProduction) productions;
    STACK(P4CString) imports;
    MemPoolManager * str_mgr;
    P4CString export;
    //HASH_MAP(pSymbol, pSymbol) symbol_map;
    char * header_name;
    FILE * header_file;
    char * source_name;
    FILE * source_file;
    bool export_found;
};

char * copy_export(P4CParser * parser, char * buffer);
P4CString get_string_from_parser(P4CParser * parser, ASTNode * node);
char * punctuator_lookup(char * punctuation, size_t len);

#endif // P4C_H

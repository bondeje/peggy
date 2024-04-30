#ifndef PEGGYPARSER_H
#define PEGGYPARSER_H

#include <peggy/parser.h>

#define MAX_N_TRANSFORM_FUNCTIONS 1
#define MAX_FUNCTION_NAME_LEN 255
#define MAX_PRODUCTION_NAME_LEN 63

typedef struct PeggyString {
    char * str;
    size_t len;
} PeggyString;

int PeggyString_comp(PeggyString a, PeggyString b);
size_t PeggyString_hash(PeggyString a, size_t bin_size);

#define ELEMENT_TYPE PeggyString
#define ELEMENT_COMP PeggyString_comp
#include <peggy/stack.h>

typedef struct PeggyProduction { 
    PeggyString name; // the string identifying its definition. taken from input. prefixed by export name
    PeggyString identifier; // allocated later to be an appropriate identifier for the associated Rule. Allocated. The enum name is the same name without a prefix and capitalized
    char const * type_name; // name of the type. This gives both the constructor and the resolution of the Rule
    STACK(PeggyString) args; // arguments for generator constructors. All allocated. Any empty args indicates an incomplete definition but only Literals can be complete in first pass
} PeggyProduction;

#define KEY_TYPE PeggyString
#define VALUE_TYPE PeggyProduction
#define HASH_FUNC PeggyString_hash
#define KEY_COMP PeggyString_comp
#include <peggy/hash_map.h>

typedef struct PeggyParser PeggyParser;

err_type PeggyParser_init(PeggyParser * parser, char const * name, size_t name_length, char const * string, size_t string_length);
void PeggyParser_dest(PeggyParser * parser);
void PeggyParser_parse(Parser * self);

#define PeggyParser_NAME "PeggyParser.Parser"

extern struct PeggyParserType {
    struct ParserType Parser_class;
    char const * type_name;
    err_type (*init)(PeggyParser * parser, char const * name, size_t name_length, char const * string, size_t string_length);
    void (*dest)(PeggyParser * parser);
} PeggyParser_class;

#define ELEMENT_TYPE PeggyProduction
#include <peggy/stack.h>

struct PeggyParser {
	Parser Parser;
    struct PeggyParserType * _class;
    //HASH_MAP(PeggyString, PeggyProduction) punctuators;
    //HASH_MAP(PeggyString, PeggyProduction) keywords;
    HASH_MAP(PeggyString, PeggyProduction) productions;
    STACK(PeggyString) imports;
    PeggyString export;
    char * header_name;
    FILE * header_file;
    char * source_name;
    FILE * source_file;
    bool export_found;
};

ASTNode * handle_peggy(Parser * parser, ASTNode * node);

ASTNode * simplify_rule(Parser * parser, ASTNode * node);

#endif // PEGGY_H

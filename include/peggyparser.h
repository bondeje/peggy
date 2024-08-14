#ifndef PEGGYPARSER_H
#define PEGGYPARSER_H

#include <peggy/parser.h>
#include <peggystring.h>

#define MAX_N_TRANSFORM_FUNCTIONS 1
#define MAX_FUNCTION_NAME_LEN 255
#define MAX_PRODUCTION_NAME_LEN 63

/* TODO: add a field to note where the production was defined for the case of troubleshooting duplicate productions */
typedef struct PeggyProduction { 
    PeggyString name; // the string identifying its definition. taken from input. prefixed by export name
    PeggyString identifier; // allocated later to be an appropriate identifier for the associated Rule. Allocated. The enum name is the same name without a prefix and capitalized
    RuleTypeID type; // name of the type. This gives both the constructor and the resolution of the Rule
    STACK(PeggyString) args; // arguments for generator constructors. All allocated. Any empty args indicates an incomplete definition but only Literals can be complete in first pass
} PeggyProduction;

#define KEY_TYPE PeggyString
#define VALUE_TYPE PeggyProduction
#define HASH_FUNC PeggyString_hash
#define KEY_COMP PeggyString_comp
#include <peggy/hash_map.h>

typedef struct PeggyParser PeggyParser;

err_type PeggyParser_init(PeggyParser * parser, char const * name, size_t name_length, char const * log_file, unsigned char log_level);
void PeggyParser_dest(PeggyParser * parser);

extern struct PeggyParserType {
    struct ParserType Parser_class;
} PeggyParser_class;

#define ELEMENT_TYPE PeggyProduction
#include <peggy/stack.h>

struct PeggyParser {
	Parser Parser;
    struct PeggyParserType * _class;
    HASH_MAP(PeggyString, PeggyProduction) productions;
    STACK(PeggyString) imports;
    MemPoolManager * str_mgr;
    PeggyString export;
    char * header_name;
    FILE * header_file;
    char * source_name;
    FILE * source_file;
    bool export_found;
};

char * copy_export(PeggyParser * parser, char * buffer);
PeggyString get_string_from_parser(PeggyParser * parser, ASTNode * node);
char * punctuator_lookup(char * punctuation, size_t len);

#endif // PEGGY_H

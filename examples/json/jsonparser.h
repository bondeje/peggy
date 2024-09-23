#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <stddef.h>

#include "peggy/hash_map.h"
#include "peggy/parser.h" // should remove this
#include "peggy/astnode.h"
#include "peggy/token.h"

#include "jason.h"

typedef struct JSONValue JSONValue;

#ifndef NULL_TYPE
    #define NULL_TYPE void *
    #define NULL_VALUE NULL
#else
    #ifndef NULL_VALUE
        #error must specify a compatible NULL_VALUE if overriding NULL_TYPE
    #endif
#endif

typedef enum JSONValueType {
    // base objects
    JSON_NOT_SET = 0, // value for an unset JSONValue (so a {0} initializer will set this)
    JSON_NULL, // null keyword.
    JSON_OBJECT, // a hash map <char *, JSONValue *>
    JSON_ARRAY,  // array of JSONValue
    JSON_STRING, // char * 
    JSON_DECIMAL, // double
    JSON_INTEGER, // long long. For now, this is only integer and will cover the boolean keywords
    JSON_BOOL,
    // special arrays when all values are uniform. These are reserved for future use cases
    JSON_INTEGER_ARRAY,
    JSON_DECIMAL_ARRAY,
    JSON_STRING_ARRAY,
    JSON_OBJECT_ARRAY,
    JSON_ARRAY_ARRAY,
    JSON_BOOL_ARRAY,
} JSONValueType;

typedef struct JSONString JSONString;

int JSONString_comp(JSONString a, JSONString b);
size_t JSONString_hash(JSONString a, size_t bin_size);

struct JSONString {
    char * str;
    size_t len;
};

typedef long long llong;

typedef JSONValue * pJSONValue;

static int pJSONValue_comp(pJSONValue a, pJSONValue b) {
    return 1;
}

#define ELEMENT_TYPE pJSONValue
#define ELEMENT_COMP pJSONValue_comp
#include <peggy/stack.h>

typedef STACK(pJSONValue) JSONArray;

#define KEY_TYPE JSONString
#define VALUE_TYPE pJSONValue
#define KEY_COMP JSONString_comp
#define HASH_FUNC JSONString_hash
#include <peggy/hash_map.h>
typedef HASH_MAP(JSONString, pJSONValue) JSONObject;

// default initialization will result in type == JSON_NOT_SET and .null == NULL
struct JSONValue {
    JSONValueType type;
    union {
        NULL_TYPE null;
        JSONObject object;
        JSONArray array;
        JSONString string;
        double decimal;
        llong integer;
        bool boolean;
    } value;
};

typedef struct JSONData {
    JSONValue * values;     // array for all the values
    char * strings;         // allocation for strings
    size_t nvalues;         // size of array for all the values
    size_t nvalues_used;    // context during traversal
    size_t string_size;     // how much space is allocated for string.
    size_t string_used;     // context during traversal
} JSONData;

typedef struct JSONDoc {
    JSONData data; // allocated structures holding all the data
    JSONValue * value; // top-level document structures (entry points)
} JSONDoc;

typedef struct JSONParser {
    Parser Parser;
    JSONDoc json;
} JSONParser;

ASTNode * build_string(Production * prod, Parser * parser_, ASTNode * node);
ASTNode * build_value(Production * prod, Parser * parser_, ASTNode * node);
JSONValue * handle_value(JSONParser * parser, ASTNode * node);
ASTNode * handle_json(Production * prod, Parser * parser, ASTNode * node);
JSONValue * JSONParser_get_next_JSONValue(JSONParser * parser);
JSONString JSONParser_get_next_JSONString(JSONParser * parser, ASTNode * node);

#endif


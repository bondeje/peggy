#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#include <peggy/hashmap.h>
#include <peggy/parser.h> // should remove this
#include <peggy/astnode.h>
#include <peggy/token.h>
#include <peggyparser.h>
#include "jsonparser.h"

typedef struct JSONValue JSonValue;

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

// TODO fix this for whatever is going to be my array type
typedef PeggyString JSONString;
typedef long long llong;

typedef HASH_MAP(JSONString, JSONValue) JSONObject;
typedef ARRAY(JSONValue) JSONArray;

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

typedef struct JSONValueTypeSize {
    JSONValueType type;
    size_t size;
} JSONValueTypeSize;

typedef struct JSONNode {
    ASTNode node;
    JSONValue * value; // it is a separate object
} JSONNode;

void handle_integer(Parser * parser, ASTNode * node, JSONVal * jval) {
    Token * tok = parser->_class->get_tokens(parser, node, NULL);
    size_t N = tok->_class->len(tok); // should probably flag error if > MAX_LLONG_STR_LEN - 1
    char * str = tok->string + tok->start;
    char stor = str[N]; // temporary storage of the character
    str[N] = '\0'; // make str a null-terminated C-string
    jval->value.decimal = strtoll(str, NULL, 10);
    str[N] = stor; // reset character
    jval->type = JSON_DECIMAL;
}

void handle_decimal(Parser * parser, ASTNode * node, double * decimal) {
    Token * tok = parser->_class->get_tokens(parser, node, NULL);
    size_t N = tok->_class->len(tok); // should probably flag error if > MAX_LLONG_STR_LEN - 1
    char * str = tok->string + tok->start;
    char stor = str[N]; // temporary storage of the character
    str[N] = '\0'; // make str a null-terminated C-string
    jval->value.decimal = strtod(str, NULL);
    str[N] = stor; // reset character
    jval->type = JSON_DECIMAL;
}

handle_value(Parser * parser, ASTNode * node) {
    ASTNode * child = node->children[0];
    switch (child->rule->id) {
        case OBJECT: {
            return build_object(parser, child);
        }
        case ARRAY: {
            return build_array(parser, child);
        }
        case TRUE_KW: {
        } __attribute__((fallthrough))
        case FALSE_KW: {
            return build_boolean(parser, child);
        }
        case NULL_VALUE: {
            return build_null(parser, child);
        }
        case STRING: {
            Token * tok = parser->_class->get_tokens(parser, node, NULL);
            jval->value.string.str = tok->string + tok->start;
            jval->value.string.len = tok->_class->len(tok);
            jval->type = JSON_STRING;
            break;
        }
        case NUMBER: {
            ASTNode * grandchild = child->children[0];
            switch (grandchild->rule->id) {
                case JSON_INTEGER: {
                    return build_integer(parser, grandchild);
                    break;
                }
                case JSON_DECIMAL: {
                    return build_decimal(parser, grandchild);
                    break;
                }
                default: {
                    /* TODO: error condition*/
                }
            }
            break;
        }
        default: {
            /* TODO: error condition*/
        }
    }
    // returns a JSONNode
    return NULL;
}



ASTNode * handle_json(Parser * parser, ASTNode * node) {
    return node;
}

int main(void) {
    return 0;
}
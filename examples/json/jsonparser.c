/* C standard library includes */
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* POSIX  includes */
#include <time.h>
#include <sys/types.h>

/* peggy includes */
#include "peggy/hash_map.h"
#include "peggy/parser.h" // should remove this
#include "peggy/astnode.h"
#include "peggy/token.h"

/* local includes */
#include "jsonparser.h"

// -9223372036854775807 + '\0'
#define MAX_LLONG_STR_LENGTH 21 

// SIGN, 53 fraction digits, DEC, e/E, SIGN, 4 digit exponent  + '\0' + a few bytes
#define MAX_DBL_STR_LENGTH 64

JSONParser json = {
    .Parser = {
        ._class = &Parser_class,
    }
};


void JSON_print_value_(JSONValue * val);

int JSONString_comp(JSONString a, JSONString b) {
    if (a.len != b.len) {
        return 1;
    }
    int res = strncmp(a.str, b.str, a.len);
    return res;
}

size_t JSONString_hash(JSONString a, size_t bin_size) {
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

JSONValue * JSONParser_get_next_JSONValue(JSONParser * parser) {
    if (parser->json.data.nvalues_used >= parser->json.data.nvalues) {
        printf("%s - not enought values allocated\n", __func__);
        return NULL;
    }
    return parser->json.data.values + parser->json.data.nvalues_used++;
}
JSONString JSONParser_get_next_JSONString(JSONParser * parser, ASTNode * node) {
    Token * tok = node->token_start;
    if (parser->json.data.string_used >= parser->json.data.string_size - tok->length + 2) {
        printf("%s - insufficient data to get next string\n", __func__);
        return (JSONString){0};
    }
    char * result = parser->json.data.strings + parser->json.data.string_used;
    memcpy(result, tok->string + 1, tok->length - 2); // copy value into allocated string
    parser->json.data.string_used += tok->length - 2; // + 1 for null-terminator
    return (JSONString){.str = result, .len = tok->length - 2};
}

ASTNode * build_string(Production * prod, Parser * parser_, ASTNode * node) {
    Token * tok = node->token_start;
    ((JSONParser *)parser_)->json.data.string_size += tok->length + 1; // add one for a null-terminator
    // not sure why this doesn't work. This would speed things up a little bit. The call to get_tokens is not cheap
    return build_action_default(prod, parser_, node);
}

ASTNode * build_value(Production * prod, Parser * parser_, ASTNode * node) {
    ((JSONParser *)parser_)->json.data.nvalues++; // record the number of values
    return build_action_default(prod, parser_, node);
}

JSONValue * handle_integer(JSONParser * parser, ASTNode * node) {
    static char integer_buffer[MAX_LLONG_STR_LENGTH] = {'\0'};
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    Token * tok = node->token_start;
    if (tok->length >= MAX_LLONG_STR_LENGTH) {
        return NULL;
    }
    memcpy(integer_buffer, tok->string, tok->length);
    integer_buffer[tok->length] = '\0';
    jval->value.integer = strtoll(integer_buffer, NULL, 10);
    jval->type = JSON_INTEGER;
    return jval;
}

JSONValue * handle_decimal(JSONParser * parser, ASTNode * node) {
    static char decimal_buffer[MAX_DBL_STR_LENGTH] = {'\0'};
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    Token * tok = node->token_start;
    if (tok->length >= MAX_DBL_STR_LENGTH) {
        /* this is technically correctable if the exponent is in range [0-2047] which means fraction bit is too long...trim it*/
        return NULL;
    }
    memcpy(decimal_buffer, tok->string, tok->length);
    decimal_buffer[tok->length] = '\0';
    jval->value.decimal = strtod(decimal_buffer, NULL);
    jval->type = JSON_DECIMAL;
    return jval;
}

JSONValue *  handle_number(JSONParser * parser, ASTNode * node) {
    ASTNode * child = node->children[0];
    switch (child->rule) {
        case JASON_INT_CONSTANT: {
            return handle_integer(parser, child);
        }
        case JASON_DECIMAL_FLOAT_CONSTANT: {
            return handle_decimal(parser, child);
        }
        default: {
            /* TODO: error condition */
        }
    }
    return NULL;
}

JSONValue * handle_string(JSONParser * parser, ASTNode * node) {
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    jval->value.string = JSONParser_get_next_JSONString(parser, node);
    jval->type = JSON_STRING;
    return jval;
}

JSONValue * handle_keyword(JSONParser * parser, ASTNode * node) {
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    switch (node->children[0]->rule) {
        case JASON_NULL_KW: {
            jval->value.null = NULL_VALUE;
            jval->type = JSON_NULL;
            break;
        }
        case JASON_TRUE_KW: {
            jval->value.boolean = true;
            jval->type = JSON_BOOL;
            break;
        }
        case JASON_FALSE_KW: {
            jval->value.boolean = false;
            jval->type = JSON_BOOL;
            break;
        }
        default: {
            /* TODO: error condition */
            return NULL;
        }
    }
    return jval;
}

JSONValue * handle_array(JSONParser * parser, ASTNode * node) {
    //printf("making array\n");
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    jval->value.array = (JSONArray){0};
    if (node->children[1]->nchildren) { // array has elements
        ASTNode * elements = node->children[1]->children[0];
        size_t nelements = (elements->nchildren + 1) >> 1;
        STACK_INIT(pJSONValue)(&jval->value.array, nelements);
        // assign all subsequent JSONValues, but we don't need to assign them to value.array as it is guaranteed to be contiguous (so long as this is single-threaded)
        for (size_t i = 0; i < elements->nchildren; i += 2) {
            jval->value.array._class->push(&jval->value.array, handle_value(parser, elements->children[i]));
        }
    }
    jval->type = JSON_ARRAY;
    return jval;
}

void handle_object_key(JSONParser * parser, ASTNode * node, JSONValue * obj, JSONValue * jval) {
    JSONString key = JSONParser_get_next_JSONString(parser, node);
    obj->value.object._class->set(&obj->value.object, key, jval);
}

void handle_member(JSONParser * parser, ASTNode * node, JSONValue * obj) {
    JSONValue * jval = handle_value(parser, node->children[2]);
    handle_object_key(parser, node->children[0], obj, jval);
}

JSONValue * handle_object(JSONParser * parser, ASTNode * node) {
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    jval->value.object = (JSONObject) {0};
    if (node->children[1]->nchildren) { // object has key-value pairs
        ASTNode * members = node->children[1]->children[0];
        size_t nmembers = (members->nchildren + 1) >> 1;
        HASH_MAP_INIT(JSONString, pJSONValue)(&jval->value.object, nmembers);
        for (size_t i = 0; i < members->nchildren; i += 2) {
            handle_member(parser, members->children[i], jval);
        }
    } else {
        HASH_MAP_INIT(JSONString, pJSONValue)(&jval->value.object, 1);
    }
    jval->type = JSON_OBJECT;
    return jval;
}

JSONValue * handle_value(JSONParser * parser, ASTNode * node) {
    ASTNode * child = node->children[0];
    switch (child->rule) {
        case JASON_OBJECT: {
            return handle_object(parser, child);
        }
        case JASON_ARRAY: {
            return handle_array(parser, child);
        }
        case JASON_KEYWORDS: {
            return handle_keyword(parser, child);
        }
        case JASON_STRING: {
            return handle_string(parser, child);
        }
        case JASON_NUMBER: {
            return handle_number(parser, child);
        }
        default: {
            printf("error in handle_value: %d %s\n", child->rule, jasonrules[child->rule]->name);
        }
    }
    return NULL;
}

void JSONDoc_init(JSONDoc * json) {
    json->data.nvalues_used = 0;
    json->data.string_used = 0;
    json->data.strings = calloc(json->data.string_size + 1024, 1);
    json->data.values = calloc(json->data.nvalues, sizeof(JSONValue));
}

void JSONDoc_dest(JSONDoc * json) {
    if (json->data.strings) {
        free(json->data.strings);
        json->data.strings = NULL;
        json->data.string_used = 0;
        json->data.string_size = 0;
    }
    if (json->data.values) {
        for (size_t i = 0; i < json->data.nvalues_used; i++) {
            if (json->data.values[i].type == JSON_OBJECT) {
                json->data.values[i].value.object._class->dest(&json->data.values[i].value.object);
            } else if (json->data.values[i].type == JSON_ARRAY) {
                json->data.values[i].value.array._class->dest(&json->data.values[i].value.array);
            }
        }
        free(json->data.values);
        json->data.values = NULL;
        json->data.nvalues_used = 0;
        json->data.nvalues = 0;
    }
}

void JSONParser_init(JSONParser * parser, char * log_file, unsigned char log_level) {
    parser->json = (JSONDoc) {0};
    Parser_init((Parser *)parser, jasonrules, JASON_NRULES, JASON_TOKEN, JASON_JSON, 0);
}

void JSONParser_dest(JSONParser * parser) {
    Parser_dest((Parser *)parser);
}

JSONValue * JSONArray_get(JSONArray * array, size_t index) {
    JSONValue * val = NULL;
    array->_class->get(array, index, &val);
    return val;
}

JSONValue * JSONObject_get(JSONObject * obj, JSONString jstr) {
    JSONValue * out = NULL;
    obj->_class->get(obj, jstr, &out);
    return out;
}

JSONValue * JSONObject_get_cstr(JSONObject * obj, char * key) {
    return JSONObject_get(obj, (JSONString){.str = key, .len = strlen(key)});
}

JSONValue * JSONObject_get_byvalue(JSONObject * obj, JSONValue * jval) {
    if (jval->type != JSON_STRING) {
        return NULL;
    }
    return JSONObject_get(obj, jval->value.string);
}

ASTNode * handle_json(Production * prod, Parser * parser_, ASTNode * node) {
    // allocate the structures for JSONDoc
    JSONParser * parser = (JSONParser *) parser_;
    JSONDoc_init(&parser->json);
    parser->json.value = handle_value(parser, node);
    return node;
}

/* globals */
_Bool timeit = false;

struct JSONDoc empty_json = {0};

JSONDoc * from_string(char * string, size_t string_length) {
    JSONParser_init(&json, NULL, 0);
    if (!timeit) {
        Parser_parse((Parser *)&json, string, string_length);
        if (Parser_is_fail_node((Parser *)&json, json.Parser.ast)) {
            return &empty_json;
        }
        return &json.json;
    }

#ifdef CLOCK_MONOTONIC
    static char const * const record_format = "%zu, %10.8lf\n";
    char buffer[1024] = {'\0'};
    FILE * time_records = fopen("times.csv", "ab");
    
    struct timespec t0, t1;
    double time;
    clockid_t clk = CLOCK_MONOTONIC;
    //double clock_conversion = 1.0e-6;
    clock_gettime(clk, &t0);
    Parser_parse((Parser *)&json, string, string_length);
    clock_gettime(clk, &t1);

    if (t1.tv_nsec < t0.tv_nsec) {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    } else {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    }

    snprintf(buffer, 1024, record_format, json.json.data.nvalues, time);
    fprintf(time_records, "%s", buffer);
    printf("%s", buffer);
    
    fclose(time_records);
    if (Parser_is_fail_node((Parser *)&json, json.Parser.ast)) {
        printf("parser failed\n");
        return &empty_json;
    }
#endif
    return &json.json;
}

JSONDoc * from_file(char * filename) {
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        return &empty_json;
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return &empty_json;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        free(string);
        return &empty_json;
    }
    string[file_size] = '\0';

    fclose(pfile);

    JSONDoc * json_data = from_string(string, (size_t) file_size);

    free(string);
    return json_data;
}

void JSON_print_bool(bool boolean) {
    printf("%s", boolean ? "true" : "false");
}

void JSON_print_null(NULL_TYPE null) {
    printf("null");
}

int JSON_print_object_item(void * data, JSONString key, JSONValue * val) {
    printf("\t%.*s: ", (int)key.len, key.str);
    JSON_print_value_(val);
    printf("\n");
    return 0;
}

void JSON_print_object(JSONObject * object) {
    printf("object:\n");
    object->_class->for_each(object, JSON_print_object_item, NULL);
}

void JSON_print_integer(llong integer) {
    printf("%lld", integer);
}

void JSON_print_decimal(double decimal) {
    printf("%f", decimal);
}

void JSON_print_string(JSONString string) {
    printf("%.*s", (int)string.len, string.str);
}

void JSON_print_array(JSONArray * array) {
    printf("array:\n");
    for (size_t i = 0; i < array->fill; i++) {
        printf("\t");
        JSON_print_value_(JSONArray_get(array, i));
        printf("\n");
    }
}

void JSON_print_value_(JSONValue * val) {
    switch (val->type) {
        case JSON_NULL: {
            JSON_print_null(val->value.null);
            break;
        }
        case JSON_BOOL: {
            JSON_print_bool(val->value.boolean);
            break;
        }
        case JSON_STRING: {
            JSON_print_string(val->value.string);
            break;
        }
        case JSON_INTEGER: {
            JSON_print_integer(val->value.integer);
            break;
        }
        case JSON_DECIMAL: {
            JSON_print_decimal(val->value.decimal);
            break;
        }
        case JSON_ARRAY: {
            JSON_print_array(&val->value.array);
            break;
        }
        case JSON_OBJECT: {
            JSON_print_object(&val->value.object);
            break;
        }
        default: {
            printf("%s: type not not understood: %d\n", __func__, val->type);
            return;
        }
    }
}

void JSON_print_value(JSONDoc * json_data, JSONValue * value, char const * index) {
    if (!value) {
        value = json_data->value;
        printf("index %s: ", index);
    }
    char const * slash = index;
    if (index) {
        slash = memchr(index, (int)'/', strlen(index));
    }
    
    size_t length = 0;
    if (!slash) {
        length = strlen(index);
    } else {
        length = (size_t)(slash - index);
    }
    switch (value->type) {
        case JSON_NULL: {
            if (!length) {
                JSON_print_null(value->value.null);
            } else {
                printf("error. found null but index remaining: %s\n", index);
                return;
            }
            break;
        }
        case JSON_BOOL: {
            if (!length) {
                JSON_print_bool(value->value.boolean);
            } else {
                printf("error. found bool but index remaining: %s\n", index);
                return;
            }
            break;
        }
        case JSON_STRING: {
            if (!length) {
                JSON_print_string(value->value.string);
            } else {
                printf("error. found string but index remaining: %s\n", index);
                return;
            }
            break;
        }
        case JSON_INTEGER: {
            if (!length) {
                JSON_print_integer(value->value.integer);
            } else {
                printf("error. found integer but index remaining: %s\n", index);
                return;
            }
            break;
        }
        case JSON_DECIMAL: {
            if (!length) {
                JSON_print_decimal(value->value.decimal);
            } else {
                printf("error. found decimal but index remaining: %s\n", index);
                return;
            }
            break;
        }
        case JSON_ARRAY: {
            if (!length) {
                JSON_print_array(&value->value.array);
            } else {
                static char integer_buffer[MAX_LLONG_STR_LENGTH] = {'\0'};
                memcpy(integer_buffer, index, length);
                long long i = atoll(integer_buffer);
                //printf("getting array value at index: %lld\n", i);
                if (i < 0 || (size_t)i >= value->value.array.fill) {
                    printf("error. index %lld is out of bounds, max: %zu\n", i, value->value.array.fill);
                    return;
                }
                value = JSONArray_get(&value->value.array, (size_t)i);
            }
            break;
        }
        case JSON_OBJECT: {
            if (!length) {
                JSON_print_object(&value->value.object);
            } else {
                JSONString key = {.len = length, .str = (char *)index};
                //printf("getting object value at key (%zu): %.*s\n", key.len, (int)key.len, key.str);
                JSONValue * new_value = JSONObject_get(&value->value.object, key);
                if (!new_value) {
                    printf("%.*s not found in object", (int)length, index);
                    JSON_print_object(&value->value.object);
                    return;
                } else {
                    value = new_value;
                }
            }
            break;
        }
        default: {
            printf("value not understood: %s\n", index);
            return;
        }
    }

    if (length) {
        JSON_print_value(json_data, value, index + length + (slash ? 1 : 0));
    }
    
    printf("\n");
}

int main(int narg, char ** args) {
    int iarg = 1;
    char const * index = NULL;
    while (iarg < narg) {
        printf("arg %d: %s\n", iarg, args[iarg]);
        if (!strcmp(args[iarg], "--timeit")) {
            timeit = true;
        } else if (!strncmp(args[iarg], "-i=", 3)) {
            index = args[iarg] + 3;
        } else {
            printf("processing file %s\n", args[iarg]);
            JSONDoc * json_data = from_file(args[iarg]);
            if (json_data && index) {
                JSON_print_value(json_data, NULL, index);
            }
            if (json_data != &empty_json) {
                JSONParser_dest(&json);
                JSONDoc_dest(json_data);
            }
            
        }
        iarg++;
    }
    jason_dest();
    return 0;
}


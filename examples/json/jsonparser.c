/* C standard library includes */
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* POSIX  includes */
#include <time.h>
#include <sys/types.h>

/* peggy includes */
#include <peggy/hash_map.h>
#include <peggy/parser.h> // should remove this
#include <peggy/astnode.h>
#include <peggy/token.h>

/* local includes */
#include "jsonparser.h"

// -9223372036854775807 + '\0'
#define MAX_LLONG_STR_LENGTH 21 

// SIGN, 53 fraction digits, DEC, e/E, SIGN, 4 digit exponent  + '\0' + a few bytes
#define MAX_DBL_STR_LENGTH 64

/* globals */
bool timeit = false;

struct JSONDoc empty_json = {0};

JSONParser json = {
    .Parser = {
        ._class = &Parser_class,
        .logger = DEFAULT_LOGGER_INIT,
    }
};

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
    Token * tok = parser->Parser._class->get_tokens(&parser->Parser, node);
    if (parser->json.data.string_used >= parser->json.data.string_size - tok->length - 1) {
        printf("%s - insufficient data to get next string\n", __func__);
        return (JSONString){0};
    }
    char * result = parser->json.data.strings + parser->json.data.string_used;
    memcpy(result, tok->string, tok->length); // copy value into allocated string
    //result[tok->length] = '\0'; // set null-teriminator. This should be handled by the calloc for the dest
    parser->json.data.string_used += tok->length + 1; // + 1 for null-terminator
    return (JSONString){.str = result, .len = tok->length};
}

ASTNode * build_string(Production * prod, Parser * parser_, ASTNode * node) {
    Token * tok = parser_->_class->get_tokens(parser_, node);
    ((JSONParser *)parser_)->json.data.string_size += tok->length + 1; // add one for a null-terminator
    // not sure why this doesn't work. This would speed things up a little bit. The call to get_tokens is not cheap
    //((JSONParser *)parser_)->json.data.string_size += node->str_length + 1; // add one for a null-terminator
    return build_action_default(prod, parser_, node);
}

ASTNode * build_value(Production * prod, Parser * parser_, ASTNode * node) {
    ((JSONParser *)parser_)->json.data.nvalues++; // record the number of values
    return build_action_default(prod, parser_, node);
}

JSONValue * handle_integer(JSONParser * parser, ASTNode * node) {
    static char integer_buffer[MAX_LLONG_STR_LENGTH] = {'\0'};
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    Token * tok = parser->Parser._class->get_tokens(&parser->Parser, node);
    if (tok->length >= MAX_LLONG_STR_LENGTH) {
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - integer value at line %u, col %u has too many digits to fit in long long. cannot convert\n", __func__, tok->coords.line, tok->coords.col);
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
    Token * tok = parser->Parser._class->get_tokens(&parser->Parser, node);
    if (tok->length >= MAX_DBL_STR_LENGTH) {
        /* this is technically correctable if the exponent is in range [0-2047] which means fraction bit is too long...trim it*/
        LOG_EVENT(&parser->Parser.logger, LOG_LEVEL_ERROR, "ERROR: %s - decimal value at line %u, col %u has too many digits to fit in double. cannot convert\n", __func__, tok->coords.line, tok->coords.col);
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
    switch (child->rule->id) {
        case JSON_INTEGER: {
            return handle_integer(parser, child);
        }
        case JSON_DECIMAL: {
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
    switch (node->children[0]->rule->id) {
        case NULL_KW: {
            jval->value.null = NULL_VALUE;
            jval->type = JSON_NULL;
            break;
        }
        case TRUE_KW: {
            jval->value.boolean = true;
            jval->type = JSON_BOOL;
            break;
        }
        case FALSE_KW: {
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
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    if (node->children[1]->nchildren) { // array has elements
        ASTNode * elements = node->children[1]->children[0];
        size_t nelements = (elements->nchildren + 1) >> 1;
        jval->value.array.length = nelements;
        jval->value.array.values = handle_value(parser, elements->children[0]);
        // assign all subsequent JSONValues, but we don't need to assign them to value.array as it is guaranteed to be contiguous (so long as this is single-threaded)
        for (size_t i = 2; i < elements->nchildren; i += 2) {
            handle_value(parser, elements->children[i]);
        }
    } else {
        jval->value.array = (JSONArray) {0};
    }
    jval->type = JSON_ARRAY;
    return jval;
}

void handle_object_key(JSONParser * parser, ASTNode * node, JSONValue * obj, JSONValue * jval) {
    obj->value.object._class->set(&obj->value.object, JSONParser_get_next_JSONString(parser, node), jval);
}

void handle_member(JSONParser * parser, ASTNode * node, JSONValue * obj) {
    JSONValue * jval = handle_value(parser, node->children[2]);
    handle_object_key(parser, node->children[0], obj, jval);
}

JSONValue * handle_object(JSONParser * parser, ASTNode * node) {
    JSONValue * jval = JSONParser_get_next_JSONValue(parser);
    if (node->children[1]->nchildren) { // object has key-value pairs
        ASTNode * members = node->children[1]->children[0];
        size_t nmembers = (members->nchildren + 1) >> 1;
        HASH_MAP_INIT(JSONString, pJSONValue)(&jval->value.object, nmembers);
        for (size_t i = 0; i < members->nchildren; i += 2) {
            handle_member(parser, members->children[i], jval);
        }
    } else {
        jval->value.object = (JSONObject) {0};
    }
    jval->type = JSON_OBJECT;
    return jval;
}

JSONValue * handle_value(JSONParser * parser, ASTNode * node) {
    ASTNode * child = node->children[0];
    switch (child->rule->id) {
        case OBJECT: {
            return handle_object(parser, child);
        }
        case ARRAY: {
            return handle_array(parser, child);
        }
        case KEYWORD: {
            return handle_keyword(parser, child);
        }
        case STRING: {
            return handle_string(parser, child);
        }
        case NUMBER: {
            return handle_number(parser, child);
        }
        default: {
            /* TODO: error condition*/
        }
    }
    return NULL;
}

void JSONDoc_init(JSONDoc * json) {
    json->data.nvalues_used = 0;
    json->data.string_used = 0;
    json->data.strings = calloc(json->data.string_size + 1024, sizeof(char));
    json->data.values = calloc(json->data.nvalues, sizeof(JSONValue));
    json->values = malloc(sizeof(JSONValue *) * json->nvalues);
}

void JSONDoc_dest(JSONDoc * json) {
    if (json->values) {
        free(json->values);
        json->nvalues = 0;
    }
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
            }
        }
        free(json->data.values);
        json->data.values = NULL;
        json->data.nvalues_used = 0;
        json->data.nvalues = 0;
    }
}

err_type JSONParser_init(JSONParser * parser, char const * string, size_t string_length, char * log_file, unsigned char log_level) {
    parser->json = (JSONDoc) {0};
    return parser->Parser._class->init(&parser->Parser, "", 0, 
        string, string_length, (Rule *)&json_token, (Rule *)&json_json, 
        JSON_NRULES, 0, 0, 0, log_file, log_level);
}

void JSONParser_dest(JSONParser * parser) {
    parser->Parser._class->dest(&parser->Parser);
}

JSONValue * JSONArray_get(JSONArray * array, size_t index) {
    if (index >= array->length) {
        /* TODO: error handling */
        return NULL;
    }
    return array->values + index;
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
    parser->json.nvalues = node->nchildren;
    JSONDoc_init(&parser->json);
    for (size_t i = 0; i < node->nchildren; i++) {
        parser->json.values[i] = handle_value(parser, node->children[i]);
    }
    return node;
}

JSONDoc from_string(char * string, size_t string_length, char * log_file, unsigned char log_level) {
    err_type status = PEGGY_SUCCESS;
    if (!timeit) {
        if ((status = JSONParser_init(&json, name, name_length, string, string_length, log_file, log_level))) {
            return empty_json;
        }
        return json.json;
    }
    static char const * const record_format = "%zu, %10.8lf, %s\n";
    char buffer[1024] = {'\0'};
    FILE * time_records = fopen("times.csv", "ab");
    
    struct timespec t0, t1;
    double time;
    clockid_t clk = CLOCK_MONOTONIC;
    //double clock_conversion = 1.0e-6;
    clock_gettime(clk, &t0);
    status = JSONParser_init(&json, string, string_length, log_file, log_level);
    clock_gettime(clk, &t1);

    if (t1.tv_nsec < t0.tv_nsec) {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    } else {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    }

    snprintf(buffer, 1024, record_format, json.json.data.nvalues, time, name);
    fprintf(time_records, "%s", buffer);
    printf("%s", buffer);
    
    fclose(time_records);
    return json.json;
}

JSONDoc from_file(char * filename, char * log_file, unsigned char log_level) {
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to open file %s\n", __func__, filename);
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return empty_json;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to read file: %s\n", __func__, filename);
        free(string);
        return empty_json;
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
    JSONDoc json_data = from_string(string, (size_t) file_size, log_file, log_level);

    free(string);
    return json_data;
}

int main(int narg, char ** args) {
    //printf("built!\n");
    char * input_file = NULL;
    char * log_file = NULL;
    unsigned char log_level = LOG_LEVEL_INFO;
    int iarg = 1;
    while (iarg < narg) {
        printf("arg %d: %s\n", iarg, args[iarg]);
        if (!timeit && !strcmp(args[iarg], "--timeit")) {
            timeit = true;
            iarg++;
            continue;
        }
        if (!input_file) {
            input_file = args[iarg++];
            continue;
        }
        if (!log_file) {
            log_file = args[iarg++];
            continue;
        }
        log_level = Logger_level_to_uchar(args[iarg], strlen(args[iarg]));
        iarg++;
    }
    if (input_file) {
        printf("processing file %s\n", input_file);
        JSONDoc json_data = from_file(input_file, log_file, log_level);

        /* insert any code you want to handle the csv here */

        JSONParser_dest(&json);
        JSONDoc_dest(&json_data);
    }
    json_dest();
    Logger_tear_down();
    return 0;
}
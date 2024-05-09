# JSON example

initialize a parser for the JSON
```
err_type JSONParser_init(
    JSONParser * parser,    // the parser to be initialized
    char const * string,    // the JSON string to be parsed
    size_t string_length,   // the length of the JSON string to be parsed
    char * log_file,        // a log output file. If null prints to stdout
    unsigned char log_level // the maximum log level to output: LOG_LEVEL_X where X = DISABLE, FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    )
```

tear down a parser
```
void JSONParser_dest(JSONParser * json) // cleans up JSONParser. Currently needs to be called for each time a parser is initialized
```

all data from parsed JSON are accessible as `JSONValue` structures
```
struct JSONValue {
    JSONValueType type; // an enum with values JSON_X where X = NULL, OBJECT, ARRAY, STRING, DECIMAL (double), INTEGER (long long)
    union {
        NULL_TYPE null; // user can set the NULL_TYPE and the unique value associated with it (NULL_VALUE). Default to 'void *' and 'NULL', respectively
        JSONObject object;
        JSONArray array;
        JSONString string;
        double decimal;
        llong integer;
        bool boolean;
    } value;
};
```

data are collected in a JSONDoc
```
struct JSONDoc {
    JSONValue ** values; // top-level list of JSONValue objects
    size_t nvalues; // number of top-level JSONValue objects
    /* other internal bookkeeping data */
};
```
JSONValue data are owned by the JSONDoc to use elsewhere after cleaning up the JSONDoc, must allocate your own memory.

To clean up JSONDoc
```
void JSONDoc_dest(JSONDoc *);
```

The two reading functions `from_file` and `from_string` return a JSONDoc from a JSON file or string, respectively
```
// input variables passed to JSONParser
JSONDoc from_string(char * string, size_t string_length, char * log_file, unsigned char log_level)

// file opened, read adn passed to from_string
JSONDoc from_file(char * filename, char * log_file, unsigned char log_level)
```

building the JSON
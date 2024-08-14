# Example use of <b>peggy</b> parser generater: a JSON parser

This is a very basic JSON parser (deserialization only, no serialization unfortunately)

## How to build
1) build <b>peggy</b> at the top level of the repo
2) in this directory, run one of the following
    - `make` - will build `jsv` binary in `/path/to/peggy/bin`

## How to use

- `/path/to/peggy/bin/csv -i=[/-separated indices] /path/to/file.csv`

For example, for a json (sample_files/basic.json):
```
[{
    "value": 0
    },
1,
3.14,
["string", null],
false]
```

The indice passed to `-i` for the following values are:
- 0: `0/value`
- 1: `1`
- 3.14: `2`
- "string": `3/0`
- null: `3/1`
- false: `4`

## Implementation details

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
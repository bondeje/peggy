# CSV example

This will use the csv.grmr file as a walkthrough to explain the components of a typical grammar file

## The CSV grammar

```
// https://datatracker.ietf.org/doc/html/rfc4180
```

The csv.grmr file in this repository is mostly taken from the ABNF in RFC4180, but in a more ENBF format and to keep it a little simpler, I have not included the "headers" portion. Syntactically, they are identical to any other "record" that appears later in the grammar--just the very first record--so it creates an ambiguity that must be handled by the parser to separate out the headers. It can be as simple as a flag in the initializer from the user saying to specially separate the first record as the header.

```
export = csv
```

The export configuration entry should almost always be the very first entry in the grammar file. It explicitly tells peggy that the module(header/source pair) generated should be title "csv" + extensions. If this were not present, peggy would just trim the extension, which would also be "csv" in this case. The export config option merely gives the user the ability to change filenames or switch between files without changing the output header/source pair.

```
import = csvparser
```

The import entry tells peggy that there are customization to the parser located in an external module. In this case, the csvparser header file.

```
punctuator:
    ','
```

The first production we see is the optional special production 'punctuator'. The punctuator will take a choice ('|') separated list of string literals and create individual `LiteralRule`s for each of them to be used as token elements during lexical analysis or by other syntax productions during parsing. CSV technically only has the ',' punctuation, but crlf ('\r\f') could also be one.

```
csv(handle_csv):
    crlf.record, (crlf | whitespace)*
```

The next production we see is the required entry point 'csv' production. Since the grammar file "exported" to "csv", it will look for a corresponding production with the name "csv" to act as the entry point. The parser can always choose a different entry point, but peggy will complain if you do not have this one. In the future, there might be a configuration option to control the name of the entry point.

The csv production also is decorated with an attribute "handle_csv". The first attribute (in a to-be-expanded list) represents a "build_action". It is essentially a function mapped to the production that will be called every time a successful match to the corresponding production is found. It's responsibility is to take an `ASTNode` corresponding to its dependent rules (in this case `crlf.record, (crlf | whitespace)*`) and generate a new node or modify the node in place for future use in the parse. This is the appropriate place to update any context or create custom nodes to propagate through the parse. Beware, however, that most nodes do not ultimately make it into the final AST tree as higher level productions may fail. The parser's `PackratCache` will still store the resulting `ASTNode` so that if a different production depends on the one you just built, it will not call the build action again and simply retrieve your previous result. In the special case of the entry point for the parse, the build_action could (and in all my examples does) initiate the tree traversal before finalizing the AST.

Build actions must have the following declaration in one of the imported modules:

```
ASTNode * [name of build action](Production *, Parser *, ASTNode *);
```

The `Production` passed to the build_action is the production that triggered the build action. The `Parser` is the parser itself, and the `ASTNode` is the node representing the result of the definition of the production. If no build_action is specified, there is a "build_action_default" that will handle the node.

The csv production itself is defined as a sequence (indicated by the higher priority ',' separator) of a list of 'record's delimited by 'crlf' denoted by the "crlf.record" subrule and finally any amount of 'crlf' or 'whitespace'; 'or' indicated by the choice operator '|'. In the case of csvs, the crlf sequence (later defined by the '\r\n' string literal) is explicitly not 'whitespace':

```
whitespace(skip_token):
    "[ \t\f\v]+"
```

Note the use of the "skip_token" build action. This is a special build action available for tokenizing productions that tells the tokenizer to consume the characters associated with it, but the parser should not use them.

The required special production 'token' is next. 

```
token:
    whitespace | crlf | punctuator | field
```

All productions that could match as distinct tokens should be present in a chained choice operation. The individual subrules should all constitute only a single token otherwise the tokenizer will fail. There currently is no check for this in peggy. In the case of csv, all tokens can be described as one of 'whitespace', 'crlf', 'punctuator', or a 'field'. As can be seen later in the grammar, a field is simply a 'string', here defined as any entry between commas that is enclosed in two unescaped double-quotes or a 'nonstring_field' that is everything else between two commas or crlf. 

```
string(process_string):
    "\"((\\\\\")|[^\"])*\""
```

```
nonstring_field(process_nonstring):
    "[^,\r\n]+"
```

Both string and nonstring_field have build actions that do some record keeping.

Some warnings about tokenizer productions:

1) do not allow any tokens to be of zero length. i.e. "[]*" should not be a successful match in any of the regular expressions. This likely will cause the tokenizer to enter an infinite loop.
2) The token production and its subrules should generally be the first productions defined. peggy traverses the productions like a tree and its dependencies like a tree, but has to stop at literals (literals do not have branches/leaves) If the literal is in a subrule but not previously defined, multiple declarations can be made or the production traversal can get stuck, especially if the punctuators or keywords have not appeared. One way around this is to do what was done with 'crlf' and wrap it in its special production. By doing this, production traversal will terminate at the name of the production, 'crlf', and just forward declare it.
3) again, do no include any productions that require more than one token to find a match. This will deadlock the tokenizer (how would you make a token by requiring two tokens?).
4) as with any other use of the choice operation, the order can matter. Especially with punctuation, you can have cases where one punctuator starts with a substring that is another punctuator, e.g. "+" and "+=", which mean very different things. In some cases, the tokenizer will just fail while in otherwise it will successfully complete an unambiguous parse that you did not mean. A simple way around it is to order the punctuators, keywords, and token subrules by what you think will be the longest first. This may make parses longer, but will ensure you tokenize and parse correctly.

The only other production that hasn't been mentioned yet is the record itself:

```
record(process_record):
    ','.field
```

which just consists of a comma-separated list of fields. The process_record build action does record keeping and ensures that the records meet the RFC requirements (all records have the same number of fields).

Other than the warnings regarding the tokenizer productions and the export configuration, the ordering of the entries in the grammar file does not matter. There is also a lot of flexibility in what you can or have to define as productions. Many times, you only really need to define a production when you want to add attributes, build actions, or do something specific during traversal when the production is encountered. By separating out a production, you can customize what happens in each stage, but if you do not care, you can always minimize the number of productions in the name of efficiency. A minimal csv grammar could also have been:

```
punctuator:
    '\r\n' | ','

nonstring_field:
    "[^,\r\n]+"

string:
    "\"((\\\\\")|[^\"])*\""

whitespace(skip_token):
    "[ \t\f\v]+"

token:
    whitespace | '\r\n' | ',' | string | nonstring_field

csv(handle_csv):
    '\r\n'.(','.(string | nonstring_field)), ('\r\n' | whitespace)*
```

which will probably execute faster with about 1/3 less memory, but now we lost the ability to check the records on the fly to ensure compatibily to our target spec, we are limited on what we can do with fields, and the entry point rule is less readable. In the future, peggy might allow all LiteralRules to exist on their own, which could reduce this further, but then everything will just look like regex.
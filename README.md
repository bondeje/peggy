<!-- need to warn users that build actions on productions used in the tokenizer should not access token->length. It will be invalid. instead use node->str_length or I should just make a parser API to get the string and deal with it in the parser -->

# <b>peg4c</b>

A packrat PEG parser generator that aids in rapid development of customizable parsers given a grammar with a format similar to EBNF. 

<b>peg4c</b> is designed to be as simple as possible to use--once the grammar specification is understood--while being as flexible as possible to allow the user compile-time and run-time customization and navigation.

Since it is packrat, parsers generated have run-time complexity O(n) where n is the number of tokens in the file to be parsed. The memory consumption is also O(n), but with an extremely large constant that is several times the number of all *rules* used in the grammar, not just productions. This could easily be 10^2 - 10^3. 

Tokenization/lexer is included as part of the grammar, i.e. tokens are specified as just another rule. No additional lexical analysis is required.

<details><summary> Dependencies </summary>


### Environments

Provided Makefiles require a POSIX-like build environment but binaries are cross-platform. I specifically am checking as targets: Windows (via Msys2), Linux (usually WSL2 Ubuntu and Arch), FreeBSD, OpenBSD, and NetBSD with amd64, and aarch64 on MacOS (M2) though the architecture dependency is only via the underlying regex library.

### Build dependencies

- compiler:
    - gcc is default but checked with clang. Only having issues with clang's ldd linking pcre2 on Msys.
- make
    - all makefiles are targeting the POSIX make specification so there is no dependence on GNU make extensions. The makefiles themselves try to account for some minor make differences on the target OSes. 

Goal is that for all target OSes, build is the usual `make` with optional `make install` (which simply copies shared libraries to `/usr/local/lib`) if there are linking issues. Without installation, all Makefiles build binaries to be run from the `/path/to/peg4c` directory.

### External dependencies

On Linux (specifically those with glibc): none. The underlying regex is provided by GNU's API for `regex.h`.
All other OSes: [pcre2](https://www.pcre.org/), specifically the build for 8-bit characters.

### Specific OS build notes

<b>Windows</b>: 
- the standard Makefiles cannot be made cross-platform to work with Windows and outside a POSIX emulation layer. Additional files "MsysMakefile.mk" are provided to run in an Msys environment to build the .exe and .dlls. Build commands are identical, but make will require specifying this file, e.g. `make -f MsysMakefile.mk`
- PCRE2 should be on the library search path

<b>NetBSD</b> 
- libpcre2-8.so.* must be put or linked into /usr/lib for ld or additionally -rpath-link must be specified at compile time to the proper directory.

<b>MacOS</b>
- I make no attempt to make `.dylib` files nor a proper installation, which MacOS treats very fragilely with .so files apparently. Linking might be an issue and you are restricted as to whence the executables are run. I have found I can only run from `/path/to/peg4c` and not `/path/to/peg4c/bin`. This makes running the examples very difficult only the Makefiles for the root build and tests currently work as expected

<b>OpenBSD</b>
- before attempting to run `make` from the top-level directory, add the to-be-created `/path/to/peg4c/bin` directory to `LD_LIBRARY_PATH` or add that path to the linker search path.

</details>

<details><summary> TODO List </summary>

- Left recursion is not yet implemented!
- Better decoupling of the grammar parser from the executable so that parsers can be built from grammars at runtime
- Right now `ASTNode`s are heavily wasted by the `PackratCache` objects, making the usefulness of parsing data files quite limited, e.g. my 16 GB RAM computer OOMs trying to parser a 32 MB `csv` file. It is possible to recover the "lost" `ASTNode`s that go unused during the parse stage reducing both allocation time and memory consumption.
- Roll my own regex using peg4c to perhaps replace the GNU regex & PCRE2
- Automatic AST traversal functions so that the user doesn't necessarily have to know the specific AST architecture during traversal

</details>

## Who should use <b>peg4c</b>

If you have a (complicated) grammar for a DSL, config file, log file, or many other types of structured text files for which you want to quickly generate a compiled parser. See especially though <b>When NOT to use</b>

## When to use

If your grammar has a lot of recursive structure. Ambiguous and even some non-context free structures welcome. <b>peg4c</b> has ways to extend the built-in parser and node structure to allow context to be captured in order to change the parser on the fly or ensure the unambiguous parser result is as intended.

### When NOT to use

PEG parsers, especially implemented with packrat parsers, are very memory hungry algorithms. Additionally, I have not done much to optimize for speed other than setting the `-O2` compilation flag in my makefiles. As such, parsing of very regularly* structured or large data files are NOT recommended. There are significantly better (read: simpler, faster, and memory efficient) parsers optimized for your data needs. I include examples for csv and JSON because their grammars are simple to understand and implement as exercises, but they are not necessarily targets of <b>peg4c</b>. JSON use as configuration is OK, but not data heavy. For these cases, I would just recommend a good regex library like pcre2.

*by regularly, I mean (1) iterative structures of very simple data types like tabular data, e.g. csv, or (2) the data has very well-defined and unambiguous structure in its type system, e.g. JSON when used for large dataset serialization.

## How to build
```
peg4c (in other parts of the documentation, this is /path/to/peg4c)
    |--examples             // each example has its own Makefiles for building
    |----c                  // C parser example
    |------sample_files
    |----calc               // REPL vector calculator for math.h
    |----csv                // csv
    |------sample_files
    |----ipv4               // ipv4 simple grammar
    |----json               // json deserializer
    |------sample_files
    |----peg4c              // peg4c reference grammar
    |--images               // for README.md
    |--include              // all header files
    |----peg4c              // include files for shared library
    |--lib                  // submodules I shared with other repos
    |--src                  // all source files
    |--tests                // test directory
    |--Makefile             // build and test on *nix
    |--MsysMakefile.mk      // build and test on Windows Msys

```

<b>*nix</b>
```
git clone --recurse-submodules https://github.com/bondeje/peg4c.git
cd peg4c
make
# to optionally build with address and undefined sanitizers
make SANITIZE="any non-empty value"
```

<b> Windows (via Msys)</b>
```
git clone --recurse-submodules https://github.com/bondeje/peg4c.git
cd peg4c
make -f MsysMakefile.mk
```

For all environments, make will also accept variables `BLD_LOG_LEVEL` and `DBG_LOG_LEVEL` which will set maximum logging levels for the libraries and binaries. Levels are:
```
LOG_LEVEL_DISABLE
LOG_LEVEL_FATAL
LOG_LEVEL_ERROR
LOG_LEVEL_WARN
LOG_LEVEL_INFO
LOG_LEVEL_DEBUG
LOG_LEVEL_TRACE
```
All `LOG_EVENTS` in the code that are lower on this list will be ommitted (in code, i.e. such logging code will not be generated). `LOG_LEVEL_DISABLE` removes all logging.

## Examples

I have included a couple toy parsers generated with <b>peg4c</b> in `examples/`. These are not intended to be stand alone project as they are not in the best of condition and of limited practical use--the possible exception being the C parser. They are meant to show different ways to use <b>peg4c</b>. If you have any interesting use cases or ideas, let me know.

### ipv4
This validates a string in dotted decimal notation as ipv4. This is only included because it is the simplest possible grammar I could think of to show how to build a parser with <b>peg4c</b>. It is of no other use.

### csv 
A simple csv parser. Generates a `CSVData` struct from an input file. Data is not converted to any data types but kept as strings. This example is more to demonstrate the linearity of the parser. The build command `make test_all` will parse the files in `examples/csv/sample_files` and time the results.

Note: watch out for line endings. I followed [RFC-4180](ietf.org/rfc/rfc4180.txt) which specifies carriage returns as well as newlines in separating records. Either resave with DOS/Windows line endings or modify the "crlf" and punctuator productions to look for '\n' only and not '\r\n'.

### json 
A simple json parser (deserialization only). Generates a `JSONData` struct from an input file. Data are converted to a tagged union `JSONValue` of the different datatypes that appear in JSON documents. This example shows how to create custom AST nodes.

### peg4c 
The defining reference grammar for <b>peg4c</b>. Running `make` under this directory will parse the `peg4c.grmr` file and rebuild <b>peg4c</b>. This is run from the top level as the "bootstrap test" to show that <b>peg4c</b> can build itself.

### calc
This is a basic calculator application that provides a REPL to the `math.h` library, but also supports vector-based calculations. Unfortunately, I have not implemented a history or memory yet so the typing is tedious.
![example calculator](images/calculator.jpg)

This example is not yet complete. It works, but I left in a small memory leak, which will get addressed when the history is implemented. 

### c
This is a C parser (of pre-processed files) in ~300 LOC; almost all of which is just loading the file and handling the well-known [typedef-identifier ambiguity](https://eli.thegreenplace.net/2007/11/24/the-context-sensitivity-of-cs-grammar/) (Note: I do not implement the lexer hack so that there is more de-coupling between lexer and parser). It should be almost fully C11 and mostly C23 compliant including properly parsing type inference. It is definitely missing the new character and constant additions of C23 and I have removed K&R function definition identifier lists. Additionally, it includes a bunch of the standard keywords and structures of compiler extensions so that you can parse the C standard library headers without modifications to the source files. Try it out and let me know if anything is wrong:
```
$ echo -e "#include <stdio.h>" | gcc -E -xc - | /path/to/peg4c/bin/c > stdio.ast
```

## How to use <b>peg4c</b> with your grammar

### Basic usage

Requirements:
- A grammar input file with the syntax detailed below and in examples. The grammar file should have some extension; I use .grmr but this extension does not have special meaning and any should be usable.
- For customization of the parsing and traversing process, at least one external header(source) file with necessary declarations and definitions. See the following <b>Grammar Structure</b> for how to set up and compile the grammar. 

Once you have a grammar `mydsl.grmr` exported as `mydsl` with `token` and `root` rules for the lexer and parser productions, respectively, parsing a string input is as follows:

```
#include <string.h>
// mydsl.h is autogenerated by the call `./peg4c mydsl.grmr`
#include "mydsl.h"
#include "peg4c/parser.h"
int main(int narg, char ** args) {
    if (narg > 1) {
        Parser parser;
        /*
        initialize the parser for the given grammar. This is a generic parser that will
        simply provide a token list an AST
        */
        Parser_init(&parser, (Rule *)&mydsl_token, (Rule *)&mydsl_root, MYDSL_NRULES, 0);
        // optional for logging. default is "stdout" with LOG_LEVEL_ERROR
        Parser_set_log_file(&parser, "stdout", LOG_LEVEL_WARN);  
        // LOG_LEVEL_* above are the same that can be specified at build time (see above)
        Parser_parse(&parser, args[1], strlen(args[1]));

        // check for failures
        err_type status = Parser_parse_status((Parser *)&parser);
        // to also print the (possibly partial) AST to a file, use instead:
        // err_type status = Parser_print_parse_status((Parser *)&parser, FILE_stream);
        if (status) { // indicates a tokenization or parse failure
            // handle error
            Parser_print_tokens((Parser *)&parser, stdout); // print tokens for debugging
        }

        /*
        code to traverse AST.
        To access tokens, which are stored in a linked list:
        Token * first_token = parser.token_head->next;
        To access ast from root:
        ASTNode * root = parser.ast;
        */
        Parser_dest(&parser); // clean up memory. 
        /*
        All AST memory is completely released so custom nodes that need to have longer
        lifetimes need a separate memory allocator
        */
    }
    return 0;
}
```

Traversing the AST depends on the structure of the rule or production and the subrules/nonterminals used to define it. In general, the subrules of a rule/production are child branches and each of the operators defines a new rule with any dependencies as child branches. The one exception to this is the `ChoiceRule` which simply yields the successful result. As an example, consider the production:

```
A:
    B, (C | D*)
```

which has a tree representation

```
a
 \
  b, (c or e)
            \
             d, d, d, d, ..., d
```

where lower case letters are resulting nodes corresponding to the rules. The result `e` represents the parent node of the repeated `d` nodes. Basically, the example grammar behaves as if it were defined as

```
A:
    B, (C | E)

E:
    D, D, D, D, ..., D // an arbitrary--possibly zero--number of Ds
```

The definition of the `struct ASTNode` in `astnode.h` shows
```
struct ASTNode {
    // struct Rule { struct RuleType _class; rule_id_type id; char const * name;}
    Rule * rule; 
    struct ASTNode ** children;
    size_t nchildren;
    Token * token_start;    // first token represented by node
    Token * token_end;      // last token (inclusive) represented by node
    // + other meta data
}
```
Assuming we have a node `a` for rule `A` and that each rule `X` has id `X_ID`, we can:
- Navigate to `b`: 
```
    b = a->children[0]
```
- Check if result is `C` or `D*`: 
```
    if (a->children[1]->rule->id == C_ID) { // result was C as opposed to D*
        // do something with C
    } else {
        // do something with D*
    }
```
- Assuming `D*` succeeded, iterate over the `d` nodes:
```
    for (size_t i = 0; i < a->children[1]->nchildren) {
        ASTNode * d = a->children[1]->children[i];
        // do something with this d
    }
```
- Iterate over tokens in node `c`:
```
    Token * tok = a->children[1]->token_start;
    Token * one_past = a->children[1]->token_end->next;
    while (tok != one_past) {
        // do something with token
        tok = tok->next; // tokens are a linked list
    }
```

The `Token` structure is very simple; just encapsulating a portion of a string (NOT NULL TERMINATED!) + some metadata for a linked list and positioning. To print a `Token * tok`:
```
    printf("token string: %.*s\n", (int)tok->length, tok->string);
```

### Grammar Structure

Grammar files are composed of whitespace (ignored), C/C++ style comments (ignored) and entries.

Every entry in the grammar either is or resembles a `<key>` : or = `<value>` pair definition.

1. config options - These are `<key>` = `<value>` pairs that configuration attributes of the generated parser. The supported key list is limited to 2:
    - import - specify external modules (headers that you wrote) to import into the generated parser. If you customize transforms functions for productions, point this to the headers that declare those functions. This can be specified multiple times per grammar
    - export - specify the parser output name and entry point for AST generation. This can only be specified once within a grammar file. If not specified, the default will be the name of the grammar file with the outermost extension removed. Whatever the export name, a production with a corresponding name must be defined in the grammar.
      - Ex. `import = csvparser export = csv` This will cause <b>peg4c</b> to generate `csv.h` and `csv.c`. `csv.c` will have `#include "csvparser.h"` for inclusion of external customization code. A production `csv: <some definition>` must exist in the grammar file as an entry point
2. productions - These are grammar productions that mostly follow common EBNF syntax. Only 1 production is required and its name must either be set by the grammar file name or the `export` config option. The syntax of the productions and grammar operators have two main differences with EBNF:
    - instead of whitespace separated terminals and nonterminals for the sequence operation, I use a comma `,`. I find that whitespace too often creates unnecessary ambiguities in parsing that require annoying, special handling.
    - productions are annotated with attributes by a comma-separated list enclosed in parentheses. e.g. `A(a): 'key', ':', B` indicates a production named `A` annotated with an attribute (in this case a build action `a`) that is defined as a sequence of the string literal `key` followed by string literal `:` followed by the production `B`.

    <b>WARNING</b> if the production is referenced at all on the tokenizer ([export name]_token production), the build action must not use Token.length. The value is invalid within the tokenizer. A more appropriate API will be built.
3. Special productions - These are identical to productions but with restrictions on attributes and specifically defined behavior and rules. None of these may have actions.
    - punctuator - optional - This is a production that must be defined as a choice rule of string literals. For each string literal a separate rule will be made for the tokenizer/parser to use. The enums corresponding to the punctuation will be generated based on a predefined table `PUNCTUATION_LOOKUP` in `peg4cparser.c` or with a generated named with format: `PUNC` + `_`-separated list of conversions to int of the punctuator characters. 
    - keyword - optional - This is similar to the punctuator production but string literals must be valid identifiers and the generated name will be of the format "[capitalized keyword]_KW". E.x. `'unsigned'` becomes `UNSIGNED_KW`.
    - token - required - This is a special production that defines how tokens are generated. No attributes can be applied and can only comprise ChoiceRule and LiteralRule subrule types.

#### A note on whitespace

A common production used in tokenization is `whitespace`, which is generally ignored. This is not the behavior of <b>peg4c</b>, which does not have any special productions for whitespace. Instead, there is a built-in action `skip_token` that can be used on any productions used in the `token` production to tell the parser to skip over that part of the string entirely. Example usage, which excludes all ASCII whitespace and C/C++ style comments (this is used by <b>peg4c</b>)

```
whitespace(skip_token):
    /* note that ASCII whitespace is just the first component "[ \t\r\n\v\f]+" */
    "([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+"
```

#### Order of grammar entries
    
- Configuration entries should come first. Order among them does not matter. Generally export should be first.
- Put tokenizer productions and the special productions `punctuator`, `keyword`, and `token` earliest in the grammar. These generate the declarations for all the parsing production. There might be issues generating a grammar if tokenizer productions are not placed early resulting in generated keywords or punctuators not being available to parsing productions

### Grammar operators implemented:

For any of the following operators that fail, a sentinel node pointer is returned to indicate fail (DO NOT USE `NULL`). Check for failure with `new_node == &ASTNode_fail`

<b>Left recursion is not yet implemented. You must manually remove left recursion from your grammar</b>

- `,` SequenceRule - set of subrules that must all succeed in order for the overall rule to succeed, otherwise failure. The resulting successful node has `N` children where `N` is the size of the set of subrules in the sequence.
- `|` ChoiceRule - set of subrules the first of which in order is returned upon success, otherwise failure. The resulting successful node has a single element in `children` corresponding to the successful subrule.
- `{min,max}` RepeateRule - The foregoing subrule must succeed a number of times in the range of `min` to `max` for the whole operation to succeed otherwise failure. `min`, `,`, and `max` are each optional with `min` and `max` defaults of 0. `max == 0` means an infinite number. The resulting node will have a list of `children` nodes of length `min` to `max`. If both the comma and max are ommitted, then `max==min`
- `+` - foregoing subrule must succeed at least once for the overall rule to succeed, otherwise failure. Equivalent to `{1,}`
- `*` - foregoing subrule may appear any number of times. Always succeeds. Equivalent to `{,}`
- `?` - foregoing rule may appear once if at all. Always succeeds. Equivalent to `{,1}`.
- `.` ListRule - the foregoing subrule denotes a delimiter of the subrule following the `.`. The following subrule must succeed at least once for overall success, otherwise failure. Note that `a.b` is semantically equivalent to `b, (a, b)*`, but navigation is quite different. The resulting rule for ListRule is a single list of children nodes - one for each of the delimiter and following subrule in order, but the semantically equivalent form must be navigated as a SequenceRule and then RepeateRule of SequenceRules. The advantage of the longer form is that the single element and repeat element can be captured in productions with separate actions and better error handling or context evaluation.
- `&` PositiveLookahead - the following subrule must succeed in order for the rule to succeed, otherwise failure. Upon success, the corresponding tokens are NOT consumed.
- `!` NegativeLookahead - the following subrule must fail in order for the rule to succeed, otherwise failure. Upon success, the corresponding tokens are NOT consumed.
- `"` LiteralRule** - regex type - The contents between two double-quotes is treated as a regular expressions following GNU regex syntax with syntax options `RE_SYNTAX_POSIX_EXTENDED | RE_BACKSLASH_ESCAPE_IN_LISTS | RE_DOT_NEWLINE` or PCRE2 syntax with syntax options `PCRE2_ANCHORED | PCRE2_DOTALL`. No special treatment of characters. 
- `'` LiteralRule** - string type - The contents between two single-quotes is treated as a plain text string. Most strings may also be in double-quotes, but care must be taken that it does not contain regex escape characters. For this type of LiteralRule, regex escape characters are properly escaped.
- `(` subrule `)` - encapsulation rule to alter binding order of subrules. Creates a single node based on the subrule.

**Currently, in order for these to be present in a subrule of a production, they must already appear as either a subrule of `punctuator` or `keyword` special productions or as a lone terminal for another production. For example for a production
```
A: ','."some string or regex"
```
There must exist somewhere:
```
punctuator:
    ... | ',' | ...
keyword:
    "some string"
// alternative to the keyword:
some_string_or_regex:
    "some string or regex"
```
The appropriate results will be substituted for the strings written in `A`.

### Operator precedence (Highest to Lowest top to bottom)
| operator punctuation | meaning/use                        |
|----------------------|------------------------------------|
| `"`, `'`, `()`       | `LiteralRule`s and parenthesizing  |
| `&`, `!`             | `Lookahead` rules                  |
| `.`                  | `ListRule`s                        |
| `+`, `*`, `?`, `{,}` | `RepeatRule`s                      |
| `,`                  | `SequenceRule`s                    |
| `\|`                 | `ChoiceRule`s                      |

## Testing <b>peg4c</b>

Basic tests of the logic of the different types of operators, lexer, and parser are automatically done at build time when `make` or `make all` are run. Additionally, a bootstrap build test of <b>peg4c</b> itself is done where peg4c rebuilds itself. These are very basic so coverage is wanting.

For the examples, I only have simple trivial examples of running them and no extensive testing. Where relevant, I provide some files under the directory `examples/*/sample_files`.

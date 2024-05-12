<!-- need to warn users that build actions on productions used in the tokenizer should not access token->length. It will be invalid. instead use node->str_length or I should just make a parser API to get the string and deal with it in the parser -->

# peggy

A PEG parser generator library that aids in rapid development of customizable (packrat) parsers given a grammar with a format similar to EBNF. 

Tokenization is included as part of the grammar and lazily evaluated, i.e. tokens are specified as just another production and are generated only as required for evaluation of the parsing productions.

<b>peggy</b> is designed to be as simple as possible to use--once the grammar specification is understood--while being as flexible as possible to allow the user compile-time and run-time customization and navigatation.

Since it is packrat, the base generated parsers have run-time complexity O(n) where n is the number of tokens in the file to be parsed. The memory consumption is also O(n), but with an extremely large constant that is several times the number of all *rules* used in the grammar, not just productions. This could easily be 10^2 - 10^3. In this context, a rule is any part of the grammar used in syntactic or lexical analysis. For example, in a production defined as `A: B | C | D, A`, there are at least 6 rules active. The production `A` itself, its derived sequence rule, and the sequence rule contains as its first element a choice rule comprising 3 other productions or rules `B`, `C`, and `D`. This would mean evaluation of `A` at a particular token will generate at least 6 nodes for the AST and the memory overhead associated with them.

## Dependencies/Limitations

Summary: [pcre2](https://www.pcre.org/) but possibly no other 3rd party libraries (mostly on Linux). GNU make (to use provided Makefiles). Some limitations on the regex style but with workarounds. Left recursion not (yet) supported.

<details> <summary>Environmental Dependency Details</summary>
On OSes with GNU standard implementations, there are no 3rd party library dependencies. For other OSes, pcre2 provides a regex backend. It is sometimes slower, sometimes faster than GNU regex but works. To be able to use <b>peggy</b> cross-platform, your grammar should try to define terminals using the considerable cross-over between POSIX Extended regex and Perl regex. I have found that I only really need to avoid POSIX character classes and write them out explicitly in ASCII. Many cases where either POSIX or Perl regex expressions cannot be expressed in the other, the PEG grammar file has a way around it by composing multiple regex into a single rule.

There are no facilities or testing yet for inputs beyond 8-bit character encodings. The regex and input strings must be composed of `char` characters. To parse anything beyond that, the regex characters must still be expressed in this type.

Build Environment: compilation and the makefiles that come in this repo should work in mostly posix environments with C99 but only with GNU Make. I have tested at various points on Windows (MSYS), FreeBSD (14.0), NetBSD (9.0), OpenBSD(7.5), Linux (Fedora 39, Debian 12.2, Ubuntu 22.04 via Windows WSL2), but almost always with gcc. clang should work but some compiler flags might have to be changed or added.

<b>Linux</b> - the GNU implementation of `regex.h` is used. The given makefile compiles with `-std=gnu99` and flag `_GNU_SOURCE`, which may change the behavior of some libraries. The `_GNU_SOURCE` is needed because the posix `regex.h` interface is uterly unsuitable for a recursive descent parser that does lazy token evaluation, which <b>peggy</b> generated parsers are. The posix API forces O(n^2) time complexity for this style by requiring null-terminated strings and internal `strlen` calls.

<b>Non-Linux OSes</b> - [pcre2](https://www.pcre.org/) compiled with 8-bit character strings is required. I am still working on my build system and do not include pcre2 so for now, the header must be placed in `lib/pcre2/include` and the shared library in `lib/pcre2/bin` in order to use the provided makefiles.

<b>Windows</b> - I highly recommend MSYS. Do not try to switch to the tre implementation of `regex.h` to avoid the pcre2 dependency. The tre library has something wrong with it where the API extension for the non-null-terminated extensions still results in an O(n^2) parse despite appearing like it doesn't depend on internal `strlen` calls. I haven't yet had time to find root cause.

<b>NetBSD</b> - libpcre2-8.so.* must be put into /usr/lib for runtime link
</details>

<details> <summary>Major Open Issues/TODO</summary>
Approximately in order of importance/dependence

- implement arena allocator for Parser.
- refactor node object allocation to minimize use of `malloc`
- refactor PackratCache back to a hashmap
- implement left recursion for packrat
- token->length are set equal to node->str-length for nodes generated as output of successful LiteralRules, however, in json example, I found that if I calculate the total string size allocation using node->str_length, I run out of buffer space in creating JSONStrings, but if I use token->length (in a 1-1 change), no problem. Need to resolve this as they should be the same and getting the raw token is a more expensive operation (traversal functions generally do not have access to the token, but they all get the node) than just getting node->str_length. node->str_length is used elsewhere in similar manners but asan is not complaining.
- Add APIs in parser struct to properly access token values. Token objects are likely to become opaque structures in the future.
</details>

<details> <summary>Wish List</summary>

- add serialization capabilities
- add automatic building of traversal functions (this will force traversal functions to have only specific input arguments and require all traversal context to be in the parser)
- (low priority) add ability to attribute destructor function to productions to handle custom ASTNode memory allocations. Right now, ASTNodes can be customized but all memory handling must be done manually by user.
- add serialization function attributes to productions
</details>

## Who should use <b>peggy</b>

If you have a (complicated) grammar for a DSL, config file, log file, or many other types of structured text files for which you want to quickly generate a compiled parser. See especially though <b>When NOT to use</b>

## When to use

If your grammar has a lot of recursive structure. Ambiguous and even some non-context free structures welcome. <b>peggy</b> has relatively straightforward ways to extend the built-in parser and node structure to allow context to be captured in order to change the parser on the fly or ensure the parse is unambiguous.

### When NOT to use

PEG parsers, especially implemented with packrat parsers, are very memory hungry algorithms. Additionally, I have not done much to optimize for speed other than setting the `-O2` compilation flag in my makefiles. As such, parsing of very regularly* structured or large data files are NOT recommended. There are significantly better (read: simpler, faster, and memory efficient) parsers optimized for your data needs. I include examples for csv and JSON because their grammars are simple to understand and implement as exercises, but they are not necessarily targets of <b>peggy</b>. JSON use as configuration is OK, but not data heavy. For these cases, I would just recommend a good regex library like pcre2.

*by regularly, I mean (1) iterative structures of very simple data types like tabular data, e.g. csv or (2) the data has very well-defined and unambiguous structure in its type system, e.g. JSON when used for large dataset serialization.

## How to build

<b>Linux</b>
```
git clone --recurse-submodules https://github.com/bondeje/peggy.git

make all NDEBUG=1
```

<b>Non-Linux (Unix-like environment)</b>
```
git clone --recurse-submodules https://github.com/bondeje/peggy.git
```

follow [here](https://www.pcre.org/current/doc/html/pcre2build.html#SEC1) to build pcre2. Put the `pcre2.h` in /path/to/peggy/lib/pcre2/include/ and the shared library in /path/to/peggy/lib/pcre2/bin/

```
make all NDEBUG=1
```

When including files to customize, use, or include in other projects the generated parsers, only include headers from include/peggy/. Do not use the other headers in include/ as they are for building <b>peggy</b> itself and will likely break whatever you are doing.

## Examples

<!--
A very simple example (and one of the worst application of PEGs) is an ipv4 parser for dot-decimal notation

```
// ipv4.grmr
export = ipv4                // redundant but allows change of file name
import = ipv4parser          // check_ipv4 is declared in ipv4parser.h

punctuator: '.'             // needed to be able to use '.' in ipv4
digit: "[0-9]"              
octet: digit{1:3}           // 0 digits and more than 3 are definitely errors
token: punctuator | digit
ipv4(check_ipv4): '.'.octet // check_ipv4 as a build action triggers AST traversal

// ipv4parser.h
#ifndef IPV4PARSER_H
#define IPV4PARSER_H

#include <peggy/astnode.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node);

#endif 

// ipv4parser.c
#include <stdio.h>
#include <peggy/astnode.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>
#include "ipv4.h"
#include "ipv4parser.h"

ASTNode ASTNode_syntax_error = {0}; // used to signal a failure that has already been addressed

int check_octet(Parser * parser, ASTNode * node, unsigned char loc) {
    // only success of the subrule will make it here. Do not have to check node for failure, node == NULL is a more serious failure so assume it is non-null
    Token * toks = parser->_class->get_tokens(parser, node);
    char const * str = (char const *)toks[0].string;
    unsigned char len = (unsigned char)(toks[node->ntokens - 1].length + (unsigned char)(toks[node->ntokens - 1].string - toks[0].string));
    // if the octet is < 3 decimals or numerically <= 255, it is valid
    if (len < 3 || str[0] < '2' || (str[0] == '2' && (str[1] < '5' || (str[1] == '5' && str[2] <= '5')))) {
        return 0;
    }
    printf("ipv4 failed. invalid octet at %hu (%.*s). must be in range 0-255\n", loc, len, str);
    return 1;
}
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node) {
    // if the parser has already failed or resulted in something other than than 3 '.' + 4 octets = 7, fail
    ASTNode * result = node;
    if (node->nchildren != 7) { 
        printf("%s is NOT valid ipv4. %zu octets not found, need 4.\n", parser->string, (node->nchildren + 1) / 2);
        result = &ASTNode_syntax_error;
    }
    for (size_t i = 0; i < node->nchildren; i += 2) {
        if (check_octet(parser, node->children[i], i / 2 + 1)) {
            result = &ASTNode_syntax_error;
        }
    }
    if (result == node) {
        printf("%s is valid ipv4\n", parser->string);
    }
    return result;
}

int main(int narg, char ** args) {
    if (narg > 1) {
        Parser parser = Parser_DEFAULT_INIT;
        while (--narg > 0) {
            // will print
            Parser_init(&parser, "ipv4", strlen("ipv4"), args[narg], strlen(args[narg]), (Rule *)&ipv4_token, 
                         (Rule *)&ipv4_ipv4, IPV4_NRULES, 0,0,0,NULL,0);
            // parser.ast should hold
            //      a valid node if passed,
            //      &ASTNode_syntax_error for any failure having been found
            //      &ASTNode_fail for any unhandled failures of the parser
            if (parser.ast == &ASTNode_fail) {
                printf("unhandled ipv4 check error\n");
            }
        }
        Parser_dest(&parser);   // clean up the parser
        ipv4_dest();            // clean up the ipv4 module
    } else {
        printf("no candidate ip address provided\n");
    }
    return 0;
}
```
-->
I have included a couple toy parsers generated with <b>peggy</b> in `examples/`. They are of limited practical use but can be used as templates for your needs. I will add more as time goes on. If you have any interesting ones, let me know.

csv - a simple csv parser. Generates a `CSVData` struct from an input file. Data is not converted to any data types but kept as strings. Customizing to convert empty data, integers, floating point, etc is a simple enough exercise

json - a simple json parser. Generates a `JSONData` struct from an input file. Data are converted to a tagged union of the different datatypes that JSON documents contain. This can be used as a reference for handling multiple data types.

peggy - a <b>peggy</b> parser parser. This example builds <b>peggy</b> from <b>peggy</b> and has the reference grammar upon which <b>peggy</b> is built.

## How to use <b>peggy</b> with your grammar

### Basic usage

Requirements:
- A grammar input file with the syntax detailed below and in examples. The grammar file should have some extension; I use .grmr but this extension does not have special meaning and any should be usable.
- For customization of the parsing and traversing process, at least one external header(source) file with necessary declarations and definitions.

For details of generating a grammar, follow the walkthrough in the `csv` example, which provides a simple enough example to describe the grammar format and facilities. It does not have full coverage so I have tried to detail the most relevant uses below in Grammar Structure.

For customization, see Parser and Node Structure and Transform Functions

To initialize a parser (in current working directory):
```
/path/to/peggy /path/to/my_grammar.grmr
```

To build an AST:
```
#include <peggy/parser.h>
Parser parser;
Parser_init(&parser, quite a few arguments, this interface is likely not stable);

/**
 * navigate AST. if you don't use a build action to trigger traversal in 
 * Parser_init or do lazy parsing, you can trigger a traversal using parser.ast
 * (which holds a reference to root node if successful or &ASTNode_fail if 
 * parsing failed) or initiate parsing with parser._class->parse, respectively 
 */

Parser_dest(&parser) // releases memory
```

To navigate an AST, the node structure and use are as follows:
```
/* // for reference, likely not stable
struct ASTNode {
    struct ASTNodeType * _class; // vtable for functions. 
    ASTNode ** children;    // the subrules associated with the rule that generated the node
    Rule * rule;            // the rule that generated the node. 
                            // To access its identifying enum, rule->id
    size_t str_length;      // the length of the str encompased by the node and its children. 
                            // Do NOT take this as the sum length of all tokens, it is not.
    size_t nchildren;       // the number of nodes in children
    size_t token_key;       // an internal register
    size_t ntokens;         // number of tokens encompased by the node and its children
};

example grammar production (a, b, c, and d are all (non)terminals):
    a: b | (c, d?)
    // 'a' has either a single child rule 'b' or 2 ('c' with optional 'd')
*/

#include <peggy/astnode.h>

handle_a(ASTNode * node_a) {
    // select traversal behavior based on which choide element comprises a
    switch (node_a->children[0]->rule->id) { 
        case B: // all standard productions can be switched with the name of the production in all caps
        {
            // handle the 'b' (non)terminal
            handle_b(node->children[0]);
            break;
        }
        case C: {
            // handle the 'c' (non)terminal
            handle_c(node->children[0]);
            if (node->children[1]->nchildren) { // if d is found in d?
                // handle the 'd' (non)terminal
                handle_d(node->children[1]->children[0]);
            }
            break;
        }
        default: {
            // handle error. branches do not match structure of 'a'
        }
    }
}
```

To get access to the content of a node (and its children)
```
/* // for reference
typedef struct TokenCoords {
    unsigned int line;  // line number of token
    unsigned int col;   // column number of token
} TokenCoords;
// for reference, likely not stable
struct Token {
    struct TokenCoords coords;  // line, col coordinates where token found
    struct TokenType * _class;  // vtable of functions
    char const * string;        // non-null-terminated string. char const to support string literals for parsing
    size_t length;              // lenth of the string
};
*/

#include <peggy/token.h>

// return an array of tokens of length node->ntokens. DO NOT FREE!
Token * toks = parser._class->get_tokens(&parser, node, NULL);

// example print of the tokens:
for (size_t i = 0; i < node->ntokens; i++) {
    printf("%.*s ", toks[i].length, toks[i].string);
}
```

To clean up parser memory (releases all memory associated with the parse)
```
parser._class->dest(&parser);
```

To clean up at module level. Optional as these are globals cleaned up at program exit. It is OK to call this without exiting the program, but next parser use will trigger reinitialization of a lot of structs and memory allocations.
```
[module export name]_dest();
```

### Grammar Structure

Grammar files are composed of whitespace (ignored), C/C++ style comments (ignored) and entries.

Every entry in the grammar either is or resembles a `<key>` : or = `<value>` pair definition.

1. config options - These are `<key>` = `<value>` pairs that configuration attributes of the generated parser. The supported key list is limited to 2:
    - import - specify external modules (headers that you wrote) to import into the generated parser. If you customize transforms functions for productions, point this to the headers that declare those functions. This can be specified multiple times per grammar
    - export - specify the parser output name and entry point for AST generation. This can only be specified once within a grammar file. If not specified, the default will be the name of the grammar file with the outermost extension removed. Whatever the export name, a production with a corresponding name must be defined in the grammar.
      - Ex. `import = csvparser export = csv` This will cause <b>peggy</b> to generate `csv.h` and `csv.c`. `csv.c` will have `#include "csvparser.h"` for inclusion of external customization code. A production `csv: <some definition>` must exist in the grammar file as an entry point
2. productions - These are grammar productions that mostly follow common EBNF syntax. Only 1 production is required and its name must either be set by the grammar file name or the `export` config option. The syntax of the productions and grammar operators have two main differences with EBNF:
    - instead of whitespace separated terminals and nonterminals for the sequence operation, I use a comma `,`. I find that whitespace too often creates unnecessary ambiguities in parsing that require annoying, special handling.
    - productions are annotated with attributes by a comma-separated list enclosed in parentheses. e.g. `A(a): 'key', ':', B` indicates a production named `A` annotated with an attribute (in this case a build action `a`) that is defined as a sequence of the string literal `key` followed by string literal `:` followed by the production `B`.

    <b>WARNING</b> if the production is referenced at all on the tokenizer ([export name]_token production), the build action must not use Token.length. The value is invalid within the tokenizer. A more appropriate API will be built.
3. Special productions - These are identical to productions but with restrictions on attributes and specifically defined behavior and rules. None of these may have actions.
    - punctuator - optional - This is a production that must be defined as a choice rule of string literals. For each string literal a separate rule will be made for the tokenizer/parser to use. The enums corresponding to the punctuation will be generated based on a predefined table `PUNCTUATION_LOOKUP` in `peggyparser.c` or with a generated named with format: `PUNC` + `_`-separated list of conversions to int of the punctuator characters. 
    - keyword - optional - This is similar to the punctuator production but string literals must be valid identifiers and the generated name will be of the format "[capitalized keyword]_KW". E.x. `'unsigned'` becomes `UNSIGNED_KW`.
    - token - required - This is a special production that defines how tokens are generated. No attributes can be applied and can only comprise ChoiceRule and LiteralRule subrule types.

#### A note on whitespace

A common production used in tokenization is `whitespace`, which is generally ignored. This is not the behavior of <b>peggy</b>, which does not have any special productions for whitespace. Instead, there is a built-in action `skip_token` that can be used on any productions used in the `token` production to tell the parser to skip over that part of the string entirely. Example usage, which excludes all ASCII whitespace and C/C++ style comments (this is used by <b>peggy</b>)

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
- `{min:max}` RepeateRule - The foregoing subrule must succeed a number of times in the range of `min` to `max` for the whole operation to succeed otherwise failure. `min` and `max` are each optional with defaults of 0. `max == 0` means an infinite number. The resulting node will have a list of `children` nodes of length `min` to `max`.
- `+` - foregoing subrule must succeed at least once for the overall rule to succeed, otherwise failure. Equivalent to `{1:}`
- `*` - foregoing subrule may appear any number of times. Always succeeds. Equivalent to `{:}`
- `?` - foregoing rule may appear once if at all. Always succeeds. Equivalent to `{:1}`.
- `.` ListRule - the foregoing subrule denotes a delimiter of the subrule following the `.`. The following subrule must succeed at least once for overall success, otherwise failure. Note that `a.b` is semantically equivalent to `b, (a, b)*`, but navigation is quite different. The resulting rule for ListRule is a single list of children nodes - one for each of the delimiter and following subrule in order, but the semantically equivalent form must be navigated as a SequenceRule and then RepeateRule of SequenceRules. The advantage of the longer form is that the single element and repeat element can be captured in productions with separate actions and better error handling or context evaluation.
- `&` PositiveLookahead - the following subrule must succeed in order for the rule to succeed, otherwise failure. Upon success, the corresponding tokens are NOT consumed.
- `!` NegativeLookahead - the following subrule must fail in order for the rule to succeed, otherwise failure. Upon success, the corresponding tokens are NOT consumed.
- `"` LiteralRule** - regex type - The contents between two double-quotes is treated as a regular expressions following GNU regex syntax with syntax options `RE_SYNTAX_POSIX_EXTENDED | RE_BACKSLASH_ESCAPE_IN_LISTS | RE_DOT_NEWLINE` or PCRE2 syntax with syntax options `PCRE2_ANCHORED | PCRE2_DOTALL`. No special treatment of characters. 
- `'` LiteralRule** - string type - The contents between two single-quotes is treated as a plain text string. Most strings may also be in double-quotes, but care must be taken that it does not contain regex escape characters. For this type of LiteralRule, regex escape characters are properly escaped.
- `(` subrule `)` - encapsulation rule to alter binding order of subrules. Creates a single node based on the subrule.

**Currently, in order for these to be present in a subrule of a production, they must already appear as either a subrule of `punctuator` or `keyword` special productions or as a lone terminal for another production

### Operator precedence (Highest to Lowest top to bottom)
| operator punctuation | meaning/use                        |
|----------------------|------------------------------------|
| `"`, `'`, `()`       | `LiteralRule`s and parenthesizing  |
| `&`, `!`             | `Lookahead` rules                  |
| `.`                  | `ListRule`s                        |
| `+`, `*`, `?`, `{:}` | `RepeatRule`s                      |
| `,`                  | `SequenceRule`s                    |
| `\|`                 | `ChoiceRule`s                      |

## Testing <b>peggy</b>

I admit there is a lot lacking here; mostly because I ported it from a shell of a python script for which the testing is not really compatible. The simplest tests are by running the the `csv` and `peggy` examples, but they are clearly missing coverage of the `PositiveLookahead` and `NegativeLookahead` rule builds and checks.


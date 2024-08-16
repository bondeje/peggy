#include <stddef.h>
#include <limits.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * First bit: 0 means successful match, 1 means no match
 * Second bit: 0 means continue, 1 means stop
*/
#define PAT_NOMATCH 0
#define PAT_MATCH 1
#define PAT_FAIL 2
#define PAT_MATCH_STOP 3
#define PAT_ERROR 4
#define MATCH_BIT 0
#define STOP_BIT 1

#define GET_BIT(value, bit_num) ((value >> bit_num) & 1)
#define SET_BIT(value, bit_num) (value |= (1 << bit_num))

typedef struct String String;
typedef struct Pattern Pattern;
typedef struct PatternType PatternType;
typedef struct StringPattern StringPattern;
typedef struct CharClass CharClass;
typedef struct ChoicePattern ChoicePattern;
typedef struct SequencePattern SequencePattern;
typedef struct RepeatPattern RepeatPattern;

// try to keep this opaque
// represents a standard string
struct String {
    char * str;
    size_t len;
};

// try to keep this opaque
// this is a base pattern
struct Pattern {
    PatternType * _vtable;
    size_t match_length; // current match size
    size_t lag; // number of characters unused by the match
    unsigned char state;
};

struct PatternType {
    unsigned char (*next)(Pattern * pat, char const * c);
    void (*reset)(Pattern * pat);
    Pattern * (*get_match)(Pattern * pat);
    size_t (*get_match_length)(Pattern * pat);
};

Pattern * Pattern_get_match(Pattern * pat);
size_t Pattern_get_match_length(Pattern * pat);

void Pattern_reset(Pattern * pat) {
    pat->state = PAT_NOMATCH;
    pat->match_length = 0;
    pat->lag = 0;
}

// really this is a string based pattern...the basic sequential element in the regex
struct StringPattern {
    Pattern pattern;
    String str;
    size_t loc;
};

unsigned char StringPattern_next(Pattern * strpat, char const * c);
void StringPattern_reset(Pattern * strpat);

struct CharClass {
    Pattern pattern;
    HASH_SET(char) set;
    bool exclude; // if set is inclusive or not
};

unsigned char CharClass_next(Pattern * char_class, char const * c);
void CharClass_reset(Pattern * char_class);

// this is a manager for multiple regexes in parallel
struct ChoicePattern {
    Pattern pattern; // parallel pattern is of course a pattern
    Pattern ** pats; // the subregexes in parallel
    size_t npats;
    size_t longest_pattern; // index of longest pattern
    bool greedy; // sets match shortest or longest
};

unsigned char ChoicePattern_next(Pattern * choice_pat, char const * pc);
void ChoicePattern_reset(Pattern * par_pat);

struct SequencePattern {
    Pattern pattern; // parallel pattern is of course a pattern
    Pattern ** pats; // the subregexes in parallel
    size_t curpat; // current pattern under evaluation
    size_t lapat; // lookahead pattern
    size_t npats;
};

unsigned char SequencePattern_next(Pattern * seq_pat, char const * pc);
void SequencePattern_reset(Pattern * seq_pat);

struct RepeatPattern {
    Pattern pattern; // parallel pattern is of course a pattern
    Pattern * pat; // the subregex that is to be repeated
    size_t nsuccess; // 
    size_t min_rep;
    size_t max_rep;
    bool greedy; // not yet implemented
};

unsigned char RepeatPattern_next(Pattern * rep_pat, char const * pc);
void RepeatPattern_reset(Pattern * rep_pat);
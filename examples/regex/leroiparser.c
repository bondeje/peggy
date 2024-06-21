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

typedef struct Pattern Pattern;
typedef struct Match Match;
typedef struct PatternType PatternType;
typedef struct String String;
typedef struct Char Char;
typedef struct StringPattern StringPattern;
typedef struct ChoicePattern ChoicePattern;
typedef struct RepeatPattern RepeatPattern;
typedef ChoicePattern Regex;
typedef struct SequencePattern SequencePattern;
typedef struct CharClass CharClass;


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

Pattern * Pattern_get_match(Pattern * pat) {
    if (GET_BIT(pat->state, MATCH_BIT)) {
        return pat;
    }
    return NULL;
}

size_t Pattern_get_match_length(Pattern * pat) {
    if (GET_BIT(pat->state, MATCH_BIT)) {
        return pat->match_length;
    }
    return 0;
}

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

#define CHAR_TYPE_LEN 1

unsigned char StringPattern_next(Pattern * strpat, char const * c) {
    StringPattern * self = (StringPattern *)strpat;
    size_t start = self->loc;
    char * str = self->str.str + start;
    if (self->str.len - start < CHAR_TYPE_LEN) {
        return (strpat->state = PAT_FAIL);
    }
    if (str[0] != *c) {
        return (strpat->state = PAT_FAIL);
    }
    self->loc += CHAR_TYPE_LEN;
    if (self->loc == self->str.len) {
        strpat->match_length = self->str.len;
        return (strpat->state = PAT_MATCH_STOP);
    }
    return (strpat->state = PAT_NOMATCH);
}

void StringPattern_reset(Pattern * strpat) {
    ((StringPattern *)strpat)->loc = 0;
    Pattern_reset(strpat);
}

// this is a manager for multiple regexes in parallel
struct ChoicePattern {
    Pattern pattern; // parallel pattern is of course a pattern
    Pattern ** pats; // the subregexes in parallel
    size_t npats;
    size_t longest_pattern; // index of longest pattern
    bool greedy; // sets match shortest or longest
};

// returns true if a match is found else false
unsigned char ChoicePattern_next(Pattern * choice_pat, char const * pc) {
    ChoicePattern * self = (ChoicePattern *)choice_pat;
    size_t active_count = self->npats;

    for (size_t i = 0; i < self->npats; i++) {
        Pattern * test_pat = self->pats[i];
        unsigned char last_state = test_pat->state;
        if (GET_BIT(last_state, STOP_BIT)) { // do not check this subpattern
            active_count--;
            if (GET_BIT(last_state, MATCH_BIT)) {
                test_pat->lag++; // increment the lag of the match without checking
            }
        } else {
            unsigned char next_state = test_pat->_vtable->next(test_pat, pc);
            if (GET_BIT(next_state, MATCH_BIT)) { // match found
                SET_BIT(choice_pat->state, MATCH_BIT);
                if (!self->greedy) {
                    *choice_pat = *self->pats[i];
                    return (choice_pat->state = PAT_MATCH_STOP);
                }
                if (test_pat->match_length > choice_pat->match_length) {
                    self->longest_pattern = i;
                }
                if (GET_BIT(next_state, STOP_BIT)) {
                    active_count--;
                }
                
            } else { // match not found
                if (next_state == PAT_FAIL) { // pattern failed
                    active_count--;
                }
            }
        }
    }
    if (!active_count) {
        *choice_pat = *self->pats[self->longest_pattern];
        SET_BIT(choice_pat->state, STOP_BIT);
    }
    return choice_pat->state;
}

void ChoicePattern_reset(Pattern * par_pat) {
    ChoicePattern * self = (ChoicePattern *)par_pat;
    //ChoicePattern_reset_bitmask(self);
    for (size_t i = 0; i < self->npats; i++) {
        self->pats[i]->_vtable->reset(self->pats[i]);
    }
    self->pattern._vtable->reset(&self->pattern);
}

// need to handle char_class objects, sequence, repeated rule
// char_class will required a hash_set of Char

int char_comp(char a, char b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}
#define ELEMENT_TYPE char
#define ELEMENT_COMP char_comp
#define HASH_FUNC uint_hash
#include <peggy/hash_set.h>

struct CharClass {
    Pattern pattern;
    HASH_SET(char) set;
    bool exclude; // if set is inclusive or not
};

unsigned char CharClass_next(Pattern * char_class, char const * c) {
    CharClass * self = (CharClass *) char_class;
    if (self->exclude ^ self->set._class->in(&self->set, c)) {
        char_class->match_length = CHAR_TYPE_LEN;
        return (char_class->state = PAT_MATCH);
    } else {
        return (char_class->state = PAT_FAIL);
    }
}

void CharClass_reset(Pattern * char_class) {
    Pattern_reset(char_class);
}

struct SequencePattern {
    Pattern pattern; // parallel pattern is of course a pattern
    Pattern ** pats; // the subregexes in parallel
    size_t curpat; // current pattern under evaluation
    size_t lapat; // lookahead pattern
    size_t npats;
};

unsigned char SequencePattern_next(Pattern * seq_pat, char const * pc) {
    SequencePattern * self = (SequencePattern *) seq_pat;
    Pattern * cur = self->pats[self->curpat];
    if (cur->state == PAT_MATCH) {
        size_t last_length = cur->match_length;
        unsigned char new_state = cur->_vtable->next(cur, pc);
        if (new_state == PAT_MATCH) {
            size_t new_length = cur->match_length;
            if (new_length > last_length) { // state did not change, but evaluation consumed characters
                self->lapat = self->curpat + 1;
                if (self->lapat < self->npats) {
                    self->pats[self->lapat]->_vtable->reset(self->pats[self->lapat]);
                }
            }
            if (self->lapat < self->npats) {
                Pattern * la = self->pats[self->lapat];
                unsigned char la_new_state = la->_vtable->next(la, pc);
                /* TODO: review and maybe break out a function to check the lookahead, call it recursively */
                if (la_new_state == PAT_MATCH_STOP) {
                    self->lapat++;
                    size_t la_lag = la->lag;
                    if (self->lapat < self->npats) {
                        la = self->pats[self->lapat];
                        la->_vtable->reset(la);
                        if (la_lag) {
                            la->_vtable->next(la, pc);
                        }
                    }
                }
            }
        } else if (new_state == PAT_MATCH_STOP) {
            seq_pat->match_length += cur->match_length;
            if (!cur->lag) {
                self->curpat++;
                if (self->curpat < self->npats) {
                    cur = self->pats[self->curpat];
                    cur->_vtable->reset(cur);
                }
            } else {
                while (++self->curpat < self->lapat) {
                    seq_pat->match_length += self->pats[self->curpat]->match_length;
                }
                // curpat now points to the last active lookahead lapat
                self->lapat++;
                if (self->lapat < self->npats) {
                    self->pats[self->lapat]->_vtable->reset(self->pats[self->lapat]);
                }
            }
        }
    } else if (cur->state == PAT_NOMATCH) {
        switch (cur->_vtable->next(cur, pc)) {
            case PAT_NOMATCH: { // do nothing
                break;
            }
            case PAT_FAIL: {
                seq_pat->state = PAT_FAIL;
                break;
            }
            case PAT_MATCH_STOP: {
                seq_pat->match_length += cur->match_length;
                self->curpat++;
                if (self->curpat < self->npats) {
                    self->pats[self->lapat]->_vtable->reset(self->pats[self->lapat]);
                }                
            } __attribute__((fallthrough));
            case PAT_MATCH: {
                self->lapat = self->curpat + 1;
                if (self->lapat < self->npats) {
                    self->pats[self->lapat]->_vtable->reset(self->pats[self->lapat]);
                }
                break;
            }
            
        }
    }
    if (self->curpat == self->npats) {
        seq_pat->state = PAT_MATCH_STOP;
    }
    return seq_pat->state;
}

void SequencePattern_reset(Pattern * seq_pat) {
    SequencePattern * self = (SequencePattern *)seq_pat;
    self->curpat = 0;
    self->lapat = 1;
    Pattern_reset(seq_pat);
}

struct RepeatPattern {
    Pattern pattern; // parallel pattern is of course a pattern
    Pattern * pat; // the subregex that is to be repeated
    size_t nsuccess; // 
    size_t min_rep;
    size_t max_rep;
    bool greedy; // not yet implemented
};

void RepeatPattern_update_state(RepeatPattern * self) {
    Pattern * rep_pat = (Pattern *)self;
    if (self->nsuccess < self->min_rep) {
        rep_pat->state = PAT_NOMATCH;
    } else {
        SET_BIT(rep_pat->state, MATCH_BIT);
    }
    if ((self->max_rep && self->nsuccess == self->max_rep) || self->pat->state == PAT_FAIL) {
        SET_BIT(rep_pat->state, STOP_BIT);
    }
}

unsigned char RepeatPattern_next(Pattern * rep_pat, char const * pc) {
    RepeatPattern * self = (RepeatPattern *)rep_pat;
    Pattern * base = self->pat;
    size_t last_length = base->match_length;
    switch (base->_vtable->next(base, pc)) {
        case PAT_NOMATCH: {
            // no action
        } break;
        case PAT_FAIL: {
            RepeatPattern_update_state(self);
        } break;
        case PAT_MATCH: {
            rep_pat->match_length += (base->match_length - last_length);
        } break;
        case PAT_MATCH_STOP: {
            rep_pat->match_length += (base->match_length - last_length);
            self->nsuccess++;
            RepeatPattern_update_state(self);
            if (!GET_BIT(rep_pat->state, STOP_BIT)) {
                size_t lag = base->lag;
                base->_vtable->reset(base);
                if (lag) {
                    base->_vtable->next(base, pc);
                }
            }
        } break;
    }
}

void RepeatPattern_reset(Pattern * rep_pat) {
    RepeatPattern * self = (RepeatPattern *)rep_pat;
    self->nsuccess = 0;
    self->pat->_vtable->reset(self->pat);
    Pattern_reset(rep_pat);
}
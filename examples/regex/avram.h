#ifndef AVRAM_H
#define AVRAM_H

#include "reutils.h"
#include "dfa.h"

struct avramT3 {
    DFA dfa;
    int cur_state;       // internal use state tracking
    char * buffer;          // buffer of characters passed to DFA
    int ibuffer;            // location of cursor in buffer
    int buffer_size;        // size of buffer
    int end;                // end of match in buffer(one past). -1 if no matches found
    unsigned int flags;     // flags for operation of the DFA. flags should account for the properties of the buffer (user provided)
};

struct MatchString {
    char const * str;
    int len;
};

char * are_compile_pattern_buffered(const char * regex, const int regex_size,
    struct avramT3 * pattern_buffer, unsigned int flags, char * buffer, 
    const int buffer_size);

char * are_compile_pattern(const char * regex, const int regex_size,
    struct avramT3 * av,
    unsigned int flags);

int are_match(struct avramT3 * av, const char * string, const int size,
    const int start);

// for streaming mode
int are_update(struct avramT3 * av, const char * string, const int size);

void are_get_match(struct avramT3 * av, struct MatchString * match, 
    struct MatchString * unmatched);

void are_free(struct avramT3 * av);

#endif

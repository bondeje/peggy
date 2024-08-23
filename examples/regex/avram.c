#include <stdlib.h>

#include "avram.h"

#include "reparser.h"
#include "dfa.h"

char * are_compile_pattern_buffered(const char * regex, const int regex_size,
    struct avramT3 * av, unsigned int flags, char * buffer, 
    const int buffer_size) {

    *av = (struct avramT3) {
        .dfa = {
            .regex_s = regex, 
            .regex_len = regex_size
        },
        .flags = flags, 
        .buffer = buffer, 
        .buffer_size = buffer_size,
        .end = -1
    };
    
    RegexBuilder reb;
    RegexBuilder_build(&reb, regex, regex_size, (DFA *)&av);
    RegexBuilder_dest(&reb);
    return NULL;
}

char * are_compile_pattern(const char * regex, const int regex_size,
    struct avramT3 * av, unsigned int flags) {
    
    flags |= REGEX_ALLOC_BUFFER;
    char * buffer = malloc(sizeof(char) * REGEX_ALLOC_BUFFER_SIZE);
    if (!buffer) {
        return "buffer malloc failure in re_compile_pattern";
    }
    return are_compile_pattern_buffered(regex, regex_size, av, flags, buffer, REGEX_ALLOC_BUFFER_SIZE);
}

int are_match(struct avramT3 * av, const char * string, const int size,
    const int start) {
    
    int cur = start;
    int status = REGEX_WAIT;
    while (cur < size && (status = are_update(av, string + cur++, 1)) >= REGEX_WAIT) {
    }
    if (status >= REGEX_WAIT) {
        return -REGEX_FAIL;
    }
    if (status > 0) {
        return -status;
    }
    return cur + status - start;
}

static inline int av_push_buffer(struct avramT3 * av, const char * string, const int size) {
    if (av->buffer_size - size < av->ibuffer) {
        return 1;
    }
    memcpy(av->buffer + av->ibuffer, string, size * sizeof(char));
    av->ibuffer += size;
    return 0;
}

/**
 * returns: 
 *      REGEX_MATCH if matches on last input
 *      REGEX_FAIL if no match at all
 *      REGEX_ERROR if internal error occurs
 *      REGEX_WAIT if no match but not fail
 *      integer x < 0 if match occurred -x characters ago
 */
int are_update(struct avramT3 * av, const char * string, const int size) {
    if (DFA_check(DFA_get_state((DFA *)av, av->cur_state), string, size, &av->cur_state)) {
        if (av->end >= 0) {
            return (av->end - av->ibuffer);
        }
        return REGEX_FAIL;
    }
    if (av->cur_state >= ((DFA *)av)->nstates) {
        return REGEX_ERROR;
    }
    if (av_push_buffer(av, string, size)) {
        return REGEX_ERROR;
    }
    if (DFA_is_accepting((DFA *)av, av->cur_state)) {
        av->end = av->ibuffer;
        if (!(av->flags & REGEX_GREEDY)) {
            return REGEX_MATCH;
        }
    }
    return REGEX_WAIT;
}

void are_get_match(struct avramT3 * av, struct MatchString * match, 
    struct MatchString * unmatched) {
    
    match->str = av->buffer;
    match->len = av->end;
    if (unmatched) {
        unmatched->str = av->buffer + av->end;
        unmatched->len = av->ibuffer - av->end;
    }
}

void are_free(struct avramT3 * av) {
    if ((av->flags & REGEX_ALLOC_BUFFER)) {
        free(av->buffer);
    }
    DFA_dest(&av->dfa);
}


#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include "test_utils.h"

_Bool verbose = 0;
char const * test_log_file = "test.log";

int check(_Bool result, char const * failmsg, ...) {
    if (!result) {
        va_list args;
        va_start(args, failmsg);
        vprintf(failmsg, args);
        va_end(args);
        return 1;
    }
    return 0;
}


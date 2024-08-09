#ifndef MATHREPL_H
#define MATHREPL_H

#include "calculatorparser.h"

#define MAX_NARGIN 2

typedef CalcValue (*REPLFunction)(CalcValue);

REPLFunction math_repl_get_func(char const * name, unsigned char length);

void print_functions(void);

void math_repl_dest(void);

#endif


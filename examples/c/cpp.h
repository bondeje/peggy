#ifndef CPP_H
#define CPP_H

#include <stddef.h>

#include "token_.h"
#include "peggy/parser.h"

typedef struct CPreProcessor CPP;

enum PPStatus {
    PP_OK
};

int CPP_directive(Parser * parser, CPP * cpp, ASTNode * directive);

int CPP_check(Parser * parser, CPP * cpp, ASTNode * identifier);

void CPP_init(CPP * cpp);

void CPP_dest(CPP * cpp);

#endif

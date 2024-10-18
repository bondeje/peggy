#ifndef CPP_H
#define CPP_H

#include <stddef.h>

#include "token_.h"
#include "peggy/parser.h"

typedef struct CPreProcessor CPP;

enum PPStatus {
    PP_OK
};

CPP * CPP_new(void);


int CPP_directive(Parser * parser, CPP * cpp);

// return 0 if a macro was identified and thus id_re is invalidated, 1 otherwise
int CPP_check(Parser * parser, CPP * cpp, ASTNode * id_re);

void CPP_del(CPP * cpp);

#endif

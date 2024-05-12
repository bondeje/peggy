#ifndef CALCPARSER_H
#define CALCPARSER_H

#include <peggy/parser.h>

#include "calculator.h"

typedef enum CalcType {
    CALC_NOTSET,
    CALC_INTEGER,
    CALC_FLOAT,
    CALC_ARRAY
} CalcType;

typedef struct CalcArray CalcArray;
typedef struct CalcValue CalcValue;

struct CalcArray {
    CalcValue * values;
    size_t length;
};

struct CalcValue {
    CalcType type;
    union {
        double d;
        long long ll;
        CalcArray arr;
    } value;
};

typedef struct CalcNode CalcNode;
struct CalcNode {
    ASTNode node;
    CalcValue value;
};

extern struct ASTNodeType CalcNode_ASTNode_class;

typedef struct CalcParser CalcParser;

struct CalcParser {
    Parser Parser;
    CalcValue result;
};

ASTNode * calc_pass0(Production * prod, Parser * parser, ASTNode * node);
ASTNode * calc_pass(Production * prod, Parser * parser, ASTNode * node);
ASTNode * calc_add_sub(Production * prod, Parser * parser, ASTNode * node);
ASTNode * calc_mul_div_mod(Production * prod, Parser * parser, ASTNode * node);
ASTNode * calc_pow(Production * prod, Parser * parser, ASTNode * node);
ASTNode * calc_unary(Production * prod, Parser * parser_, ASTNode * node);
ASTNode * func_eval(Production * prod, Parser * parser_, ASTNode * node);
ASTNode * calc_eval(Production * prod, Parser * parser_, ASTNode * node);
ASTNode * build_int(Production * prod, Parser * parser, ASTNode * node);
ASTNode * build_float(Production * prod, Parser * parser, ASTNode * node);
ASTNode * build_constants(Production * prod, Parser * parser, ASTNode * node);
ASTNode * build_array(Production * prod, Parser * parser, ASTNode * node);
ASTNode * handle_calculator(Production * prod, Parser * parser_, ASTNode * node);

void CalcNode_dest(ASTNode * value);
void CalcNode_del(ASTNode * value);

#endif
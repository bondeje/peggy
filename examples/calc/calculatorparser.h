#ifndef CALCPARSER_H
#define CALCPARSER_H

#include <complex.h>

#include "peg4c/parser.h"

//#include "calculator.h"

#define CALC_NOTSET 0
#define CALC_INTEGER 1
#define CALC_FLOAT 2
#define CALC_COMPLEX 3
#define CALC_ARRAY 4

typedef struct CalcArray CalcArray;
typedef struct CalcArrayInteger CalcArrayInteger;
typedef struct CalcArrayFloat CalcArrayFloat;
typedef struct CalcArrayComplex CalcArrayComplex;
typedef struct CalcValue CalcValue;

struct CalcArray {
    CalcValue * values;
    size_t length;
};

struct CalcValue {
    union {
        double d;
        long long ll;
        double _Complex cd;
        CalcArray arr;
    } value;
    unsigned char type;
};

#define IS_ARRAY(calc_value) ((calc_value).type == CALC_ARRAY)
#define ARRAY_SIZE(calc_value) ((calc_value).type == CALC_ARRAY ? (calc_value).value.arr.length : 1)
#define IS_SCALAR(calc_value) ((calc_value).type != CALC_ARRAY)
#define IS_REAL_SCALAR(calc_value) ((calc_value).type && (calc_value).type < CALC_COMPLEX)

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


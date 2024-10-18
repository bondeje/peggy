#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "peg4c/rule.h"
#include "peg4c/astnode.h"
#include "peg4c/parser.h"

#include "calculatorparser.h"
#include "calctr.h"
#include "mathrepl.h"

// -9223372036854775807 + '\0'
#define MAX_LLONG_STR_LENGTH 21 

// SIGN, 53 fraction digits, DEC, e/E, SIGN, 4 digit exponent  + '\0' + a few bytes
#define MAX_DBL_STR_LENGTH 64

#define CONSTANT_PI     3.14159265358979311599796346854
#define CONSTANT_TAU    (2 * CONSTANT_PI)
#define CONSTANT_EXP    2.71828182845904523536028747135
#define CONSTANT_GAMMA  0.57721566490153286060651209008

struct ASTNodeType CalcNode_ASTNode_class = {
    .type_name = "CalcNode.ASTNode",
};

ASTNode * calc_pass0(Production * prod, Parser * parser, ASTNode * node) {
    return node->children[0];
}

ASTNode * calc_pass(Production * prod, Parser * parser, ASTNode * node) {
    return node;
}

// calculate a binary operation of form "array op singleton"
void calc_as(CalcValue * array, CalcValue const * single, void (*op)(CalcValue *, CalcValue const *)) {
    size_t N = array->value.arr.length;
    CalcValue * dest_a = array->value.arr.values;
    for (size_t i = 0; i < N; i++) {
        op(dest_a + i, single);
    }
}

// calculate a binary operation of form "array op array". arrays must be the same length
void calc_aa(CalcValue * array1, CalcValue const * array2, void (*op)(CalcValue *, CalcValue const *)) {
    size_t N = array1->value.arr.length;
    CalcValue * left = array1->value.arr.values;
    CalcValue * right = array2->value.arr.values;
    for (size_t i = 0; i < N; i++) {
        op(left + i, right + i);
    }
}

void calc_add_ss(CalcValue * dest_s, CalcValue const * src_s) {
    if (dest_s->type == CALC_FLOAT || src_s->type == CALC_FLOAT) {
        double result = 0.0;
        if (dest_s->type == CALC_FLOAT) {
            result += dest_s->value.d;
        } else {
            result += dest_s->value.ll;
        }
        if (src_s->type == CALC_FLOAT) {
            result += src_s->value.d;
        } else {
            result += src_s->value.ll;
        }
        dest_s->value.d = result;
        dest_s->type = CALC_FLOAT;
    } else {
        dest_s->value.ll += src_s->value.ll;
    }
}

void calc_sub_ss(CalcValue * dest_s, CalcValue const * src_s) {
    if (dest_s->type == CALC_FLOAT || src_s->type == CALC_FLOAT) {
        double result = 0.0;
        if (dest_s->type == CALC_FLOAT) {
            result += dest_s->value.d;
        } else {
            result += dest_s->value.ll;
        }
        if (src_s->type == CALC_FLOAT) {
            result -= src_s->value.d;
        } else {
            result -= src_s->value.ll;
        }
        dest_s->value.d = result;
        dest_s->type = CALC_FLOAT;
    } else {
        dest_s->value.ll -= src_s->value.ll;
    }
}

void calc_mul_ss(CalcValue * dest_s, CalcValue const * src_s) {
    if (dest_s->type == CALC_FLOAT || src_s->type == CALC_FLOAT) {
        double result = 0.0;
        if (dest_s->type == CALC_FLOAT) {
            result += dest_s->value.d;
        } else {
            result += (double)dest_s->value.ll;
        }
        if (src_s->type == CALC_FLOAT) {
            result *= src_s->value.d;
        } else {
            result *= (double)src_s->value.ll;
        }
        dest_s->value.d = result;
        dest_s->type = CALC_FLOAT;
    } else {
        dest_s->value.ll *= src_s->value.ll;
    }
}

void calc_div_ss(CalcValue * dest_s, CalcValue const * src_s) {
    if (dest_s->type == CALC_FLOAT || src_s->type == CALC_FLOAT) {
        double result = 0.0;
        if (dest_s->type == CALC_FLOAT) {
            result += dest_s->value.d;
        } else {
            result += (double)dest_s->value.ll;
        }
        if (src_s->type == CALC_FLOAT) {
            result /= src_s->value.d;
        } else {
            result /= (double)src_s->value.ll;
        }
        dest_s->value.d = result;
        dest_s->type = CALC_FLOAT;
    } else {
        dest_s->value.ll /= src_s->value.ll;
    }
}

void calc_mod_ss(CalcValue * dest_s, CalcValue const * src_s) {
    if (dest_s->type == CALC_FLOAT || src_s->type == CALC_FLOAT) {
        double result = 0.0;
        if (dest_s->type == CALC_FLOAT) {
            result += dest_s->value.d;
        } else {
            result += (double)dest_s->value.ll;
        }
        if (src_s->type == CALC_FLOAT) {
            result = fmod(result, src_s->value.d);
        } else {
            result = fmod(result, src_s->value.ll);
        }
        dest_s->value.d = result;
        dest_s->type = CALC_FLOAT;
    } else {
        dest_s->value.ll %= src_s->value.ll;
    }
}

/* destination is the exponent and the result. */
void calc_pow_ss(CalcValue * dest_s, CalcValue const * src_s) {
    if (dest_s->type == CALC_FLOAT || src_s->type == CALC_FLOAT) {
        double result = 0.0;
        if (dest_s->type == CALC_FLOAT) {
            result += dest_s->value.d;
        } else {
            result += (double)dest_s->value.ll;
        }
        if (src_s->type == CALC_FLOAT) {
            result = pow(src_s->value.d, result);
        } else {
            result = pow((double)src_s->value.ll, result);
        }
        dest_s->value.d = result;
        dest_s->type = CALC_FLOAT;
    } else {
        if (dest_s->value.ll < 0) {
            dest_s->value.d = pow((double)src_s->value.ll, (double)dest_s->value.ll);
            dest_s->type = CALC_FLOAT;
        } else {
            dest_s->value.ll = (long long)pow((double)src_s->value.ll, (double)dest_s->value.ll);
        }
    }
}

// returns 0 on failure else the size of array result
size_t calc_make_array(CalcValue * result, size_t n) {
    CalcArray arr = {.length = n, .values = NULL};
    arr.values = malloc(sizeof(CalcValue) * n);
    if (!arr.values) {
        return 0;
    }
    for (size_t i = 0; i < n; i++) {
        arr.values[i] = *result;
    }
    result->value.arr = arr;
    result->type = CALC_ARRAY;
    return n;
}

ASTNode * calc_add_sub(Production * prod, Parser * parser, ASTNode * node) {
    size_t N = node->nchildren;
    if (N == 1) { // if no actual add/subtract
        return node->children[0]; // return node as-is
    }
    ASTNode * child = node->children[0];
    CalcValue result = ((CalcNode *)child)->value;
    size_t is_array = (result.type == CALC_ARRAY) ? result.value.arr.length : 0; // 0 for not array else the size
    if (is_array) {
        // if result is array, need to clear it from previous node.
        ((CalcNode *)node->children[0])->value.value.arr.values = NULL;
    }
    // evalute left to right adding/subtracting to result
    
    for (size_t i = 1; i < node->nchildren; i++) {
        ASTNode * operator = node->children[i++];
        child = node->children[i];
        CalcValue * rvalue = &(((CalcNode *)child)->value);
        if (rvalue->type == CALC_ARRAY) {
            if (is_array && is_array != rvalue->value.arr.length) {
                return Parser_fail_node(parser);
            } else if (!is_array) {
                if (!(is_array = calc_make_array(&result, rvalue->value.arr.length))) {
                    return Parser_fail_node(parser);
                }
            }
            switch (operator->rule) {
                case CALCTR_ADD: {
                    calc_aa(&result, rvalue, calc_add_ss);
                    break;
                }
                case CALCTR_SUBTRACT: {
                    calc_aa(&result, rvalue, calc_sub_ss);
                    break;
                }
            }
        } else if (is_array) {
            switch (operator->rule) {
                case CALCTR_ADD: {
                    calc_as(&result, rvalue, calc_add_ss);
                    break;
                }
                case CALCTR_SUBTRACT: {
                    calc_as(&result, rvalue, calc_sub_ss);
                    break;
                }
            }
        } else {
            switch (operator->rule) {
                case CALCTR_ADD: {
                    calc_add_ss(&result, rvalue);
                    break;
                }
                case CALCTR_SUBTRACT: {
                    calc_sub_ss(&result, rvalue);
                    break;
                }
            }
        }
    }
    CalcNode * node_result = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    node_result->node._class = &CalcNode_ASTNode_class;
    node_result->value = result;
    return (ASTNode *)node_result;
}

ASTNode * calc_mul_div_mod(Production * prod, Parser * parser, ASTNode * node) {
    size_t N = node->nchildren;
    if (N == 1) { // if no actual add/subtract
        return node->children[0]; // return node as-is
    }
    ASTNode * child = node->children[0];
    CalcValue result = ((CalcNode *)child)->value;
    size_t is_array = result.type == CALC_ARRAY ? result.value.arr.length : 0; // 0 for not array else the size
    if (is_array) {
        // if result is array, need to clear it from previous node.
        ((CalcNode *)child)->value.value.arr.values = NULL;
    }
    // evalute left to right adding/subtracting to result
    for (size_t i = 1; i < node->nchildren; i++) {
        ASTNode * operator = node->children[i++];
        child = node->children[i];
        CalcValue * rvalue = &(((CalcNode *)child)->value);
        if (rvalue->type == CALC_ARRAY) {
            if (is_array && is_array != rvalue->value.arr.length) {
                return Parser_fail_node(parser);;
            } else if (!is_array) {
                if (!(is_array = calc_make_array(&result, rvalue->value.arr.length))) {
                    return Parser_fail_node(parser);;
                }
            }
            switch (operator->rule) {
                case CALCTR_MULTIPLY: {
                    calc_aa(&result, rvalue, calc_mul_ss);
                    break;
                }
                case CALCTR_DIVIDE: {
                    calc_aa(&result, rvalue, calc_div_ss);
                    break;
                }
                case CALCTR_MOD: {
                    calc_aa(&result, rvalue, calc_mod_ss);
                    break;
                }
            }
        } else if (is_array) {
            switch (operator->rule) {
                case CALCTR_MULTIPLY: {
                    calc_as(&result, rvalue, calc_mul_ss);
                    break;
                }
                case CALCTR_DIVIDE: {
                    calc_as(&result, rvalue, calc_div_ss);
                    break;
                }
                case CALCTR_MOD: {
                    calc_as(&result, rvalue, calc_mod_ss);
                    break;
                }
            }
        } else {
            switch (operator->rule) {
                case CALCTR_MULTIPLY: {
                    calc_mul_ss(&result, rvalue);
                    break;
                }
                case CALCTR_DIVIDE: {
                    calc_div_ss(&result, rvalue);
                    break;
                }
                case CALCTR_MOD: {
                    calc_mod_ss(&result, rvalue);
                    break;
                }
            }
        }
    }
    CalcNode * node_result = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    node_result->node._class = &CalcNode_ASTNode_class;
    node_result->value = result;
    return (ASTNode *)node_result;
}

ASTNode * calc_pow(Production * prod, Parser * parser, ASTNode * node) {
    size_t N = node->nchildren;
    if (N == 1) { // if no actual add/subtract
        return node->children[0]; // return node as-is
    }
    N--;
    CalcValue result = ((CalcNode *)node->children[N])->value;
    size_t is_array = result.type == CALC_ARRAY ? result.value.arr.length : 0; // 0 for not array else the size
    
    if (is_array) {
        // if result is array, need to clear it from previous node.
        ((CalcNode *)node->children[N])->value.value.arr.values = NULL;
    }
    // evalute right to left
    for (; N; ) {
        N -= 2;
        CalcValue * lvalue = &(((CalcNode *)node->children[N])->value);
        if (lvalue->type == CALC_ARRAY) {
            if (is_array && is_array != lvalue->value.arr.length) {
                return Parser_fail_node(parser);;
            } else if (!is_array) {
                if (!(is_array = calc_make_array(&result, lvalue->value.arr.length))) {
                    return Parser_fail_node(parser);;
                }
            }
            calc_aa(&result, lvalue, calc_pow_ss);
        } else if (is_array) {
            calc_as(&result, lvalue, calc_pow_ss);
        } else {
            calc_pow_ss(&result, lvalue);
        }
    }

    CalcNode * node_result = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    node_result->node._class = &CalcNode_ASTNode_class;
    node_result->value = result;
    return (ASTNode *)node_result;
}

ASTNode * func_eval(Production * prod, Parser * parser, ASTNode * node) {
    Token * func_name = node->token_start;
    ASTNode * argsnode = node->children[2];
    size_t nargin = (argsnode->nchildren + 1) >> 1;
    CalcValue vals;
    if (nargin > 1) {
        CalcValue args[MAX_NARGIN] = {0};
        size_t i = 0;
        for (size_t j = 0; j < argsnode->nchildren; j += 2) {
            args[i++] = ((CalcNode *)argsnode->children[j])->value;
        }
        vals.type = CALC_ARRAY;
        vals.value.arr.length = nargin;
        vals.value.arr.values = &args[0];
    } else {
        vals = ((CalcNode *)argsnode->children[0])->value;
    }
    
    REPLFunction func = math_repl_get_func(func_name->string, (unsigned char) func_name->length);
    CalcNode * result = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(*result));
    result->node._class = &CalcNode_ASTNode_class;
    result->value = func(vals);
    return (ASTNode *)result;
}

ASTNode * calc_eval(Production * prod, Parser * parser, ASTNode * node) {
    switch (node->rule) {
        case CALCTR_INTEGER: {} FALLTHROUGH
        case CALCTR_FLOAT: {} FALLTHROUGH
        case CALCTR_CONSTANTS: {} FALLTHROUGH
        case CALCTR_ARRAY: {} FALLTHROUGH
        case CALCTR_FUNCTION_CALL: {
            return node;
        }
        default: { // parenthesized expression
            return node->children[1];
        }
    }
    return Parser_fail_node(parser);;
}

const CalcValue NEG_ONE = {.type = CALC_INTEGER, .value.ll = -1};

// TODO: not sure this one is right
ASTNode * calc_unary(Production * prod, Parser * parser, ASTNode * node) {
    size_t n_odd = 0;
    ASTNode * unary_repeat = node->children[0];
    size_t Nunary = unary_repeat->nchildren;
    for (size_t i = 0; i < Nunary; i++) {
        if (unary_repeat->children[i]->rule == CALCTR_SUBTRACT) {
            n_odd++;
        }
    }
    if (n_odd & 1) {
        CalcNode * result = (CalcNode * )node->children[1];
        switch (result->value.type) {
            case CALC_INTEGER: {
                result->value.value.ll *= -1;
                break;
            }
            case CALC_FLOAT: {
                result->value.value.d *= -1;
                break;
            }
            case CALC_ARRAY: {
                calc_as(&result->value, &NEG_ONE, calc_mul_ss);
                break;
            }
            case CALC_NOTSET: {
                /* error condition */
                break;
            }
        }
    }
    return node->children[1];
}


ASTNode * build_int(Production * prod, Parser * parser, ASTNode * node) {
    static char integer_buffer[MAX_LLONG_STR_LENGTH] = {'\0'};
    Token * tok = node->token_start;
    /* at this point, the token does not have tok->length set properly so use the node->str_length */
    size_t length = node->str_length;
    if (length >= MAX_LLONG_STR_LENGTH) {
        return NULL;
    }
    CalcNode * val = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    memcpy(integer_buffer, tok->string, length);
    integer_buffer[length] = '\0';
    val->value.value.ll = strtoll(integer_buffer, NULL, 10);
    val->value.type = CALC_INTEGER;
    return (ASTNode *)val;
}

ASTNode * build_float(Production * prod, Parser * parser, ASTNode * node) {
    static char float_buffer[MAX_DBL_STR_LENGTH] = {'\0'};
    Token * tok = node->token_start;
    /* at this point, the token does not have tok->length set properly so use the node->str_length */
    size_t length = node->str_length;
    if (length >= MAX_DBL_STR_LENGTH) {
        return NULL;
    }
    CalcNode * val = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    memcpy(float_buffer, tok->string, length);
    float_buffer[length] = '\0';
    val->value.value.d = strtod(float_buffer, NULL);
    val->value.type = CALC_FLOAT;
    return (ASTNode *)val;
}

ASTNode * build_constants(Production * prod, Parser * parser, ASTNode * node) {
    CalcNode * val = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    val->value.type = CALC_FLOAT;
    switch (node->rule) {
        case CALCTR_PI_KW: {
            val->value.value.d = CONSTANT_PI;
            break;
        }
        case CALCTR_TAU_KW: {
            val->value.value.d = CONSTANT_TAU;
            break;
        }
        case CALCTR_E_KW: {
            val->value.value.d = CONSTANT_EXP;
            break;
        }
        case CALCTR_GAMMA_KW: {
            val->value.value.d = CONSTANT_GAMMA;
            break;
        }
        case CALCTR_PREV_KW: { // copy in the previous result into the current calculation
            val->value = ((CalcParser *)parser)->result;
            break;
        }
    }
    
    return (ASTNode *)val;
}

ASTNode * build_array(Production * prod, Parser * parser, ASTNode * node) {
    ASTNode * child = node->children[1];
    //ASTNode * child = node->child->next;
    size_t length = (child->nchildren + 1) / 2;
    CalcValue * arr = malloc(sizeof(CalcValue) * length);
    if (!arr) {
        return Parser_fail_node(parser);;
    }
    size_t j = 0;
    for (size_t i = 0; i < child->nchildren; i += 2) {
        arr[j++] = ((CalcNode *)child->children[i])->value;
    }
    CalcNode * val = (CalcNode *)Parser_add_node(parser, ((Rule *)prod)->id, 
        node->token_start, node->token_end, node->str_length, 0, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    val->value.value.arr.length = length;
    val->value.value.arr.values = arr;
    val->value.type = CALC_ARRAY;
    return (ASTNode *)val;
}

void CalcValue_print(CalcValue * value, char const * terminator) {
    switch (value->type) {
        case CALC_INTEGER: {
            printf("%lld%s", value->value.ll, terminator);
            break;
        }
        case CALC_FLOAT: {
            printf("%f%s", value->value.d, terminator);
            break;
        }
        case CALC_ARRAY: {
            printf("[");
            size_t N = value->value.arr.length - 1;
            size_t i = 0;
            CalcValue * arr = value->value.arr.values;
            for (; i < N; i++) {
                CalcValue_print(arr + i, "");
                printf(",");
            }
            CalcValue_print(arr + i, "");
            printf("]%s", terminator);
            break;
        }
        case CALC_NOTSET: {
            printf("calc notset not handled\n");
            break;
        }
        default: {
            //printf("unknown value type\n");
        }
    }
}

ASTNode * handle_calculator(Production * prod, Parser * parser_, ASTNode * node) {
    CalcParser * parser = (CalcParser *) parser_;
    for (size_t i = 0; i < node->nchildren; i++) {
        parser->result = ((CalcNode *)node->children[i])->value;
        CalcValue_print(&parser->result, "\n");
    }
    node->rule = ((Rule *)prod)->id;
    return node;
}

void print_help(void) {
    printf(
        "Welcome to the libc math calculator. Use this as a REPL interpreter \n"
        "of (vectorized) basic math operators and function calls for those in\n"
        "math.h (and soon cmath.h).\n\n"
          "\tOperators available: +, -, /, *, %%, ^ (power, not bitwise xor)\n"
          "\tConstants avialable: pi, tau (=2pi), e (base of ln), gamma\n"
          "\tFunctions available: type 'functions' for a full list\n"
          "\tClose: type 'exit'\n"
          "\tNOTE: sadly I have not implemented a function call memory yet\n");
}

static inline void new_input(void) {
    printf("> ");
}

int main(int narg, char ** args) {
    static char buffer[1024] = {'\0'};
    print_help();
    new_input();
    size_t N = strlen(fgets(buffer, 1024, stdin));
    CalcParser calc = {
        .Parser = {
            ._class = &Parser_class,
        }
    };
    bool EXIT_COND = strcmp("exit\n", buffer) == 0;
    while (!EXIT_COND) {
        if (!strcmp("help\n", buffer)) {
            print_help();
        } else if (!strcmp("functions\n", buffer)) {
            print_functions();
        } else {
            Parser_init((Parser *)&calc, calctrrules, CALCTR_NRULES, CALCTR_TOKEN, CALCTR_CALCULATOR, 0);
            Parser_parse((Parser *)&calc, buffer, strlen(buffer));
            Parser_dest((Parser *)&calc);
        }
        new_input();
        N = strlen(fgets(buffer, 1024, stdin));
        EXIT_COND = strcmp("exit\n", buffer) == 0;
    }
    calctr_dest();
    math_repl_dest();
    return 0;
}


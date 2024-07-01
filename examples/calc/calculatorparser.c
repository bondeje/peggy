#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include <peggy/rule.h>
#include <peggy/astnode.h>
#include <peggy/parser.h>
#include <logger.h>

#include "calculatorparser.h"
#include "calculator.h"
#include "mathrepl.h"

// -9223372036854775807 + '\0'
#define MAX_LLONG_STR_LENGTH 21 

// SIGN, 53 fraction digits, DEC, e/E, SIGN, 4 digit exponent  + '\0' + a few bytes
#define MAX_DBL_STR_LENGTH 64

#define CONSTANT_PI     3.14159265358979311599796346854
#define CONSTANT_TAU    (2 * CONSTANT_PI)
#define CONSTANT_EXP    2.71828182845904523536028747135
#define CONSTANT_GAMMA  0.57721566490153286060651209008

void CalcValue_dest(CalcValue * value) {
    if ((value->type == CALC_ARRAY) && value->value.arr.values) {
        free(value->value.arr.values);
        value->value.arr.values = NULL;
        value->value.arr.length = 0;
    }
}

void CalcNode_dest(ASTNode * node_) {
    CalcNode * node = (CalcNode *)node_;
    ASTNode_dest(&node->node);
    CalcValue_dest(&node->value);
}

void CalcNode_del(ASTNode * node) {
    CalcNode_dest(node);
    free(node);
}

struct ASTNodeType CalcNode_ASTNode_class = {
    .type_name = "CalcNode.ASTNode",
    .new = &ASTNode_new,
    .init = &ASTNode_init,
    .dest = &CalcNode_dest,
    .del = &CalcNode_del,
    .iter = &ASTNode_iter,
    .reverse = &ASTNode_reverse,
    .get = &ASTNode_get,
    .len = &ASTNode_len,
    .str = &ASTNode_str,
};

ASTNode * calc_pass0(Production * prod, Parser * parser, ASTNode * node) {
    node->children[0]->token_key = node->token_key;
    node->children[0]->ntokens = node->ntokens;
    node->children[0]->str_length = node->str_length;
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
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - passing to expression. tokens %zu-%zu\n", __func__, node->token_key, node->token_key + node->ntokens); 
        return node->children[0]; // return node as-is
    }
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - adding/subtracting %zu subexpressions. tokens %zu-%zu\n", __func__, N, node->token_key, node->token_key + node->ntokens);
    CalcValue result = ((CalcNode *)node->children[0])->value;
    size_t is_array = (result.type == CALC_ARRAY) ? result.value.arr.length : 0; // 0 for not array else the size
    if (is_array) {
        // if result is array, need to clear it from previous node.
        ((CalcNode *)node->children[0])->value.value.arr.values = NULL;
    }
    // evalute left to right adding/subtracting to result
    for (size_t i = 2; i < N; i += 2) {
        ASTNode * operator = node->children[i - 1];
        CalcValue * rvalue = &(((CalcNode *)(node->children[i]))->value);
        if (rvalue->type == CALC_ARRAY) {
            if (is_array && is_array != rvalue->value.arr.length) {
                return &ASTNode_fail;
            } else if (!is_array) {
                LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - extending result to array of size %zu. tokens %zu-%zu\n", __func__, rvalue->value.arr.length, node->token_key, node->token_key + node->ntokens);
                if (!(is_array = calc_make_array(&result, rvalue->value.arr.length))) {
                    return &ASTNode_fail;
                }
            }
            switch (operator->rule->id) {
                case ADD: {
                    calc_aa(&result, rvalue, calc_add_ss);
                    break;
                }
                case SUBTRACT: {
                    calc_aa(&result, rvalue, calc_sub_ss);
                    break;
                }
            }
        } else if (is_array) {
            switch (operator->rule->id) {
                case ADD: {
                    calc_as(&result, rvalue, calc_add_ss);
                    break;
                }
                case SUBTRACT: {
                    calc_as(&result, rvalue, calc_sub_ss);
                    break;
                }
            }
        } else {
            switch (operator->rule->id) {
                case ADD: {
                    calc_add_ss(&result, rvalue);
                    break;
                }
                case SUBTRACT: {
                    calc_sub_ss(&result, rvalue);
                    break;
                }
            }
        }
    }
    CalcNode * node_result = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
    node_result->node._class = &CalcNode_ASTNode_class;
    node_result->value = result;
    return (ASTNode *)node_result;
}

ASTNode * calc_mul_div_mod(Production * prod, Parser * parser, ASTNode * node) {
    size_t N = node->nchildren;
    if (N == 1) { // if no actual add/subtract
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - passing to add_sub_expression. tokens %zu-%zu\n", __func__, node->token_key, node->token_key + node->ntokens); 
        return node->children[0]; // return node as-is
    }
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - mul/div/mod %zu subexpressions. tokens %zu-%zu\n", __func__, N, node->token_key, node->token_key + node->ntokens);
    CalcValue result = ((CalcNode *)node->children[0])->value;
    size_t is_array = result.type == CALC_ARRAY ? result.value.arr.length : 0; // 0 for not array else the size
    if (is_array) {
        // if result is array, need to clear it from previous node.
        ((CalcNode *)node->children[0])->value.value.arr.values = NULL;
    }
    // evalute left to right adding/subtracting to result
    for (size_t i = 2; i < N; i += 2) {
        ASTNode * operator = node->children[i - 1];
        CalcValue * rvalue = &(((CalcNode *)(node->children[i]))->value);
        if (rvalue->type == CALC_ARRAY) {
            if (is_array && is_array != rvalue->value.arr.length) {
                return &ASTNode_fail;
            } else if (!is_array) {
                LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - extending result to array of size %zu. tokens %zu-%zu\n", __func__, rvalue->value.arr.length, node->token_key, node->token_key + node->ntokens);
                if (!(is_array = calc_make_array(&result, rvalue->value.arr.length))) {
                    return &ASTNode_fail;
                }
            }
            switch (operator->rule->id) {
                case MULTIPLY: {
                    calc_aa(&result, rvalue, calc_mul_ss);
                    break;
                }
                case DIVIDE: {
                    calc_aa(&result, rvalue, calc_div_ss);
                    break;
                }
                case MOD: {
                    calc_aa(&result, rvalue, calc_mod_ss);
                    break;
                }
            }
        } else if (is_array) {
            switch (operator->rule->id) {
                case MULTIPLY: {
                    calc_as(&result, rvalue, calc_mul_ss);
                    break;
                }
                case DIVIDE: {
                    calc_as(&result, rvalue, calc_div_ss);
                    break;
                }
                case MOD: {
                    calc_as(&result, rvalue, calc_mod_ss);
                    break;
                }
            }
        } else {
            switch (operator->rule->id) {
                case MULTIPLY: {
                    calc_mul_ss(&result, rvalue);
                    break;
                }
                case DIVIDE: {
                    calc_div_ss(&result, rvalue);
                    break;
                }
                case MOD: {
                    calc_mod_ss(&result, rvalue);
                    break;
                }
            }
        }
    }
    CalcNode * node_result = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
    node_result->node._class = &CalcNode_ASTNode_class;
    node_result->value = result;
    return (ASTNode *)node_result;
}

ASTNode * calc_pow(Production * prod, Parser * parser, ASTNode * node) {
    size_t N = node->nchildren;
    if (N == 1) { // if no actual add/subtract
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - passing to mul_div_mod_expression. tokens %zu-%zu\n", __func__, node->token_key, node->token_key + node->ntokens); 
        return node->children[0]; // return node as-is
    }
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - power %zu subexpressions. tokens %zu-%zu\n", __func__, N, node->token_key, node->token_key + node->ntokens);
    CalcValue result = ((CalcNode *)node->children[--N])->value;
    size_t is_array = result.type == CALC_ARRAY ? result.value.arr.length : 0; // 0 for not array else the size
    if (is_array) {
        // if result is array, need to clear it from previous node.
        ((CalcNode *)node->children[N])->value.value.arr.values = NULL;
    }
    // evalute left to right adding/subtracting to result
    for (; N; ) {
        N -= 2;
        CalcValue * lvalue = &(((CalcNode *)(node->children[N]))->value);
        if (lvalue->type == CALC_ARRAY) {
            if (is_array && is_array != lvalue->value.arr.length) {
                return &ASTNode_fail;
            } else if (!is_array) {
                LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - extending result to array of size %zu. tokens %zu-%zu\n", __func__, lvalue->value.arr.length, node->token_key, node->token_key + node->ntokens);
                if (!(is_array = calc_make_array(&result, lvalue->value.arr.length))) {
                    return &ASTNode_fail;
                }
            }
            calc_aa(&result, lvalue, calc_pow_ss);
        } else if (is_array) {
            calc_as(&result, lvalue, calc_pow_ss);
        } else {
            calc_pow_ss(&result, lvalue);
        }
    }

    CalcNode * node_result = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
    node_result->node._class = &CalcNode_ASTNode_class;
    node_result->value = result;
    return (ASTNode *)node_result;
}

ASTNode * func_eval(Production * prod, Parser * parser, ASTNode * node) {
    Token * func_name = parser->_class->get_tokens(parser, node->children[0]);
    size_t nargin = (node->children[2]->nchildren + 1) >> 1;
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - calling function %.*s with %zu arguments\n", __func__, (int)func_name->length, func_name->string, nargin);
    CalcValue vals;
    if (nargin > 1) {
        CalcValue args[MAX_NARGIN] = {0};
        for (size_t i = 0; i < nargin; i++) {
            args[i] = ((CalcNode *)(node->children[2]->children[i << 1]))->value;
        }
        vals.type = CALC_ARRAY;
        vals.value.arr.length = nargin;
        vals.value.arr.values = &args[0];
    } else {
        vals = ((CalcNode *)node->children[2]->children[0])->value;
    }
    
    REPLFunction func = math_repl_get_func(func_name->string, (unsigned char) func_name->length);
    CalcNode * result = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(*result));
    result->node._class = &CalcNode_ASTNode_class;
    result->value = func(vals);
    return (ASTNode *)result;
}

ASTNode * calc_eval(Production * prod, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - tokens %zu-%zu\n", __func__, node->token_key, node->token_key + node->ntokens);
    switch (node->rule->id) {
        case INTEGER: {} __attribute__((fallthrough));
        case FLOAT: {} __attribute__((fallthrough));
        case CONSTANTS: {} __attribute__((fallthrough));
        case ARRAY: {} __attribute__((fallthrough));
        case FUNCTION_CALL: {
            return node;
        }
        default: { // parenthesized expression
            node->children[1]->token_key = node->token_key;
            node->children[1]->ntokens = node->ntokens;
            node->children[1]->str_length = node->str_length;
            return node->children[1];
        }
    }
    return &ASTNode_fail;
}

const CalcValue NEG_ONE = {.type = CALC_INTEGER, .value.ll = -1};

ASTNode * calc_unary(Production * prod, Parser * parser, ASTNode * node) {
    size_t n_odd = 0;
    ASTNode * unary_repeat = node->children[0];
    size_t Nunary = unary_repeat->nchildren;
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - calc unary with %zu operators. tokens %zu-%zu\n", __func__, Nunary, node->token_key, node->token_key + node->ntokens);
    for (size_t i = 0; i < Nunary; i++) {
        if (unary_repeat->children[i]->rule->id == SUBTRACT) {
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
                calc_as(&result->value, &NEG_ONE, calc_add_ss);
                break;
            }
            case CALC_NOTSET: {
                /* error condition */
                break;
            }
        }
    }

    node->children[1]->token_key = node->token_key;
    node->children[1]->ntokens = node->ntokens;
    node->children[1]->str_length = node->str_length;
    return node->children[1];
}


ASTNode * build_int(Production * prod, Parser * parser, ASTNode * node) {
    static char integer_buffer[MAX_LLONG_STR_LENGTH] = {'\0'};
    Token * tok = parser->_class->get_tokens(parser, node);
    /* at this point, the token does not have tok->length set properly so use the node->str_length */
    size_t length = node->str_length;
    if (length >= MAX_LLONG_STR_LENGTH) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - integer value %.*s at line %u, col %u has too many digits to fit in long long. cannot convert\n", __func__, (int)length, tok->string, tok->coords.line, tok->coords.col);
        return NULL;
    }
    CalcNode * val = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    memcpy(integer_buffer, tok->string, length);
    integer_buffer[length] = '\0';
    val->value.value.ll = strtoll(integer_buffer, NULL, 10);
    val->value.type = CALC_INTEGER;
    return (ASTNode *)val;
}

ASTNode * build_float(Production * prod, Parser * parser, ASTNode * node) {
    static char float_buffer[MAX_DBL_STR_LENGTH] = {'\0'};
    Token * tok = parser->_class->get_tokens(parser, node);
    /* at this point, the token does not have tok->length set properly so use the node->str_length */
    size_t length = node->str_length;
    if (length >= MAX_DBL_STR_LENGTH) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - float value %.*s at line %u, col %u has too many digits to fit in double. cannot convert\n", __func__, (int)length, tok->string, tok->coords.line, tok->coords.col);
        return NULL;
    }
    CalcNode * val = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    memcpy(float_buffer, tok->string, length);
    float_buffer[length] = '\0';
    val->value.value.d = strtod(float_buffer, NULL);
    val->value.type = CALC_FLOAT;
    return (ASTNode *)val;
}

ASTNode * build_constants(Production * prod, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG %s - constant found\n", __func__);
    CalcNode * val = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
    val->node._class = &CalcNode_ASTNode_class;
    val->value.type = CALC_FLOAT;
    switch (node->rule->id) {
        case PI_KW: {
            val->value.value.d = CONSTANT_PI;
            break;
        }
        case TAU_KW: {
            val->value.value.d = CONSTANT_TAU;
            break;
        }
        case E_KW: {
            val->value.value.d = CONSTANT_EXP;
            break;
        }
        case GAMMA_KW: {
            val->value.value.d = CONSTANT_GAMMA;
            break;
        }
        case PREV_KW: { // copy in the previous result into the current calculation
            val->value = ((CalcParser *)parser)->result;
            break;
        }
    }
    
    return (ASTNode *)val;
}

ASTNode * build_array(Production * prod, Parser * parser, ASTNode * node) {
    size_t length = (node->children[1]->nchildren + 1) / 2;
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - building array of size %zu. tokens %zu-%zu\n", __func__, length, node->token_key, node->token_key + node->ntokens);
    CalcValue * arr = malloc(sizeof(CalcValue) * length);
    if (!arr) {
        return &ASTNode_fail;
    }
    size_t j = 0;
    size_t N = node->children[1]->nchildren;
    ASTNode ** children = node->children[1]->children;
    for (size_t i = 0; i < N; i += 2) {
        arr[j++] = ((CalcNode *)children[i])->value;
    }
    CalcNode * val = (CalcNode *)parser->_class->add_node(parser, (Rule *)prod, 
        node->token_key, node->ntokens, node->str_length, 0, 
        NULL, sizeof(CalcNode));
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
            printf("unknown value type\n");
        }
    }
}

ASTNode * handle_calculator(Production * prod, Parser * parser_, ASTNode * node) {
    CalcParser * parser = (CalcParser *) parser_;
    //printf("handle_calculator node type_name: %s\n", node->rule->_class->type_name);
    for (size_t i = 0; i < node->nchildren; i++) {
        //printf("handle_calculator child %zu: %d\n", i, node->children[i]->rule->id);
        parser->result = ((CalcNode *)node->children[i])->value;
        CalcValue_print(&parser->result, "\n");
    }
    node->rule = (Rule *)prod;
    return node;
}

void print_help(void) {
    printf(
        "Welcome to the libc math calculator. Use this as a REPL interpreter \n"
        "of (vectorized) basic math operators and function calls for those in\n"
        "math.h (and soon cmath.h).\n\n"
          "\tOperators available: +, -, /, *, %%, ^ (power, not bitwise xor)\n"
          "\tConstants avialable: pi, tau (=2pi), exp, gamma\n"
          "\tFunctions available: type 'functions for a full list\n");
}

inline void new_input(void) {
    printf("> ");
}

int main(int narg, char ** args) {
    static char buffer[1024] = {'\0'};
    print_help();
    new_input();
    size_t N = strlen(fgets(buffer, 1024, stdin));
    CalcParser calc = {.Parser = Parser_DEFAULT_INIT};
    char * log_file = NULL;
    unsigned char log_level = LOG_LEVEL_WARN; // default to logging warning
    if (narg > 1) {
        log_file = args[1];
    }
    if (narg > 2) { 
        log_level = Logger_level_to_uchar(args[2], strlen(args[2]));
    }
    bool EXIT_COND = strcmp("exit\n", buffer) == 0;
    while (!EXIT_COND) {
        if (!strcmp("help\n", buffer)) {
            print_help();
        } else if (!strcmp("functions\n", buffer)) {
            print_functions();
        } else {
            calc.Parser._class->init(&calc.Parser, "", 0, buffer, strlen(buffer),
                (Rule *)&calculator_token, (Rule *)&calculator_calculator, CALCULATOR_NRULES, 
                0, 0, 0, log_file, log_level);
            //Parser_print_ast(&calc.Parser, NULL);
            calc.Parser._class->dest(&calc.Parser);
        }
        new_input();
        N = strlen(fgets(buffer, 1024, stdin));
        EXIT_COND = strcmp("exit\n", buffer) == 0;
    }
    calculator_dest();
    math_repl_dest();
    Logger_tear_down();
    return 0;
}
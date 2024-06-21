#include <stddef.h>
#include <complex.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include "mathrepl.h"
#include "calculatorparser.h"

#define UNARY_NARG 1
#define BINARY_NARG 2

#ifndef STRINGIFY
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#endif

#ifndef CAT
#define CAT_(x, y) x##y
#define CAT(x, y) CAT_(x, y)
#endif

#define CHECK_NARG(func_string, nvals, req_vals) \
if (nvals != req_vals) { \
    printf("%s called with improper number of arguments\n", func_string);\
    return err_value;\
}

#define EVAL_UNARY_ARRAY(func, vals) \
size_t length = (vals).value.arr.length; \
CalcValue result = {.type = CALC_ARRAY, .value = {.arr = {.values = NULL, .length = length}}};\
CalcValue * out_values = malloc(length * sizeof(*out_values));\
if (!out_values) {\
    printf("%s malloc failure\n", STRINGIFY(func));\
    return err_value;\
}\
CalcValue * in_vals = (vals).value.arr.values;\
for (size_t i = 0; i < length; i++) {\
    out_values[i] = CAT(func, _)(in_vals[i]);\
}\
result.value.arr.values = out_values;\
return result;

#define TEMPLATE_CAST_INT(func)\
CalcValue CAT(func, _)(CalcValue vals) {\
    switch (vals.type) {\
        case CALC_INTEGER: {\
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = func((double)vals.value.ll)}};\
        }\
        case CALC_FLOAT: {\
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = func(vals.value.d)}};\
        }\
        case CALC_COMPLEX: {\
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = CAT(c, func)(vals.value.cd)}};\
        }\
        case CALC_ARRAY: {\
            EVAL_UNARY_ARRAY(func, vals)\
        }\
        default: {\
            printf("%s called with values of unknown type\n", STRINGIFY(func));\
            return err_value;\
        }\
    }\
}

#define TEMPLATE_CAST_INT_NOCOMPLEX(func)\
CalcValue CAT(func, _)(CalcValue vals) {\
    switch (vals.type) {\
        case CALC_INTEGER: {\
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = func((double)vals.value.ll)}};\
        }\
        case CALC_FLOAT: {\
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = func(vals.value.d)}};\
        }\
        case CALC_COMPLEX: {\
            printf("%s has no implementation for complex numbers\n", STRINGIFY(func));\
            return err_value;\
        }\
        case CALC_ARRAY: {\
            EVAL_UNARY_ARRAY(func, vals)\
        }\
        default: {\
            printf("%s called with values of unknown type\n", STRINGIFY(func));\
            return err_value;\
        }\
    }\
}

typedef struct REPLFunctionKey {
    char const * str;
    unsigned char len;
} REPLFunctionKey;

int REPLFunctionKey_comp(REPLFunctionKey a, REPLFunctionKey b) {
    if (a.len != b.len) {
        return 1;
    }
    int res = strncmp(a.str, b.str, a.len);
    return res;
}

size_t REPLFunctionKey_hash(REPLFunctionKey key, size_t bin_size) {
    unsigned long long hash = 5381;
    size_t i = 0;
    unsigned char * str = (unsigned char *) key.str;

    while (i < key.len) {
        hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
        str++;
        i++;
    }
    return hash % bin_size;
}

long long llmod(long long dividend, long long divisor) {
    return dividend % divisor;
}

CalcValue err_value = {.type = CALC_NOTSET, .value = {0}};

CalcValue err_func_(CalcValue vals) {
    return err_value;
}

CalcValue abs_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            long long val = vals.value.ll;
            if (val == LLONG_MIN) { // handle edge case that is UB in llabs
                return (CalcValue) {.type = CALC_FLOAT, .value = {.d = -1.0 * (double)LLONG_MIN}};
            }
            return (CalcValue) {.type = CALC_INTEGER, .value = {.ll = llabs(val)}};;
        }
        case CALC_FLOAT: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = fabs(vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = cabs(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(abs, vals)
        }
        default: {
            printf("%s called with values of unknown type\n", "abs");
            return err_value;
        }
    }
}

CalcValue mod_(CalcValue vals) {
    if (!IS_ARRAY(vals)) {
        printf("%s called without array argument\n", "mod");
        return err_value;
    }
    CHECK_NARG("mod", vals.value.arr.length, BINARY_NARG)
    CalcValue * dividend = vals.value.arr.values;
    CalcValue * divisor = vals.value.arr.values + 1;           
    if (IS_ARRAY(*dividend) || IS_ARRAY(*divisor)) {
        size_t length_divid = ARRAY_SIZE(*dividend);
        size_t length_divis = ARRAY_SIZE(*divisor);
        if (length_divis > 1 && length_divid > 1 && length_divis != length_divid) {
            printf("array mismatch in call to %s: %zu != %zu\n", "mod", length_divid, length_divis);
            return err_value;
        }
        size_t length = (length_divid > length_divis ? length_divid : length_divis);
        CalcValue result = {.type = CALC_ARRAY, .value = {.arr = {.values = NULL, .length = length}}};
        CalcValue * values = malloc(length * sizeof(*values));
        if (!values) {
            printf("%s malloc failure\n", "mod");
            return err_value;
        }
        CalcValue args[2];
        CalcValue input_args = {.type = CALC_ARRAY, .value = {.arr = {.values = &args[0], .length = BINARY_NARG}}};
        if (length_divid == 1) {
            args[0] = *dividend;
            for (size_t i = 0; i < length; i++) {
                args[1] = divisor->value.arr.values[i];
                values[i] = mod_(input_args);
            }
        } else if (length_divis == 1) {
            args[1] = *divisor;
            for (size_t i = 0; i < length; i++) {
                args[0] = dividend->value.arr.values[i];
                values[i] = mod_(input_args);
            }
        } else {
            for (size_t i = 0; i < length; i++) {
                args[0] = dividend->value.arr.values[i];
                args[1] = divisor->value.arr.values[i];
                values[i] = mod_(input_args);
            }
        }
        result.value.arr.values = values;
        return result;
    } else {
        if (!IS_REAL_SCALAR(*dividend) || !IS_REAL_SCALAR(*divisor)) {
            printf("%s does not accept complex arguments\n", "mod");
            return err_value;
        }
        if (dividend->type != divisor->type || divisor->type == CALC_FLOAT) {
            double divid;
            double divis;
            if (dividend->type == CALC_INTEGER) {
                divid = (double) dividend->value.ll;
            } else {
                divid = dividend->value.d;
            }
            if (divisor->type == CALC_INTEGER) {
                divis = (double) divisor->value.ll;
            } else {
                divis = divisor->value.d;
            }
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = fmod(divid, divis)}};
        } else {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = llmod(dividend->value.ll, divisor->value.ll)}};
        }        
    }
    return err_value;
}

CalcValue * max_value(CalcValue * _1, CalcValue * _2) {
    if (_1->type == CALC_INTEGER) {
        if (_2->type == CALC_INTEGER) {
            return _1->value.ll > _2->value.ll ? _1 : _2;
        } else {
            return _1->value.ll > _2->value.d ? _1 : _2;
        }
    } else {
        if (_2->type == CALC_INTEGER) {
            return _1->value.d > _2->value.ll ? _1 : _2;
        } else {
            return _1->value.d > _2->value.d ? _1 : _2;
        }
    }
}

CalcValue max_(CalcValue vals) {
    switch (vals.type) {
        case CALC_ARRAY: {
            size_t length = ARRAY_SIZE(vals);
            CalcValue * result = vals.value.arr.values;
            CalcValue * next = vals.value.arr.values + 1;
            for (size_t i = 1; i < length; i++) {
                result = max_value(result, next);
                next++;
            }
            return *result;
        }
        default: {
            return vals;
        }
    }
    return err_value;
}

CalcValue * min_value(CalcValue * _1, CalcValue * _2) {
    if (_1->type == CALC_INTEGER) {
        if (_2->type == CALC_INTEGER) {
            return _1->value.ll > _2->value.ll ? _2 : _1;
        } else {
            return _1->value.ll > _2->value.d ? _2 : _1;
        }
    } else {
        if (_2->type == CALC_INTEGER) {
            return _1->value.d > _2->value.ll ? _2 : _1;
        } else {
            return _1->value.d > _2->value.d ? _2 : _1;
        }
    }
}

CalcValue min_(CalcValue vals) {
    switch (vals.type) {
        case CALC_ARRAY: {
            size_t length = ARRAY_SIZE(vals);
            CalcValue * result = vals.value.arr.values;
            CalcValue * next = vals.value.arr.values + 1;
            for (size_t i = 1; i < length; i++) {
                result = min_value(result, next);
                next++;
            }
            return *result;
        }
        default: {
            return vals;
        }
    }
    return err_value;
}

TEMPLATE_CAST_INT(exp)
TEMPLATE_CAST_INT_NOCOMPLEX(exp2)
TEMPLATE_CAST_INT(log)
TEMPLATE_CAST_INT_NOCOMPLEX(log10)
TEMPLATE_CAST_INT_NOCOMPLEX(log2)

CalcValue pow_(CalcValue vals) {
    if (!IS_ARRAY(vals)) {
        printf("%s called without array argument\n", "pow");
        return err_value;
    }
    CHECK_NARG("pow", vals.value.arr.length, BINARY_NARG)
    CalcValue * base = vals.value.arr.values;
    CalcValue * expo = vals.value.arr.values + 1;           
    if (IS_ARRAY(*base) || IS_ARRAY(*expo)) {
        size_t length_base = ARRAY_SIZE(*base);
        size_t length_expo = ARRAY_SIZE(*expo);
        if (length_expo > 1 && length_base > 1 && length_expo != length_base) {
            printf("array mismatch in call to %s: %zu != %zu\n", "pow", length_base, length_expo);
            return err_value;
        }
        size_t length = (length_base > length_expo ? length_base : length_expo);
        CalcValue result = {.type = CALC_ARRAY, .value = {.arr = {.values = NULL, .length = length}}};
        CalcValue * values = malloc(length * sizeof(*values));
        if (!values) {
            printf("%s malloc failure\n", "pow");
            return err_value;
        }
        CalcValue args[2];
        CalcValue input_args = {.type = CALC_ARRAY, .value = {.arr = {.values = &args[0], .length = BINARY_NARG}}};
        if (length_base == 1) {
            args[0] = *base;
            for (size_t i = 0; i < length; i++) {
                args[1] = expo->value.arr.values[i];
                values[i] = pow_(input_args);
            }
        } else if (length_expo == 1) {
            args[1] = *expo;
            for (size_t i = 0; i < length; i++) {
                args[0] = base->value.arr.values[i];
                values[i] = pow_(input_args);
            }
        } else {
            for (size_t i = 0; i < length; i++) {
                args[0] = base->value.arr.values[i];
                args[1] = expo->value.arr.values[i];
                values[i] = pow_(input_args);
            }
        }
        result.value.arr.values = values;
        return result;
    } else {
        if (!IS_REAL_SCALAR(*base) || !IS_REAL_SCALAR(*expo)) {
            double _Complex base_ = 0;
            double _Complex expo_ = 0;
            switch (base->type) {
                case CALC_INTEGER: {
                    base_ = (double _Complex) base->value.ll;
                    break;
                }
                case CALC_FLOAT: {
                    base_ = (double _Complex) base->value.d;
                    break;
                }
                case CALC_COMPLEX: {
                    base_ = base->value.cd;
                    break;
                }
                default: {}
            }
            switch (expo->type) {
                case CALC_INTEGER: {
                    expo_ = (double _Complex) expo->value.ll;
                    break;
                }
                case CALC_FLOAT: {
                    expo_ = (double _Complex) expo->value.d;
                    break;
                }
                case CALC_COMPLEX: {
                    expo_ = expo->value.cd;
                    break;
                }
                default: {}
            }
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = cpow(base_, expo_)}};
        }
        if (base->type != expo->type || expo->type == CALC_FLOAT) {
            double base_;
            double expo_;
            if (base->type == CALC_INTEGER) {
                base_ = (double) base->value.ll;
            } else {
                base_ = base->value.d;
            }
            if (expo->type == CALC_INTEGER) {
                expo_ = (double) expo->value.ll;
            } else {
                expo_ = expo->value.d;
            }
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = pow(base_, expo_)}};
        } else {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = pow((double)base->value.ll, (double)expo->value.ll)}};
        }        
    }
    return err_value;
}

CalcValue sqrt_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            if (vals.value.ll < 0) {
                return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = csqrt((double _Complex) vals.value.ll)}};
            }
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = sqrt((double)vals.value.ll)}};;
        }
        case CALC_FLOAT: {
            if (vals.value.d < 0) {
                return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = csqrt((double _Complex) vals.value.d)}};
            }
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = sqrt(vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = csqrt(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(sqrt, vals)
        }
        default: {
            printf("%s alled with values of unknown type\n", "sqrt");
            return err_value;
        }
    }
}

TEMPLATE_CAST_INT_NOCOMPLEX(cbrt)
TEMPLATE_CAST_INT(sin)
TEMPLATE_CAST_INT(cos)
TEMPLATE_CAST_INT(tan)
TEMPLATE_CAST_INT(asin)
TEMPLATE_CAST_INT(acos)
TEMPLATE_CAST_INT(atan)

CalcValue atan2_(CalcValue vals) {
    if (!IS_ARRAY(vals)) {
        printf("%s called without array argument\n", "atan2");
        return err_value;
    }
    CHECK_NARG("atan2", vals.value.arr.length, BINARY_NARG)
    CalcValue * dividend = vals.value.arr.values;
    CalcValue * divisor = vals.value.arr.values + 1;           
    if (IS_ARRAY(*dividend) || IS_ARRAY(*divisor)) {
        size_t length_divid = ARRAY_SIZE(*dividend);
        size_t length_divis = ARRAY_SIZE(*divisor);
        if (length_divis > 1 && length_divid > 1 && length_divis != length_divid) {
            printf("array mismatch in call to %s: %zu != %zu\n", "atan2", length_divid, length_divis);
            return err_value;
        }
        size_t length = (length_divid > length_divis ? length_divid : length_divis);
        CalcValue result = {.type = CALC_ARRAY, .value = {.arr = {.values = NULL, .length = length}}};
        CalcValue * values = malloc(length * sizeof(*values));
        if (!values) {
            printf("%s malloc failure\n", "atan2");
            return err_value;
        }
        CalcValue args[2];
        CalcValue input_args = {.type = CALC_ARRAY, .value = {.arr = {.values = &args[0], .length = BINARY_NARG}}};
        if (length_divid == 1) {
            args[0] = *dividend;
            for (size_t i = 0; i < length; i++) {
                args[1] = divisor->value.arr.values[i];
                values[i] = atan2_(input_args);
            }
        } else if (length_divis == 1) {
            args[1] = *divisor;
            for (size_t i = 0; i < length; i++) {
                args[0] = dividend->value.arr.values[i];
                values[i] = atan2_(input_args);
            }
        } else {
            for (size_t i = 0; i < length; i++) {
                args[0] = dividend->value.arr.values[i];
                args[1] = divisor->value.arr.values[i];
                values[i] = atan2_(input_args);
            }
        }
        result.value.arr.values = values;
        return result;
    } else {
        if (!IS_REAL_SCALAR(*dividend) || !IS_REAL_SCALAR(*divisor)) {
            printf("%s does not accept complex arguments\n", "atan2");
            return err_value;
        }
        if (dividend->type != divisor->type || divisor->type == CALC_FLOAT) {
            double divid;
            double divis;
            if (dividend->type == CALC_INTEGER) {
                divid = (double) dividend->value.ll;
            } else {
                divid = dividend->value.d;
            }
            if (divisor->type == CALC_INTEGER) {
                divis = (double) divisor->value.ll;
            } else {
                divis = divisor->value.d;
            }
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = atan2(divid, divis)}};
        } else {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = atan2((double)dividend->value.ll, (double)divisor->value.ll)}};
        }        
    }
    return err_value;
}

TEMPLATE_CAST_INT(sinh)
TEMPLATE_CAST_INT(cosh)
TEMPLATE_CAST_INT(tanh)
TEMPLATE_CAST_INT(asinh)
TEMPLATE_CAST_INT(acosh)
TEMPLATE_CAST_INT(atanh)

TEMPLATE_CAST_INT_NOCOMPLEX(erf)
TEMPLATE_CAST_INT_NOCOMPLEX(erfc)
TEMPLATE_CAST_INT_NOCOMPLEX(tgamma)
TEMPLATE_CAST_INT_NOCOMPLEX(ceil)
TEMPLATE_CAST_INT_NOCOMPLEX(floor)
TEMPLATE_CAST_INT_NOCOMPLEX(round)

CalcValue creal_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = creal((double _Complex)vals.value.ll)}};
        }
        case CALC_FLOAT: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = creal((double _Complex)vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = creal(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(creal, vals)
        }
        default: {
            printf("%s called with values of unknown type\n", "creal");
            return err_value;
        }
    }
}

CalcValue cimag_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = cimag((double _Complex)vals.value.ll)}};
        }
        case CALC_FLOAT: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = cimag((double _Complex)vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = cimag(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(cimag, vals)
        }
        default: {
            printf("%s called with values of unknown type\n", "cimag");
            return err_value;
        }
    }
}

CalcValue carg_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = carg((double _Complex)vals.value.ll)}};
        }
        case CALC_FLOAT: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = carg((double _Complex)vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_FLOAT, .value = {.d = carg(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(carg, vals)
        }
        default: {
            printf("%s called with values of unknown type\n", "carg");
            return err_value;
        }
    }
}

CalcValue conj_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = conj((double _Complex)vals.value.ll)}};
        }
        case CALC_FLOAT: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = conj((double _Complex)vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = conj(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(conj, vals)
        }
        default: {
            printf("%s called with values of unknown type\n", "conj");
            return err_value;
        }
    }
}

CalcValue cproj_(CalcValue vals) {
    switch (vals.type) {
        case CALC_INTEGER: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = cproj((double _Complex)vals.value.ll)}};
        }
        case CALC_FLOAT: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = cproj((double _Complex)vals.value.d)}};
        }
        case CALC_COMPLEX: {
            return (CalcValue) {.type = CALC_COMPLEX, .value = {.cd = cproj(vals.value.cd)}};
        }
        case CALC_ARRAY: {
            EVAL_UNARY_ARRAY(cproj, vals)
        }
        default: {
            printf("%s called with values of unknown type\n", "cproj");
            return err_value;
        }
    }
}

#define KEY_TYPE REPLFunctionKey
#define VALUE_TYPE REPLFunction
#define KEY_COMP REPLFunctionKey_comp
#define HASH_FUNC REPLFunctionKey_hash
#include <peggy/hash_map.h>

HASH_MAP(REPLFunctionKey, REPLFunction) func_map = {0};

#define REPLMAP_INSERT(func_map, func) \
(func_map)._class->set(&(func_map), (REPLFunctionKey){.str = STRINGIFY(func), .len = strlen(STRINGIFY(func))}, CAT(func, _))

void math_repl_init(void) {
    HASH_MAP_INIT(REPLFunctionKey, REPLFunction)(&func_map, next_prime(74));
    
    REPLMAP_INSERT(func_map, abs);
    REPLMAP_INSERT(func_map, mod);
    REPLMAP_INSERT(func_map, max);
    REPLMAP_INSERT(func_map, min);
    REPLMAP_INSERT(func_map, exp);
    REPLMAP_INSERT(func_map, exp2);
    REPLMAP_INSERT(func_map, log);
    REPLMAP_INSERT(func_map, log10);
    REPLMAP_INSERT(func_map, log2);
    REPLMAP_INSERT(func_map, pow);
    REPLMAP_INSERT(func_map, sqrt);
    REPLMAP_INSERT(func_map, cbrt);
    REPLMAP_INSERT(func_map, sin);
    REPLMAP_INSERT(func_map, cos);
    REPLMAP_INSERT(func_map, tan);
    REPLMAP_INSERT(func_map, asin);
    REPLMAP_INSERT(func_map, acos);
    REPLMAP_INSERT(func_map, atan);
    REPLMAP_INSERT(func_map, atan2);
    REPLMAP_INSERT(func_map, sinh);
    REPLMAP_INSERT(func_map, cosh);
    REPLMAP_INSERT(func_map, tanh);
    REPLMAP_INSERT(func_map, asinh);
    REPLMAP_INSERT(func_map, acosh);
    REPLMAP_INSERT(func_map, atanh);
    REPLMAP_INSERT(func_map, erf);
    REPLMAP_INSERT(func_map, erfc);
    REPLMAP_INSERT(func_map, tgamma);
    REPLMAP_INSERT(func_map, ceil);
    REPLMAP_INSERT(func_map, floor);
    REPLMAP_INSERT(func_map, round);
    REPLMAP_INSERT(func_map, creal);
    REPLMAP_INSERT(func_map, cimag);
    REPLMAP_INSERT(func_map, carg);
    REPLMAP_INSERT(func_map, conj);
    REPLMAP_INSERT(func_map, cproj);
    REPLMAP_INSERT(func_map, err_func);
}

void math_repl_dest(void) {
    if (func_map._class) {
        func_map._class->dest(&func_map);
    }
}

int print_function(void * data, REPLFunctionKey key, REPLFunction value) {
    printf("\t%.*s\n", (int)key.len, key.str);
    return 0;
}

void print_functions(void) {
    if (!func_map.capacity) {
        math_repl_init();
    }
    printf("Functions availables:\n");
    func_map._class->for_each(&func_map, print_function, NULL);
}

REPLFunction math_repl_get_func(char const * name, unsigned char length) {
    if (!func_map.capacity) {
        math_repl_init();
    }
    REPLFunction result;
    hash_map_err status = HM_SUCCESS;
    if ((status = func_map._class->get(&func_map, (REPLFunctionKey) {.str = name, .len = length}, &result))) {
        printf("unknown function called with name %.*s\n", (int)length, name);
        return err_func_;
    }
    return result;
}
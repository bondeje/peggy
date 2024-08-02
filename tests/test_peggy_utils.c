#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "test_utils.h"
#include "test_peggy_utils.h"

int check_tokens(Token * cur, size_t ntokens, char const ** result_tokens, char const * file, char const * func, size_t line) {
    int nerrors = 0;
    size_t i = 0;
    while (result_tokens[i] && cur && cur->length) {
        size_t result_length = strlen(result_tokens[i]);
        nerrors += check(cur->length == result_length, "%s-%s-%zu: failed to tokenize at token %zu. expected: %zu, matched: %zu\n", file, func, line, i, result_length, cur->length);
        nerrors += check(!strncmp(result_tokens[i], cur->string, cur->length), "%s-%s-%zu: token-character mismatch: expected %s, found %.*s\n", file, func, line, result_tokens[i], (int)cur->length, cur->string);
        cur = cur->next;
        i++;
    }
    nerrors += check(ntokens == i, "%s-%s-%zu: failed to find all the tokens in the target string: expected %zu, found %zu\n", file, func, line, i, ntokens);
    return 0;
}

int check_ASTNodes(ASTNode * uut, TestASTNode * ref, char const * file, char const * func, size_t line) {
    int nerrors = 0;
    nerrors += check(uut->rule->id == ref->rule_id, "%s-%s-%zu: rules do not match in node. expected: %d, found: %d\n", file, func, line, uut->rule->id, ref->rule_id);
    nerrors += check(!strncmp(uut->token_start->string, ref->start_str, strlen(ref->start_str)), "%s-%s-%zu: start strings do not match in node. expected: %s, found: %.*s\n", file, func, line, ref->start_str, (int)uut->token_start->length, uut->token_start->string);
    nerrors += check(!strncmp(uut->token_end->string, ref->end_str, strlen(ref->end_str)), "%s-%s-%zu: end strings do not match in node. expected: %s, found: %.*s\n", file, func, line, ref->end_str, (int)uut->token_end->length, uut->token_end->string);
    nerrors += check(uut->nchildren == ref->nchildren, "%s-%s-%zu: nchildren do not match in node. expected: %d, found: %d\n", file, func, line, uut->nchildren, ref->nchildren);
    ASTNode * child = uut->child;
    TestASTNode * test_child = ref->child;
    for (size_t i = 0; i < ref->nchildren; i++) {
        assert(test_child != NULL || !printf("TestASTNode malformed with null child %zu in node with rule: %d, nchildren: %zu, start_str: %s\n", i, ref->rule_id, ref->nchildren, ref->start_str));
        int valid = check(child != NULL, "%s-%s-%zu: invalid ASTNode in child %zu in node with rule: %d, nchildren: %zu, start_str: %.*s\n", i, uut->rule->id, uut->nchildren, (int)uut->token_start->length, uut->token_start->string);
        if (!valid) {
            nerrors += check_ASTNodes(child, test_child, file, func, line);
            child = child->next;
            test_child = test_child->next;
        } else {
            nerrors += valid;
            break;
        }
    }
    return nerrors;
}
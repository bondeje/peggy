#include <stddef.h>

#include <peggy/token.h>
#include <peggy/rule.h>
#include <peggy/astnode.h>

int check_tokens(Token * cur, size_t ntokens, char const ** result_tokens, char const * file, char const * func, size_t line);

typedef struct TestASTNode TestASTNode;

struct TestASTNode {
    rule_id_type rule_id;
    size_t nchildren;
    char const * start_str;
    char const * end_str;
    TestASTNode * child;
    TestASTNode * next;
};

#define NULLNODE ((TestASTNode *)NULL)

#define TESTASTNODE(id, start_str_, end_str_, nchildren_, child_, next_) \
    (TestASTNode){ \
        .rule_id = id,  \
        .start_str = start_str_,  \
        .end_str = end_str_, \
        .nchildren = nchildren_, \
        .child = child_, \
        .next = next_ \
    }

int check_ASTNodes(ASTNode * uut, TestASTNode * ref, char const * file, char const * func, size_t line);


#include <stdio.h>
#include "csv.h"

ASTNode * handle_csv(Parser * parser, ASTNode * node) {
    node = NULL;
    parser = NULL;
    return &ASTNode_fail;
}

int main(void) {
    printf("built!\n");
    return 0;
}
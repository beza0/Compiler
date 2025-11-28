#include <stdio.h>
#include "ast.h"

extern int yylineno;
extern int yyparse();
extern ASTNode *root;

int main() {
    printf("MiniLang Compiler\n");

    if (yyparse() == 0) {
        printf("\n=== AST ===\n");
        ast_print(root, 0);
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "sem/symbol.h"
#include "sem/sema.h"
#include "cg/codegen.h"
#include "cg/vm.h"

extern int yyparse(void);
extern FILE *yyin;
extern ASTNode *root;

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s program.minilang\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("fopen");
        return 1;
    }

    if (yyparse() != 0) {
        fprintf(stderr, "Parse failed.\n");
        fclose(yyin);
        return 1;
    }
    fclose(yyin);

    if (!root) {
        fprintf(stderr, "No AST produced.\n");
        return 1;
    }

    // (opsiyonel) AST debug:
    // ast_print(root, 0);

    SymbolTable* st = symtab_create();
    SemaResult sr = sema_analyze(root, st);

    if (sr.error_count > 0) {
        fprintf(stderr, "Compilation failed: %d semantic error(s)\n", sr.error_count);
        symtab_free(st);
        return 1;
    }

    Bytecode* bc = gen_program(root);
    int rc = vm_run(bc);

    bc_free(bc);
    symtab_free(st);
    return rc;
}



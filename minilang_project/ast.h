#ifndef AST_H
#define AST_H

typedef enum {
    NODE_INT_LITERAL,
    NODE_FLOAT_LITERAL,
    NODE_STRING_LITERAL,
    NODE_IDENT,
    NODE_BIN_OP,
    NODE_UNARY_OP,
    NODE_ASSIGN,
    NODE_VAR_DECL,
    NODE_PRINT,
    NODE_IF,
    NODE_WHILE,
    NODE_FOR,
    NODE_BLOCK,
    NODE_STMT_LIST,
    NODE_PROGRAM
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;

    const char *op;    // operator sembolü veya tip ismi (int/float/string)
    const char *name;  // identifier ismi

    int ival;          // integer değerler
    double fval;       // float değerler
    const char *sval;  // string literal

    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third; // if-else, for vs. ek çocuklar için
    struct ASTNode *body;  // for/while gövdesi için
    struct ASTNode *next;  // stmt listelerinde bir sonraki statement
} ASTNode;

ASTNode *ast_int(int v);
ASTNode *ast_float(double v);
ASTNode *ast_string(const char *s);
ASTNode *ast_ident(const char *s);
ASTNode *ast_binop(const char *op, ASTNode *l, ASTNode *r);
ASTNode *ast_unary(const char *op, ASTNode *n);
ASTNode *ast_assign(ASTNode *id, ASTNode *expr);
ASTNode *ast_vardecl(const char *type, const char *name, ASTNode *init);
ASTNode *ast_print_stmt(ASTNode *expr);
ASTNode *ast_if(ASTNode *cond, ASTNode *thenBranch, ASTNode *elseBranch);
ASTNode *ast_while(ASTNode *cond, ASTNode *body);
ASTNode *ast_for(ASTNode *init, ASTNode *cond, ASTNode *update, ASTNode *body);
ASTNode *ast_block(ASTNode *stmts);
ASTNode *ast_stmtlist(ASTNode *list, ASTNode *stmt);
ASTNode *ast_program(ASTNode *stmts);

void ast_print(ASTNode *n, int indent);

#endif

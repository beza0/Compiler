#ifndef AST_H
#define AST_H

// Project 3 eklentileri:
struct Type;
struct Symbol;

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
    NODE_PROGRAM,

    // ast_print içinde var ama pratikte list "next" ile:
    NODE_STMT_LIST
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;

    // line number: parser doldurabiliyorsa kullanacağız
    int line;

    // ===== Project 3 alanları =====
    struct Type* node_type;   // semantic sonucu
    struct Symbol* sym;       // symbol table entry (slot + type)

    // ===== Project 2 alanları (ast.c’de kullandıkların) =====
    int ival;
    double fval;
    char *sval;
    char *name;   // ident
    char *op;     // binop/unaryop veya vardecl type string

    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third;
    struct ASTNode *body;

    struct ASTNode *next; // stmt list chain
} ASTNode;

// constructors (ast.c’de olanlar)
ASTNode *ast_int(int v);
ASTNode *ast_float(double v);
ASTNode *ast_string(const char *s);
ASTNode *ast_ident(const char *s);
ASTNode *ast_binop(const char *op, ASTNode *l, ASTNode *r);
ASTNode *ast_unary(const char *op, ASTNode *n1);
ASTNode *ast_assign(ASTNode *id, ASTNode *expr);
ASTNode *ast_vardecl(const char *type, const char *name, ASTNode *init);
ASTNode *ast_print_stmt(ASTNode *expr);
ASTNode *ast_if(ASTNode *cond, ASTNode *thenBranch, ASTNode *elseBranch);
ASTNode *ast_while(ASTNode *cond, ASTNode *body);
ASTNode *ast_for(ASTNode *init, ASTNode *cond, ASTNode *update, ASTNode *body);
ASTNode *ast_block(ASTNode *stmts);
ASTNode *ast_stmtlist(ASTNode *list, ASTNode *stmt);
ASTNode *ast_program(ASTNode *stmts);

void ast_print(ASTNode *n, int ind);

#endif

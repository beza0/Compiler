#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static char *dupstr(const char *s) {
    if (!s) return NULL;
    char *p = malloc(strlen(s) + 1);
    if (!p) {
        perror("malloc");
        exit(1);
    }
    strcpy(p, s);
    return p;
}

static ASTNode *new_node(NodeKind k) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    if (!n) {
        perror("calloc");
        exit(1);
    }
    n->kind = k;
    return n;
}

ASTNode *ast_int(int v) {
    ASTNode *n = new_node(NODE_INT_LITERAL);
    n->ival = v;
    return n;
}

ASTNode *ast_float(double v) {
    ASTNode *n = new_node(NODE_FLOAT_LITERAL);
    n->fval = v;
    return n;
}

ASTNode *ast_string(const char *s) {
    ASTNode *n = new_node(NODE_STRING_LITERAL);
    n->sval = dupstr(s);
    return n;
}

ASTNode *ast_ident(const char *s) {
    ASTNode *n = new_node(NODE_IDENT);
    n->name = dupstr(s);
    return n;
}

ASTNode *ast_binop(const char *op, ASTNode *l, ASTNode *r) {
    ASTNode *n = new_node(NODE_BIN_OP);
    n->op = dupstr(op);
    n->left = l;
    n->right = r;
    return n;
}

ASTNode *ast_unary(const char *op, ASTNode *n1) {
    ASTNode *n = new_node(NODE_UNARY_OP);
    n->op = dupstr(op);
    n->left = n1;
    return n;
}

ASTNode *ast_assign(ASTNode *id, ASTNode *expr) {
    ASTNode *n = new_node(NODE_ASSIGN);
    n->left = id;
    n->right = expr;
    return n;
}

ASTNode *ast_vardecl(const char *type, const char *name, ASTNode *init) {
    ASTNode *n = new_node(NODE_VAR_DECL);
    n->op = dupstr(type);     // type: "int", "float", "string"
    n->name = dupstr(name);   // variable name
    n->left = init;           // initializer expression (NULL olabilir)
    return n;
}

ASTNode *ast_print_stmt(ASTNode *expr) {
    ASTNode *n = new_node(NODE_PRINT);
    n->left = expr;
    return n;
}

ASTNode *ast_if(ASTNode *cond, ASTNode *thenBranch, ASTNode *elseBranch) {
    ASTNode *n = new_node(NODE_IF);
    n->left  = cond;
    n->right = thenBranch;
    n->third = elseBranch;
    return n;
}

ASTNode *ast_while(ASTNode *cond, ASTNode *body) {
    ASTNode *n = new_node(NODE_WHILE);
    n->left  = cond;
    n->right = body;
    return n;
}

ASTNode *ast_for(ASTNode *init, ASTNode *cond, ASTNode *update, ASTNode *body) {
    ASTNode *n = new_node(NODE_FOR);
    n->left  = init;
    n->right = cond;
    n->third = update;
    n->body  = body;
    return n;
}

ASTNode *ast_block(ASTNode *stmts) {
    ASTNode *n = new_node(NODE_BLOCK);
    n->left = stmts;
    return n;
}

ASTNode *ast_stmtlist(ASTNode *list, ASTNode *stmt) {
    if (!list) return stmt;
    ASTNode *p = list;
    while (p->next) p = p->next;
    p->next = stmt;
    return list;
}

ASTNode *ast_program(ASTNode *stmts) {
    ASTNode *n = new_node(NODE_PROGRAM);
    n->left = stmts;
    return n;
}

static void indent(int i) {
    while (i-- > 0) printf("  ");
}

void ast_print(ASTNode *n, int ind) {
    if (!n) return;

    indent(ind);
    switch (n->kind) {
    case NODE_PROGRAM:
        printf("PROGRAM\n");
        ast_print(n->left, ind+1);
        break;
    case NODE_STMT_LIST:
        printf("STMT_LIST\n");
        ast_print(n->left, ind+1);
        break;
    case NODE_BLOCK:
        printf("BLOCK\n");
        ast_print(n->left, ind+1);
        break;
    case NODE_VAR_DECL:
        printf("VAR_DECL %s %s\n",
               n->op  ? n->op  : "?",
               n->name? n->name: "?");
        if (n->left) {
            indent(ind+1);
            printf("INIT:\n");
            ast_print(n->left, ind+2);
        }
        break;
    case NODE_ASSIGN:
        printf("ASSIGN\n");
        ast_print(n->left, ind+1);
        ast_print(n->right, ind+1);
        break;
    case NODE_PRINT:
        printf("PRINT\n");
        ast_print(n->left, ind+1);
        break;
    case NODE_IF:
        printf("IF\n");
        indent(ind+1); printf("COND:\n");
        ast_print(n->left, ind+2);
        indent(ind+1); printf("THEN:\n");
        ast_print(n->right, ind+2);
        if (n->third) {
            indent(ind+1); printf("ELSE:\n");
            ast_print(n->third, ind+2);
        }
        break;
    case NODE_WHILE:
        printf("WHILE\n");
        indent(ind+1); printf("COND:\n");
        ast_print(n->left, ind+2);
        indent(ind+1); printf("BODY:\n");
        ast_print(n->right, ind+2);
        break;
    case NODE_FOR:
        printf("FOR\n");
        if (n->left) {
            indent(ind+1); printf("INIT:\n");
            ast_print(n->left, ind+2);
        }
        if (n->right) {
            indent(ind+1); printf("COND:\n");
            ast_print(n->right, ind+2);
        }
        if (n->third) {
            indent(ind+1); printf("UPDATE:\n");
            ast_print(n->third, ind+2);
        }
        if (n->body) {
            indent(ind+1); printf("BODY:\n");
            ast_print(n->body, ind+2);
        }
        break;
    case NODE_IDENT:
        printf("IDENT %s\n", n->name ? n->name : "?");
        break;
    case NODE_INT_LITERAL:
        printf("INT %d\n", n->ival);
        break;
    case NODE_FLOAT_LITERAL:
        printf("FLOAT %f\n", n->fval);
        break;
    case NODE_STRING_LITERAL:
        printf("STRING \"%s\"\n", n->sval ? n->sval : "");
        break;
    case NODE_BIN_OP:
        printf("BIN_OP %s\n", n->op ? n->op : "?");
        ast_print(n->left, ind+1);
        ast_print(n->right, ind+1);
        break;
    case NODE_UNARY_OP:
        printf("UNARY_OP %s\n", n->op ? n->op : "?");
        ast_print(n->left, ind+1);
        break;
    }

    if (n->next)
        ast_print(n->next, ind);
}

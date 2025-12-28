#include "sema.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

static void sem_error(SemaResult* r, int line, const char* msg){
    fprintf(stderr, "[Semantic Error] line %d: %s\n", line, msg);
    r->error_count++;
}

static Type* type_from_string(const char* s){
    if(!s) return &TYPE_ERROR;
    if(strcmp(s,"int")==0) return &TYPE_INT;
    if(strcmp(s,"float")==0) return &TYPE_FLOAT;
    if(strcmp(s,"string")==0) return &TYPE_STRING;
    if(strcmp(s,"bool")==0) return &TYPE_BOOL;
    if(strcmp(s,"void")==0) return &TYPE_VOID;
    return &TYPE_ERROR;
}

static Type* sem_expr(ASTNode* n, SymbolTable* st, SemaResult* r);

static void sem_stmt(ASTNode* n, SymbolTable* st, SemaResult* r){
    if(!n) return;

    switch(n->kind){
        case NODE_BLOCK: {
            symtab_push(st);
            for(ASTNode* s = n->left; s; s = s->next) sem_stmt(s, st, r);
            symtab_pop(st);
        } break;

        case NODE_VAR_DECL: {
            Type* decl_t = type_from_string(n->op);
            if(decl_t == &TYPE_ERROR){
                sem_error(r, n->line, "Unknown variable type in declaration");
                n->node_type = &TYPE_ERROR;
                break;
            }

            Symbol* sym = symtab_insert(st, n->name, decl_t, n->line);
            if(!sym){
                sem_error(r, n->line, "Redeclaration in same scope");
            } else {
                n->sym = sym;
            }

            if(n->left){
                Type* init_t = sem_expr(n->left, st, r);
                if(!can_assign(decl_t, init_t)){
                    sem_error(r, n->line, "Type mismatch in variable initializer");
                }
            }
            n->node_type = &TYPE_VOID;
        } break;

        case NODE_ASSIGN: {
            // left taraf ident olmalı
            if(!n->left || n->left->kind != NODE_IDENT){
                sem_error(r, n->line, "Left-hand side of assignment must be an identifier");
                break;
            }

            Symbol* sym = symtab_lookup(st, n->left->name);
            if(!sym){
                sem_error(r, n->line, "Undeclared variable in assignment");
                n->node_type = &TYPE_ERROR;
                break;
            }
            n->left->sym = sym;

            Type* rhs_t = sem_expr(n->right, st, r);
            if(!can_assign(sym->type, rhs_t)){
                sem_error(r, n->line, "Type mismatch in assignment");
            }
            n->node_type = &TYPE_VOID;
        } break;

        case NODE_PRINT: {
            Type* t = sem_expr(n->left, st, r);
            (void)t;
            n->node_type = &TYPE_VOID;
        } break;

        case NODE_IF: {
            Type* ct = sem_expr(n->left, st, r);
            if(ct->kind != TY_BOOL && ct->kind != TY_ERROR){
                sem_error(r, n->line, "If condition must be bool");
            }
            sem_stmt(n->right, st, r);
            if(n->third) sem_stmt(n->third, st, r);
            n->node_type = &TYPE_VOID;
        } break;

        case NODE_WHILE: {
            Type* ct = sem_expr(n->left, st, r);
            if(ct->kind != TY_BOOL && ct->kind != TY_ERROR){
                sem_error(r, n->line, "While condition must be bool");
            }
            sem_stmt(n->right, st, r);
            n->node_type = &TYPE_VOID;
        } break;

        case NODE_FOR: {
            symtab_push(st);
            if(n->left) sem_stmt(n->left, st, r); // init is stmt (decl/assign)
            if(n->right){
                Type* ct = sem_expr(n->right, st, r);
                if(ct->kind != TY_BOOL && ct->kind != TY_ERROR){
                    sem_error(r, n->line, "For condition must be bool");
                }
            }
            if(n->third) sem_stmt(n->third, st, r); // update stmt (assign)
            if(n->body) sem_stmt(n->body, st, r);
            symtab_pop(st);
            n->node_type = &TYPE_VOID;
        } break;

        case NODE_PROGRAM: {
            for(ASTNode* s = n->left; s; s = s->next) sem_stmt(s, st, r);
            n->node_type = &TYPE_VOID;
        } break;

        default:
            // expr node gelirse ignore
            break;
    }
}

static Type* sem_expr(ASTNode* n, SymbolTable* st, SemaResult* r){
    if(!n) return &TYPE_ERROR;

    switch(n->kind){
        case NODE_INT_LITERAL:
            n->node_type = &TYPE_INT; return n->node_type;

        case NODE_FLOAT_LITERAL:
            n->node_type = &TYPE_FLOAT; return n->node_type;

        case NODE_STRING_LITERAL:
            n->node_type = &TYPE_STRING; return n->node_type;

        case NODE_IDENT: {
            Symbol* sym = symtab_lookup(st, n->name);
            if(!sym){
                sem_error(r, n->line, "Undeclared variable");
                n->node_type = &TYPE_ERROR;
                return n->node_type;
            }
            n->sym = sym;
            n->node_type = sym->type;
            return n->node_type;
        }

        case NODE_UNARY_OP: {
            Type* t = sem_expr(n->left, st, r);
            if(n->op && strcmp(n->op, "-")==0){
                if(t->kind != TY_INT && t->kind != TY_FLOAT && t->kind != TY_ERROR){
                    sem_error(r, n->line, "Unary '-' requires int or float");
                    n->node_type = &TYPE_ERROR;
                } else {
                    n->node_type = t;
                }
                return n->node_type;
            }
            if(n->op && strcmp(n->op, "!")==0){
                if(t->kind != TY_BOOL && t->kind != TY_ERROR){
                    sem_error(r, n->line, "Unary '!' requires bool");
                    n->node_type = &TYPE_ERROR;
                } else {
                    n->node_type = &TYPE_BOOL;
                }
                return n->node_type;
            }
            sem_error(r, n->line, "Unknown unary operator");
            n->node_type = &TYPE_ERROR;
            return n->node_type;
        }

        case NODE_BIN_OP: {
            Type* lt = sem_expr(n->left, st, r);
            Type* rt = sem_expr(n->right, st, r);

            if(!n->op){
                sem_error(r, n->line, "Missing binary operator");
                n->node_type = &TYPE_ERROR;
                return n->node_type;
            }

            // arithmetic
            if(strcmp(n->op,"+")==0 || strcmp(n->op,"-")==0 ||
               strcmp(n->op,"*")==0 || strcmp(n->op,"/")==0){
                Type* res = arith_result(lt, rt);
                if(res == &TYPE_ERROR){
                    sem_error(r, n->line, "Arithmetic operator requires int/float");
                }
                n->node_type = res;
                return n->node_type;
            }

            // comparisons -> bool
            if(strcmp(n->op,"==")==0 || strcmp(n->op,"<")==0 || strcmp(n->op,">")==0){
                // int/float compare allowed, string only == allowed (istersen)
                if(lt->kind == TY_STRING || rt->kind == TY_STRING){
                    if(strcmp(n->op,"==")!=0){
                        sem_error(r, n->line, "Only '==' allowed for strings");
                        n->node_type = &TYPE_ERROR;
                        return n->node_type;
                    }
                }
                n->node_type = &TYPE_BOOL;
                return n->node_type;
            }

            // logical
            if(strcmp(n->op,"&&")==0 || strcmp(n->op,"||")==0){
                if(lt->kind != TY_BOOL && lt->kind != TY_ERROR) sem_error(r, n->line, "Left operand must be bool");
                if(rt->kind != TY_BOOL && rt->kind != TY_ERROR) sem_error(r, n->line, "Right operand must be bool");
                n->node_type = &TYPE_BOOL;
                return n->node_type;
            }

            sem_error(r, n->line, "Unknown binary operator");
            n->node_type = &TYPE_ERROR;
            return n->node_type;
        }

        default:
            n->node_type = &TYPE_ERROR;
            return n->node_type;
    }
}

SemaResult sema_analyze(ASTNode* root, SymbolTable* st){
    SemaResult r = {0};
    sem_stmt(root, st, &r);
    return r;
}


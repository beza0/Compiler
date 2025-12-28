#include "codegen.h"
#include "../sem/symbol.h"
#include <string.h>

static void gen_stmt(ASTNode* n, Bytecode* bc);
static void gen_expr(ASTNode* n, Bytecode* bc);

static void gen_expr(ASTNode* n, Bytecode* bc){
    if(!n) return;

    switch(n->kind){
        case NODE_INT_LITERAL:
            bc_emit(bc, (Instr){.op=BC_PUSH_INT, .a=n->ival});
            break;

        case NODE_FLOAT_LITERAL:
            bc_emit(bc, (Instr){.op=BC_PUSH_FLOAT, .f=n->fval});
            break;

        case NODE_STRING_LITERAL:
            bc_emit(bc, (Instr){.op=BC_PUSH_STR, .s=n->sval});
            break;

        case NODE_IDENT:
            // semantic pass n->sym dolduruyor
            bc_emit(bc, (Instr){.op=BC_LOAD, .a=n->sym ? n->sym->slot : 0});
            break;

        case NODE_UNARY_OP:
            gen_expr(n->left, bc);
            if(n->op && strcmp(n->op, "-")==0) bc_emit(bc, (Instr){.op=BC_NEG});
            else if(n->op && strcmp(n->op, "!")==0) bc_emit(bc, (Instr){.op=BC_NOT});
            break;

        case NODE_BIN_OP:
            gen_expr(n->left, bc);
            gen_expr(n->right, bc);

            if(!n->op) break;

            if(strcmp(n->op, "+")==0) bc_emit(bc, (Instr){.op=BC_ADD});
            else if(strcmp(n->op, "-")==0) bc_emit(bc, (Instr){.op=BC_SUB});
            else if(strcmp(n->op, "*")==0) bc_emit(bc, (Instr){.op=BC_MUL});
            else if(strcmp(n->op, "/")==0) bc_emit(bc, (Instr){.op=BC_DIV});
            else if(strcmp(n->op, "==")==0) bc_emit(bc, (Instr){.op=BC_EQ});
            else if(strcmp(n->op, "<")==0) bc_emit(bc, (Instr){.op=BC_LT});
            else if(strcmp(n->op, ">")==0) bc_emit(bc, (Instr){.op=BC_GT});
            else if(strcmp(n->op, "&&")==0) bc_emit(bc, (Instr){.op=BC_AND});
            else if(strcmp(n->op, "||")==0) bc_emit(bc, (Instr){.op=BC_OR});
            break;

        default:
            break;
    }
}

static void gen_stmt(ASTNode* n, Bytecode* bc){
    if(!n) return;

    switch(n->kind){
        case NODE_PROGRAM:
            for(ASTNode* s=n->left; s; s=s->next) gen_stmt(s, bc);
            break;

        case NODE_BLOCK:
            for(ASTNode* s=n->left; s; s=s->next) gen_stmt(s, bc);
            break;

        case NODE_VAR_DECL:
            // init varsa evaluate + store
            if(n->left){
                gen_expr(n->left, bc);
                bc_emit(bc, (Instr){.op=BC_STORE, .a=n->sym ? n->sym->slot : 0});
            }
            break;

        case NODE_ASSIGN:
            gen_expr(n->right, bc);
            // left ident slot
            bc_emit(bc, (Instr){.op=BC_STORE, .a=n->left && n->left->sym ? n->left->sym->slot : 0});
            break;

        case NODE_PRINT:
            gen_expr(n->left, bc);
            bc_emit(bc, (Instr){.op=BC_PRINT});
            break;

        case NODE_IF: {
            gen_expr(n->left, bc);
            int jmpf = bc_emit(bc, (Instr){.op=BC_JMPF, .a=-1});
            gen_stmt(n->right, bc);

            if(n->third){
                int jmp = bc_emit(bc, (Instr){.op=BC_JMP, .a=-1});
                bc_patch_a(bc, jmpf, bc->count);
                gen_stmt(n->third, bc);
                bc_patch_a(bc, jmp, bc->count);
            } else {
                bc_patch_a(bc, jmpf, bc->count);
            }
        } break;

        case NODE_WHILE: {
            int start = bc->count;
            gen_expr(n->left, bc);
            int jmpf = bc_emit(bc, (Instr){.op=BC_JMPF, .a=-1});
            gen_stmt(n->right, bc);
            bc_emit(bc, (Instr){.op=BC_JMP, .a=start});
            bc_patch_a(bc, jmpf, bc->count);
        } break;

        case NODE_FOR: {
            // for(init; cond; update) body
            if(n->left) gen_stmt(n->left, bc);
            int start = bc->count;
            if(n->right){
                gen_expr(n->right, bc);
            } else {
                // cond yoksa true varsay (1)
                bc_emit(bc, (Instr){.op=BC_PUSH_INT, .a=1});
            }
            int jmpf = bc_emit(bc, (Instr){.op=BC_JMPF, .a=-1});
            if(n->body) gen_stmt(n->body, bc);
            if(n->third) gen_stmt(n->third, bc);
            bc_emit(bc, (Instr){.op=BC_JMP, .a=start});
            bc_patch_a(bc, jmpf, bc->count);
        } break;

        default:
            // expr stmt varsa ignore
            break;
    }
}

Bytecode* gen_program(ASTNode* root){
    Bytecode* bc = bc_create();
    gen_stmt(root, bc);
    bc_emit(bc, (Instr){.op=BC_HALT});
    return bc;
}


#include "bytecode.h"
#include <stdlib.h>

Bytecode* bc_create(void){
    Bytecode* bc = (Bytecode*)calloc(1, sizeof(Bytecode));
    bc->cap = 256;
    bc->code = (Instr*)calloc(bc->cap, sizeof(Instr));
    return bc;
}

void bc_free(Bytecode* bc){
    if(!bc) return;
    free(bc->code);
    free(bc);
}

int bc_emit(Bytecode* bc, Instr in){
    if(bc->count >= bc->cap){
        bc->cap *= 2;
        bc->code = (Instr*)realloc(bc->code, bc->cap * sizeof(Instr));
    }
    bc->code[bc->count] = in;
    return bc->count++;
}

void bc_patch_a(Bytecode* bc, int at, int value){
    bc->code[at].a = value;
}


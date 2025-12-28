#pragma once

typedef enum {
    BC_PUSH_INT,
    BC_PUSH_FLOAT,
    BC_PUSH_STR,

    BC_LOAD,      // a = slot
    BC_STORE,     // a = slot

    BC_ADD,
    BC_SUB,
    BC_MUL,
    BC_DIV,

    BC_EQ,
    BC_LT,
    BC_GT,

    BC_AND,
    BC_OR,
    BC_NOT,
    BC_NEG,

    BC_PRINT,
    BC_JMP,       // a = target ip
    BC_JMPF,      // a = target ip (pop bool)
    BC_HALT
} OpCode;

typedef struct {
    OpCode op;
    int a;
    double f;
    const char* s;
} Instr;

typedef struct {
    Instr* code;
    int count;
    int cap;
} Bytecode;

Bytecode* bc_create(void);
void bc_free(Bytecode* bc);
int bc_emit(Bytecode* bc, Instr in);
void bc_patch_a(Bytecode* bc, int at, int value);


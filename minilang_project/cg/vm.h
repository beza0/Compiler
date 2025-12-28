#pragma once
#include "bytecode.h"

typedef enum { V_INT, V_FLOAT, V_BOOL, V_STR } ValTag;

typedef struct {
    ValTag tag;
    union { int i; double f; int b; const char* s; } as;
} Value;

int vm_run(Bytecode* bc);


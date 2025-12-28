#pragma once
#include "../ast.h"
#include "symbol.h"

typedef struct {
    int error_count;
} SemaResult;

SemaResult sema_analyze(ASTNode* root, SymbolTable* st);


#pragma once
#include "types.h"

typedef struct Symbol {
    char* name;
    Type* type;
    int slot;            // codegen için local index
    int line_declared;
    struct Symbol* next;
} Symbol;

typedef struct Scope {
    Symbol* symbols;
    struct Scope* parent;
} Scope;

typedef struct SymbolTable {
    Scope* current;
    int next_slot; // slot counter
} SymbolTable;

SymbolTable* symtab_create(void);
void symtab_free(SymbolTable* st);

void symtab_push(SymbolTable* st);
void symtab_pop(SymbolTable* st);

Symbol* symtab_lookup(SymbolTable* st, const char* name);
Symbol* symtab_lookup_current(SymbolTable* st, const char* name);

// redeclare varsa NULL döner
Symbol* symtab_insert(SymbolTable* st, const char* name, Type* type, int line);



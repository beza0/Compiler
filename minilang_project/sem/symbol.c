#include "symbol.h"
#include <stdlib.h>
#include <string.h>

static char* xstrdup(const char* s){
    if(!s) return NULL;
    size_t n = strlen(s);
    char* r = (char*)malloc(n+1);
    memcpy(r, s, n+1);
    return r;
}

static void free_symbols(Symbol* s){
    while(s){
        Symbol* n = s->next;
        free(s->name);
        free(s);
        s = n;
    }
}

SymbolTable* symtab_create(void){
    SymbolTable* st = (SymbolTable*)calloc(1, sizeof(SymbolTable));
    st->next_slot = 0;
    symtab_push(st);
    return st;
}

void symtab_free(SymbolTable* st){
    if(!st) return;
    while(st->current) symtab_pop(st);
    free(st);
}

void symtab_push(SymbolTable* st){
    Scope* sc = (Scope*)calloc(1, sizeof(Scope));
    sc->parent = st->current;
    st->current = sc;
}

void symtab_pop(SymbolTable* st){
    if(!st || !st->current) return;
    Scope* sc = st->current;
    st->current = sc->parent;
    free_symbols(sc->symbols);
    free(sc);
}

Symbol* symtab_lookup_current(SymbolTable* st, const char* name){
    if(!st || !st->current) return NULL;
    for(Symbol* s = st->current->symbols; s; s=s->next){
        if(strcmp(s->name, name)==0) return s;
    }
    return NULL;
}

Symbol* symtab_lookup(SymbolTable* st, const char* name){
    if(!st) return NULL;
    for(Scope* sc = st->current; sc; sc=sc->parent){
        for(Symbol* s = sc->symbols; s; s=s->next){
            if(strcmp(s->name, name)==0) return s;
        }
    }
    return NULL;
}

Symbol* symtab_insert(SymbolTable* st, const char* name, Type* type, int line){
    if(!st || !st->current || !name) return NULL;
    if(symtab_lookup_current(st, name)) return NULL;

    Symbol* s = (Symbol*)calloc(1, sizeof(Symbol));
    s->name = xstrdup(name);
    s->type = type;
    s->slot = st->next_slot++;
    s->line_declared = line;
    s->next = st->current->symbols;
    st->current->symbols = s;
    return s;
}


#include "types.h"

Type TYPE_INT   = {TY_INT};
Type TYPE_FLOAT = {TY_FLOAT};
Type TYPE_STRING= {TY_STRING};
Type TYPE_BOOL  = {TY_BOOL};
Type TYPE_VOID  = {TY_VOID};
Type TYPE_ERROR = {TY_ERROR};

const char* type_name(const Type* t){
    if(!t) return "<?>";

    switch(t->kind){
        case TY_INT: return "int";
        case TY_FLOAT: return "float";
        case TY_STRING: return "string";
        case TY_BOOL: return "bool";
        case TY_VOID: return "void";
        case TY_ERROR: return "<error>";
        default: return "<?>"; 
    }
}

int type_equals(const Type* a, const Type* b){
    return a && b && a->kind == b->kind;
}

int can_assign(const Type* dst, const Type* src){
    if(!dst || !src) return 0;
    if(dst->kind == TY_ERROR || src->kind == TY_ERROR) return 1;
    if(dst->kind == src->kind) return 1;
    if(dst->kind == TY_FLOAT && src->kind == TY_INT) return 1;
    return 0;
}

Type* arith_result(const Type* a, const Type* b){
    if(!a || !b) return &TYPE_ERROR;
    if(a->kind == TY_ERROR || b->kind == TY_ERROR) return &TYPE_ERROR;
    if(a->kind == TY_STRING || b->kind == TY_STRING) return &TYPE_ERROR;
    if(a->kind == TY_FLOAT || b->kind == TY_FLOAT) return &TYPE_FLOAT;
    if(a->kind == TY_INT && b->kind == TY_INT) return &TYPE_INT;
    return &TYPE_ERROR;
}


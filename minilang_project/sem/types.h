#pragma once

typedef enum {
    TY_INT,
    TY_FLOAT,
    TY_STRING,
    TY_BOOL,
    TY_VOID,
    TY_ERROR
} TypeKind;

typedef struct Type { TypeKind kind; } Type;

extern Type TYPE_INT, TYPE_FLOAT, TYPE_STRING, TYPE_BOOL, TYPE_VOID, TYPE_ERROR;

const char* type_name(const Type* t);
int type_equals(const Type* a, const Type* b);

// assign rules: int->float allowed, others strict
int can_assign(const Type* dst, const Type* src);

// arithmetic result (int/float)
Type* arith_result(const Type* a, const Type* b);


#include "vm.h"
#include <stdio.h>
#include <string.h>
static Value stack_[2048];
static int sp_;
static Value locals_[512];

static void push(Value v){ stack_[sp_++] = v; }
static Value pop(void){ return stack_[--sp_]; }

static int as_bool(Value v){
    if(v.tag == V_BOOL) return v.as.b;
    if(v.tag == V_INT) return v.as.i != 0;
    if(v.tag == V_FLOAT) return v.as.f != 0.0;
    return 0;
}

int vm_run(Bytecode* bc){
    sp_ = 0;
    for(int i=0;i<512;i++) locals_[i].tag = V_INT, locals_[i].as.i = 0;

    int ip = 0;
    while(ip < bc->count){
        Instr in = bc->code[ip++];

        switch(in.op){
            case BC_PUSH_INT:  push((Value){.tag=V_INT, .as.i=in.a}); break;
            case BC_PUSH_FLOAT:push((Value){.tag=V_FLOAT, .as.f=in.f}); break;
            case BC_PUSH_STR:  push((Value){.tag=V_STR, .as.s=in.s}); break;

            case BC_LOAD:  push(locals_[in.a]); break;
            case BC_STORE: locals_[in.a] = pop(); break;

            case BC_NEG: {
                Value a = pop();
                if(a.tag==V_FLOAT) push((Value){.tag=V_FLOAT, .as.f=-a.as.f});
                else push((Value){.tag=V_INT, .as.i=-a.as.i});
            } break;

            case BC_NOT: {
                Value a = pop();
                push((Value){.tag=V_BOOL, .as.b=!as_bool(a)});
            } break;

            case BC_ADD:
            case BC_SUB:
            case BC_MUL:
            case BC_DIV: {
                Value b = pop(), a = pop();
                int use_float = (a.tag==V_FLOAT || b.tag==V_FLOAT);
                double af = (a.tag==V_FLOAT)? a.as.f : (double)a.as.i;
                double bf = (b.tag==V_FLOAT)? b.as.f : (double)b.as.i;

                double rf = 0;
                if(in.op==BC_ADD) rf = af + bf;
                if(in.op==BC_SUB) rf = af - bf;
                if(in.op==BC_MUL) rf = af * bf;
                if(in.op==BC_DIV) rf = af / bf;

                if(use_float) push((Value){.tag=V_FLOAT, .as.f=rf});
                else push((Value){.tag=V_INT, .as.i=(int)rf});
            } break;

            case BC_EQ: {
                Value b = pop(), a = pop();
                int eq = 0;
                if(a.tag==V_STR && b.tag==V_STR) eq = (a.as.s && b.as.s && strcmp(a.as.s,b.as.s)==0);
                else {
                    double af = (a.tag==V_FLOAT)? a.as.f : (double)a.as.i;
                    double bf = (b.tag==V_FLOAT)? b.as.f : (double)b.as.i;
                    eq = (af == bf);
                }
                push((Value){.tag=V_BOOL, .as.b=eq});
            } break;

            case BC_LT:
            case BC_GT: {
                Value b = pop(), a = pop();
                double af = (a.tag==V_FLOAT)? a.as.f : (double)a.as.i;
                double bf = (b.tag==V_FLOAT)? b.as.f : (double)b.as.i;
                int res = (in.op==BC_LT) ? (af < bf) : (af > bf);
                push((Value){.tag=V_BOOL, .as.b=res});
            } break;

            case BC_AND: {
                Value b = pop(), a = pop();
                push((Value){.tag=V_BOOL, .as.b=(as_bool(a) && as_bool(b))});
            } break;

            case BC_OR: {
                Value b = pop(), a = pop();
                push((Value){.tag=V_BOOL, .as.b=(as_bool(a) || as_bool(b))});
            } break;

            case BC_PRINT: {
                Value v = pop();
                if(v.tag==V_INT) printf("%d\n", v.as.i);
                else if(v.tag==V_FLOAT) printf("%f\n", v.as.f);
                else if(v.tag==V_BOOL) printf("%s\n", v.as.b ? "true" : "false");
                else if(v.tag==V_STR) printf("%s\n", v.as.s ? v.as.s : "");
            } break;

            case BC_JMP: ip = in.a; break;

            case BC_JMPF: {
                Value c = pop();
                if(!as_bool(c)) ip = in.a;
            } break;

            case BC_HALT:
                return 0;

            default:
                fprintf(stderr,"VM: unknown opcode %d\n", (int)in.op);
                return 1;
        }
    }
    return 0;
}


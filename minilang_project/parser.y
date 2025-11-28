%{
#include "ast.h"
#include <stdio.h>

extern int yylex();
void yyerror(const char *s);

ASTNode *root;
%}

%union {
    int ival;
    double fval;
    char *sval;
    ASTNode *node;
}

%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING
%token <sval> ID

%token IF ELSE WHILE FOR
%token KW_INT KW_FLOAT KW_STRING
%token PRINT RETURN

%token EQ NEQ GT LT GE LE
%token ASSIGN PLUS MINUS STAR SLASH
%token SEMI COMMA LPAREN RPAREN LBRACE RBRACE

%type <node> program stmtlist stmt block
%type <node> expr equality relational additive multiplicative unary primary
%type <node> opt_else for_init opt_expr opt_init

%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left STAR SLASH
%right UMINUS

%%

program:
      stmtlist                               { root = ast_program($1); $$ = $1; }
    | KW_INT ID LBRACE stmtlist RBRACE       { root = ast_program($4); $$ = $4; } /* int main { ... } */
    ;

stmtlist:
      stmt                                   { $$ = $1; }
    | stmtlist stmt                          { $$ = ast_stmtlist($1, $2); }
    ;

block:
      LBRACE stmtlist RBRACE                 { $$ = ast_block($2); }
    ;

stmt:
      KW_INT ID opt_init SEMI                { $$ = ast_vardecl("int", $2, $3); }
    | KW_FLOAT ID opt_init SEMI              { $$ = ast_vardecl("float", $2, $3); }
    | KW_STRING ID opt_init SEMI             { $$ = ast_vardecl("string", $2, $3); }
    | ID ASSIGN expr SEMI                    { $$ = ast_assign(ast_ident($1), $3); }
    | PRINT LPAREN expr RPAREN SEMI          { $$ = ast_print_stmt($3); }
    | IF LPAREN expr RPAREN block opt_else   { $$ = ast_if($3, $5, $6); }
    | WHILE LPAREN expr RPAREN block         { $$ = ast_while($3, $5); }
    | FOR LPAREN for_init SEMI opt_expr SEMI opt_expr RPAREN block
                                             { $$ = ast_for($3, $5, $7, $9); }
    | block                                  { $$ = $1; }
    ;

opt_init:
      /* empty */                            { $$ = NULL; }
    | ASSIGN expr                            { $$ = $2; }
    ;

opt_expr:
      /* empty */                            { $$ = NULL; }
    | expr                                   { $$ = $1; }
    ;

for_init:
      KW_INT ID opt_init                     { $$ = ast_vardecl("int", $2, $3); }
    | KW_FLOAT ID opt_init                   { $$ = ast_vardecl("float", $2, $3); }
    | KW_STRING ID opt_init                  { $$ = ast_vardecl("string", $2, $3); }
    | ID ASSIGN expr                         { $$ = ast_assign(ast_ident($1), $3); }
    | /* empty */                            { $$ = NULL; }
    ;

opt_else:
      /* empty */                            { $$ = NULL; }
    | ELSE block                             { $$ = $2; }
    ;

expr:
      equality                               { $$ = $1; }
    ;

equality:
      relational                             { $$ = $1; }
    | equality EQ  relational                { $$ = ast_binop("==", $1, $3); }
    | equality NEQ relational                { $$ = ast_binop("!=", $1, $3); }
    ;

relational:
      additive                               { $$ = $1; }
    | relational LT additive                 { $$ = ast_binop("<",  $1, $3); }
    | relational GT additive                 { $$ = ast_binop(">",  $1, $3); }
    | relational LE additive                 { $$ = ast_binop("<=", $1, $3); }
    | relational GE additive                 { $$ = ast_binop(">=", $1, $3); }
    ;

additive:
      multiplicative                         { $$ = $1; }
    | additive PLUS  multiplicative          { $$ = ast_binop("+", $1, $3); }
    | additive MINUS multiplicative          { $$ = ast_binop("-", $1, $3); }
    ;

multiplicative:
      unary                                  { $$ = $1; }
    | multiplicative STAR  unary             { $$ = ast_binop("*", $1, $3); }
    | multiplicative SLASH unary             { $$ = ast_binop("/", $1, $3); }
    ;

unary:
      primary                                { $$ = $1; }
    | MINUS unary %prec UMINUS               { $$ = ast_unary("-", $2); }
    ;

primary:
      INT                                    { $$ = ast_int($1); }
    | FLOAT                                  { $$ = ast_float($1); }
    | STRING                                 { $$ = ast_string($1); }
    | ID                                     { $$ = ast_ident($1); }
    | LPAREN expr RPAREN                     { $$ = $2; }
    ;

%%

void yyerror(const char *s) {
    printf("Parser error: %s\n", s);
}

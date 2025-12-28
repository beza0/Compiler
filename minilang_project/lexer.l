%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

%}


%option noyywrap
%option yylineno

%%

[ \t\r]+                ;
\n                      ;

 /* Single line comment */
"//"[^\n]* ;

 /* Multi-line comment (Düzeltilmiş) */
"/*"([^*]|\*+[^*/])*\*+"/"  ;

 /* Keywords */
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"for"                   { return FOR; }
"int"                   { return KW_INT; }
"float"                 { return KW_FLOAT; }
"string"                { return KW_STRING; }
"print"                 { return PRINT; }
"return"                { return RETURN; }

 /* Operators */
"=="                    { return EQ; }
"!="                    { return NEQ; }
">="                    { return GE; }
"<="                    { return LE; }
">"                     { return GT; }
"<"                     { return LT; }
"="                     { return ASSIGN; }
";"                     { return SEMI; }
","                     { return COMMA; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return STAR; }
"/"                     { return SLASH; }

 /* Float */
[0-9]+\.[0-9]+         { yylval.fval = atof(yytext); return FLOAT; }

 /* Integer */
[0-9]+                  { yylval.ival = atoi(yytext); return INT; }

 /* String */
\"[^\"]*\"              {
                            size_t len = yyleng - 2;
                            char *s = malloc(len + 1);
                            memcpy(s, yytext + 1, len);
                            s[len] = '\0';
                            yylval.sval = s;
                            return STRING;
                        }

 /* Identifier */
[a-zA-Z_][a-zA-Z0-9_]*  {
                            yylval.sval = strdup(yytext);
                            return ID;
                        }

 /* Unknown */
.                       { printf("Unknown token: %s\n", yytext); }

%%

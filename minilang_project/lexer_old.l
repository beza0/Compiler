%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

int yylineno = 1;
%}

%option noyywrap

%%

[ \t\r]+                ;
\n                      { yylineno++; }

/* Tek satır yorum */
"//"[^\n]*              ;



/* Keyword'ler */
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"for"                   { return FOR; }
"int"                   { return KW_INT; }
"float"                 { return KW_FLOAT; }
"string"                { return KW_STRING; }
"print"                 { return PRINT; }
"return"                { return RETURN; }

/* Operatörler ve noktalama */
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

/* Float: 12.34 */
[0-9]+"."[0-9]+         { yylval.fval = atof(yytext); return FLOAT; }

/* Integer: 123 */
[0-9]+                  { yylval.ival = atoi(yytext); return INT; }

/* String literal */
\"[^\"]*\"              {
                            size_t len = yyleng - 2;
                            char *s = (char*)malloc(len + 1);
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

/* Diğer karakterler */
.                       { printf("Unknown token: %s\n", yytext); }

%%

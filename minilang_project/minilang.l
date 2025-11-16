%{
#include <stdio.h>
%}

digit       [0-9]
letter      [a-zA-Z_]
id          {letter}({letter}|{digit})*
number      {digit}+(\.{digit}+)?
string      \"[^"\n]*\"

%%

"if"            { printf("TOKEN_IF\n"); }
"else"          { printf("TOKEN_ELSE\n"); }
"while"         { printf("TOKEN_WHILE\n"); }
"for"           { printf("TOKEN_FOR\n"); }
"int"           { printf("TOKEN_INT\n"); }
"float"         { printf("TOKEN_FLOAT\n"); }
"string"        { printf("TOKEN_STRING\n"); }
"print"         { printf("TOKEN_PRINT\n"); }
"return"        { printf("TOKEN_RETURN\n"); }

"=="            { printf("TOKEN_EQ\n"); }
"!="            { printf("TOKEN_NEQ\n"); }
">="            { printf("TOKEN_GTE\n"); }
"<="            { printf("TOKEN_LTE\n"); }
">"             { printf("TOKEN_GT\n"); }
"<"             { printf("TOKEN_LT\n"); }

"+"             { printf("TOKEN_PLUS\n"); }
"-"             { printf("TOKEN_MINUS\n"); }
"*"             { printf("TOKEN_MUL\n"); }
"/"             { printf("TOKEN_DIV\n"); }
"="             { printf("TOKEN_ASSIGN\n"); }

"("             { printf("TOKEN_LPAREN\n"); }
")"             { printf("TOKEN_RPAREN\n"); }
"{"             { printf("TOKEN_LBRACE\n"); }
"}"             { printf("TOKEN_RBRACE\n"); }
";"             { printf("TOKEN_SEMICOLON\n"); }
","             { printf("TOKEN_COMMA\n"); }

{number}        { printf("TOKEN_NUMBER(%s)\n", yytext); }
{id}            { printf("TOKEN_IDENTIFIER(%s)\n", yytext); }
{string}        { printf("TOKEN_STRING_LITERAL(%s)\n", yytext); }

"//".*          { /* single line comment, ignore */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* multi-line comment, ignore */ }

[ \t\n]+        { /* whitespace, ignore */ }
.               { printf("UNKNOWN(%s)\n", yytext); }

%%

int yywrap() {
    return 1;
}


int main(void) {
    yylex();
    return 0;
}


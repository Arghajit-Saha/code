%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct Entry { char* name; int value; } symbols[256];
int count = 0;
int yylex(void);
void yyerror(char* s);
int lookup(char* name) {
    for (int i = 0; i < count; ++i) if (!strcmp(symbols[i].name, name)) return i;
    return -1;
}
int getValue(char* name) {
    int i = lookup(name);
    if (i < 0) { fprintf(stderr, "Undefined variable = %s\n", name); exit(1); }
    return symbols[i].value;
}
void assign(char* name, int value) {
    int i = lookup(name);
    if (i < 0) { i = count++; symbols[i].name = strdup(name); }
    symbols[i].value = value;
    printf("%s = %d\n", name, value);
}
%}
%union { int number; char* name; }
%token <number> NUMBER
%token <name> IDENTIFIER
%type <number> expr
%left '+' '-'
%left '*' '/'
%right UMINUS
%%
program: | program statement;
statement: IDENTIFIER '=' expr '\n' { assign($1, $3); free($1); }
         | expr '\n' { printf("Result = %d\n", $1); }
         | '\n';
expr: NUMBER { $$ = $1; }
    | IDENTIFIER { $$ = getValue($1); free($1); }
    | expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | expr '/' expr { if (!$3) { yyerror("division by zero"); YYABORT; } $$ = $1 / $3; }
    | '-' expr %prec UMINUS { $$ = -$2; }
    | '(' expr ')' { $$ = $2; };
%%
void yyerror(char* s) { fprintf(stderr, "Syntax error = %s\n", s); }
int main(void) { puts("Enter statements ="); return yyparse(); }

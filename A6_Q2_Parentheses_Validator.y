%{
#include <stdio.h>
int yylex(void);
void yyerror(char* s);
%}
%token NUMBER
%left '+' '-'
%left '*' '/'
%right UMINUS
%%
lines: | lines line;
line: expr '\n' { printf("Valid expression = %d\n", $1); }
    | error '\n' { puts("Invalid expression"); yyerrok; }
    | '\n';
expr: NUMBER { $$ = $1; }
    | '(' expr ')' { $$ = $2; }
    | expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | expr '/' expr { if (!$3) { yyerror("division by zero"); YYERROR; } $$ = $1 / $3; }
    | '-' expr %prec UMINUS { $$ = -$2; };
%%
void yyerror(char* s) { fprintf(stderr, "Error = %s\n", s); }
int main(void) { puts("Enter arithmetic expressions ="); return yyparse(); }

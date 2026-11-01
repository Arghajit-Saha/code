%option noyywrap
%{
#include "q1.tab.h"
#include <stdlib.h>
#include <string.h>
%}
%%
[0-9]+                    { yylval.number = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*     { yylval.name = strdup(yytext); return IDENTIFIER; }
[ \t\r]+                  ;
\n                        return '\n';
.                         return yytext[0];
%%

%option noyywrap
%{
#include "q2.tab.h"
#include <stdlib.h>
%}
%%
[0-9]+       { yylval = atoi(yytext); return NUMBER; }
[ \t\r]+     ;
\n           return '\n';
.            return yytext[0];
%%

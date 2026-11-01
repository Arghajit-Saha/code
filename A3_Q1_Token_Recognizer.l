%option noyywrap
%{
#include <stdio.h>
int line_num = 1;
%}
DIGIT [0-9]
LETTER [a-zA-Z_]
ID {LETTER}({LETTER}|{DIGIT})*
INTEGER {DIGIT}+
FLOAT {DIGIT}+\.{DIGIT}+
%%
\n { ++line_num; }
[ \t\r]+ ;
"//"[^\n]* ;
"int"|"float"|"if"|"else"|"while"|"return"|"for"|"void" printf("Line %d | KEYWORD = %s\n", line_num, yytext);
{FLOAT} printf("Line %d | FLOAT = %s\n", line_num, yytext);
{INTEGER} printf("Line %d | INTEGER = %s\n", line_num, yytext);
{ID} printf("Line %d | IDENTIFIER = %s\n", line_num, yytext);
"=="|"!="|"<="|">="|"="|"<"|">" printf("Line %d | REL_ASSIGN = %s\n", line_num, yytext);
"++"|"--"|"+"|"-"|"*"|"/"|"%" printf("Line %d | OPERATOR = %s\n", line_num, yytext);
[(){};,] printf("Line %d | PUNCTUATION = %s\n", line_num, yytext);
. printf("Line %d | UNKNOWN = %s\n", line_num, yytext);
%%
int main(void) {
    puts("Enter source code. Finish with Ctrl+D on a new line =");
    yylex();
    return 0;
}

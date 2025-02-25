%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEFT_PAR 1
#define RIGHT_PAR 2
#define PLUS 3
#define MINUS 4
#define MULT 5
#define DIV 6
#define MOD 7
#define ID 8
#define NUM 9
#define ERROR 13

%}

ID              [a-zA-Z_][a-zA-Z0-9_]*
NUM             [+-]?[0-9]+
%%
"("             { return LEFT_PAR; }
")"             { return RIGHT_PAR; }
\+              { return PLUS; }
"-"             { return MINUS; }
\*              { return MULT; }
"/"             { return DIV; }
"%"             { return MOD; }
{ID}            { return ID; }
{NUM}           { return NUM; }
[ \t\n]+        { /* Ignore whitespace */ }
.               { return ERROR; }

%%

 int yywrap(void) { return 1; }

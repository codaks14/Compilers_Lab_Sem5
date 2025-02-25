%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.c"

%}

%%

[ \t\n\r]+          {}
set               { return SET; }
[\+\-]?[0-9]+            {yylval.val = atoi(yytext);  return NUM; }
[a-zA-Z_][a-zA-Z_0-9]*  {S=add_symbol(S,yytext); yylval.table = find_symbol(S,yytext); return ID;}
\*\*              { return EXP; }  
\+                { return PLUS; }
\-                { return MINUS; }
\*                { return MUL; }
\/                { return DIV; }
%                 { return MOD; }
\(                 { return '('; }
\)                 { return ')'; }
.                 { }

%%
int yywrap(void)
{
    return 1;
}
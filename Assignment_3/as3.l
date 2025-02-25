%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.c"

%}

%%

set               { return SET; }
[\+\-]?[0-9]+            {S=add_symbol(S,"#",atoi(yytext),NUM);yylval.table=find_symbol(S,"#",NUM,atoi(yytext));  return NUM; }
[a-zA-Z_][a-zA-Z_0-9]*  {S=add_symbol(S,yytext,-1,ID);yylval.table=find_symbol(S,yytext,ID,-1); return ID; }
\*\*              { return EXP; }  
\+                { return PLUS; }
\-                { return MINUS; }
\*                { return MUL; }
\/                { return DIV; }
%                 { return MOD; }
\(                 { return '('; }
\)                 { return ')'; }
[ \t\n\r]+          {}
.                 { }

%%
int yywrap(void)
{
    return 1;
}
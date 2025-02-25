%{
   #include "y.tab.c"
   #include<string>
%}

spaces                [ \t\r\n]+
numb                  [+-]?[0-9]+
iden                  [a-zA-Z_]([a-zA-Z_]|[0-9])*
errors                 .

%%

{spaces}           { }
"set"              { return SET; }
"when"             { return WHEN; }
"loop"             { return LOOP; }
"while"            { return WHILE; }
{numb}             { yylval.num = atoi(yytext); return NUMB; }
"("                { return LP; }
")"                { return RP; }
"+"                { return PLUS; }
"-"                { return MINUS; }
"*"                { return MUL; }
"/="               { return NE; }
"/"                { return DIV; }
"%"                { return MOD; }
"="                { return ASSIGN; }
"<"                { return LT; }
"<="               { return LE; }
">"                { return GT; }
">="               { return GE; }
{iden}             { yylval.str = strdup(yytext); return IDEN; }
{errors}           {  }

%%

int yywrap() {
    return 1;
}
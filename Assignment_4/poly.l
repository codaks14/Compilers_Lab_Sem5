%{
 #include "y.tab.c"

%}
spaces [ \t\n\r]+ 

%%
{spaces} {}
"1" {return ONE;}
"0" {return ZERO;}
"x" {return x;}
"+" {return PLUS;}
"-" {return MINUS;}
"^" {return EXPONENT;}
[2-9] {yylval.val = atoi(yytext); return Digit;}
%%

int yywrap(void){
    return 1;
}
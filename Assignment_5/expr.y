%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define id 0
#define num 1
#define expr 2

typedef struct _symbol
{
    char *name;
    int offset;
    struct _symbol *next;
} symbol;

typedef struct _expression
{
    int regindex;
    int memoryindex;
    int value;
    int type;
    int index_type;
} expression;

typedef symbol *symbolTable;
symbolTable S= NULL;
FILE *fptr;

symbolTable add_symbol(symbolTable S,char *name);
symbolTable find_symbol(symbolTable S,char *name);
expression expression_assign(int op,expression arg1,expression arg2);

int memcnt=0;
int regcnt=2;
int yylex();
void yyerror(char *s);
%}

%union
{
    symbolTable table;
    int val;
    expression exp;
}
%start PROGRAM
%token <table> ID
%token SET 
%token PLUS MINUS EXP DIV MUL MOD
%token <val> NUM
%type  <exp> EXPR ARG
%type STMT SETSTMT EXPRSTMT 
%type <val> OP

%%
PROGRAM : STMT PROGRAM {}
        | STMT {}
        ;
STMT    : SETSTMT {}
        | EXPRSTMT {}
        ;
SETSTMT : '(' SET ID NUM ')' { fprintf(fptr,"MEM[%d] = %d;\n\t",$3->offset,$4);fprintf(fptr,"mprn(MEM,%d);\n\t",$3->offset); }
        | '(' SET ID ID ')' { fprintf(fptr,"R[0] = MEM[%d];\n\t",$4->offset); fprintf(fptr,"MEM[%d] = R[0];\n\t",$3->offset); fprintf(fptr,"mprn(MEM,%d);\n\t",$3->offset); }
        | '(' SET ID EXPR ')' { fprintf(fptr,"MEM[%d] = R[%d];\n\t",$3->offset,$4.regindex); regcnt--; fprintf(fptr,"mprn(MEM,%d);\n\t",$3->offset); }
EXPRSTMT : EXPR {regcnt--; fprintf(fptr,"eprn(R,%d);\n\t",regcnt);}
        ;
EXPR    : '(' OP ARG ARG ')' { 
                                $$=expression_assign($2,$3,$4);
                            }
        ;
OP      : PLUS { $$ = PLUS; }
        | MINUS { $$ = MINUS; }
        | MUL { $$ = MUL; }
        | DIV { $$ = DIV; }
        | MOD { $$ = MOD; }
        | EXP { $$ = EXP; }
        ;
ARG     : ID { $$.type = 0;$$.memoryindex = $1->offset; }
        | NUM { $$.type = 1; $$.value = $1; }
        | EXPR { $$.type=2;
                $$.regindex = $1.regindex;
                $$.index_type = $1.index_type;
                $$.memoryindex=$1.memoryindex;
                $$.value = $1.value;

                } 
        ;

%%

void yyerror(char *s)
{
    fprintf(stderr, "%s\n\t", s);
    exit(1);
}




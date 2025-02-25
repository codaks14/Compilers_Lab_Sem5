%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The symbol node in the symbol table consists of the name of the variable, its value, its type and a pointer to the next symbol in the table
typedef struct _symbol
{
    char *name;
    int value;
    int type;
    struct _symbol *next;
} symbol;

typedef symbol *symbolTable;
symbolTable S= NULL;

// The tree node in the syntax tree consists of the type of the node, the value of the node, and pointers to the left and right children
typedef struct _tree
{
    int type;
    union
    {
        symbolTable s;
        int op;

    } val;
    struct _tree *left;
    struct _tree *right;

} tree;

// write the protoptypes of all the functions
symbolTable add_symbol(symbolTable, char *, int, int);
symbolTable find_symbol(symbolTable, char *,int, int);
tree * create_leaf(symbol *, int);
tree * create_op_node(int, tree *, tree *);
int eval_tree(tree *);
void free_tree(tree *);
void set_value(symbolTable, int);
int read_val(symbolTable);
int yylex(void);
void yyerror(char *);

%}

%union
{
    symbolTable table;
    tree *treenode;
    char *name;
    int type;
}
%start PROGRAM
%token <table> ID NUM
%token SET 
%token PLUS MINUS EXP DIV MUL MOD
%token LPAREN RPAREN
%type <treenode> EXPR ARG
%type STMT SETSTMT EXPRSTMT 
%type <type> OP 

%%
PROGRAM : STMT PROGRAM {}
        | STMT {}
        ;
STMT    : SETSTMT {}
        | EXPRSTMT {}
        ;
SETSTMT : '(' SET ID NUM ')' {set_value($3,read_val($4)); printf("Variable %s is set to %d\n",$3->name,read_val($4));}
        | '(' SET ID ID ')' {set_value($3,read_val($4)); printf("Variable %s is set to %d\n",$3->name,read_val($4));}
        | '(' SET ID EXPR ')' {set_value($3,eval_tree($4)); printf("Variable %s is set to %d\n",$3->name,read_val($3));}
EXPRSTMT : EXPR {printf("Standalone expression evaluates to %d\n",eval_tree($1));}
        ;
EXPR    : '(' OP ARG ARG ')' {$$=create_op_node($2,$3,$4);}
        ;
OP      : PLUS { $$ = PLUS; }
        | MINUS { $$ = MINUS; }
        | MUL { $$ = MUL; }
        | DIV { $$ = DIV; }
        | MOD { $$ = MOD; }
        | EXP { $$ = EXP; }
        ;
ARG     : ID { $$ = create_leaf($1,1); }
        | NUM { $$ = create_leaf($1,2); }
        | EXPR { $$ = $1; }
        ;
%%

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}




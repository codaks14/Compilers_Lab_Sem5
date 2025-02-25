%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(char *s);
int yylex();

typedef struct poly_node
{
     int inh;
     int val;
     char *type;
     struct poly_node *left;
     struct poly_node *right;
     struct poly_node *neither_left_nor_right;
} node;

node *create_node(char *name);
void add_child(node *parent, node *child);
void add_leaf(node *parent, char *leaf);
void add_digit_leaf(node *parent, int leaf);
void create_root(node *child);
void print_tree(node *root,int depth);
 
%}

%union{
    char val;
    node * nodep;
}

%start Program

%token <val> Digit
%type <nodep> Start Polynomial Term X_power Number Number_M Program

%token PLUS MINUS EXPONENT x ZERO ONE


%%

Program : Start { create_root($1); }
        ;
Start : Polynomial {$$=create_node("S");add_child($$,$1);}
      | PLUS Polynomial {$$=create_node("S");add_leaf($$,"+");add_child($$,$2);}
      | MINUS Polynomial {$$=create_node("S");add_leaf($$,"-");add_child($$,$2);}
      ;

Polynomial : Term {$$=create_node("P");add_child($$,$1); }
           | Term PLUS Polynomial {$$=create_node("P");add_child($$,$1);add_leaf($$,"+");add_child($$,$3); }
           | Term MINUS Polynomial {$$=create_node("P");add_child($$,$1);add_leaf($$,"-");add_child($$,$3); }
           ;

Term : ONE { $$=create_node("T");add_leaf($$,"1");}
    | Number { $$=create_node("T");add_child($$,$1); }
    | X_power { $$=create_node("T");add_child($$,$1); }
    | Number X_power { $$=create_node("T");add_child($$,$1);add_child($$,$2); }
    ;

X_power : x EXPONENT Number { $$=create_node("X");add_leaf($$,"x");add_leaf($$,"^");add_child($$,$3); }
        | x { $$=create_node("X");add_leaf($$,"x"); }
        ;

Number : Digit { $$=create_node("N");add_digit_leaf($$,$1); }
       | ONE Number_M { $$=create_node("N");add_leaf($$,"1");add_child($$,$2); }
       | Digit Number_M { $$=create_node("N");add_digit_leaf($$,$1);add_child($$,$2); }
        ;

Number_M : ZERO { $$=create_node("M");add_leaf($$,"0"); }
         | ONE { $$=create_node("M");add_leaf($$,"1"); }
         | Digit { $$=create_node("M");add_digit_leaf($$,$1); }
         | ZERO Number_M { $$=create_node("M");add_leaf($$,"0");add_child($$,$2); }
         | ONE Number_M { $$=create_node("M");add_leaf($$,"1");add_child($$,$2); }
         | Digit Number_M { $$=create_node("M");add_digit_leaf($$,$1);add_child($$,$2); }
         ;

%%

void yyerror(char *s){
    fprintf(stderr, "%s\n", s);
}

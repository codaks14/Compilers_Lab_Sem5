#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lex.yy.c"
//write comments for the code explaining the functions and variables

// this function is used to create a new symbol and add it to the symbol table if it is not already present
symbolTable add_symbol(symbolTable table, char *name, int value, int type)
{
    symbol *t = find_symbol(table, name, type, value);
    if (t != NULL)
    {
        return table;
    }
    symbol *s = (symbol *)malloc(sizeof(symbol));
    s->type = type;
    s->name = strdup(name);
    s->value = value;
    s->next = table;
    return s;
}

// this function is used to find a symbol in the symbol table
symbolTable find_symbol(symbolTable table, char *name, int type, int value)
{
    if (type == NUM)
    {
        symbol *s;
        for (s = table; s != NULL; s = s->next)
            if (s->value == value)
                return s;
        return NULL;
    }
    else
    {
        symbol *s;
        for (s = table; s != NULL; s = s->next)
            if (strcmp(s->name, name) == 0)
                return s;
        return NULL;
    }
}

// this function is used to create a new leaf node in the tree
tree *create_leaf(symbol *s, int type)
{
    tree *t = (tree *)malloc(sizeof(tree));
    if (type == ID)
    {
        t->type = ID;
        t->val.s = s;
    }
    else
    {
        t->val.s = s;
        t->type = NUM;
    }
    t->left = NULL;
    t->right = NULL;
    return t;
}

// this function is used to create a new operator node in the tree
tree *create_op_node(int op, tree *left, tree *right)
{
    tree *t = (tree *)malloc(sizeof(tree));
    t->type = op;
    t->val.op = op;
    t->left = left;
    t->right = right;
    return t;
}

// this function is used to get the value of a symbol
int read_val(symbolTable s)
{
    return s->value;
}

// this function is used to evaluate the binary exponential operation
int bin_exp(int a, int b)
{
    int tem = 1;
    while (b > 0)
    {
        if (b & 1)
            tem = tem * a;
        a = a * a;
        b >>= 1;
    }
    return tem;
}

// this function is used to evaluate the tree
int eval_tree(tree *node)
{
    if (node->type == NUM)
        return node->val.s->value;
    else if (node->type == ID)
    {
        return node->val.s->value;
    }
    else
    {
        int left = eval_tree(node->left);
        int right = eval_tree(node->right);
        switch (node->val.op)
        {
        case PLUS:
            return left + right;
        case MINUS:
            return left - right;
        case MUL:
            return left * right;
        case DIV:
        {
            if (right == 0)
            {
                fprintf(stderr, "Error: division by zero\n");
                exit(1);
            }
            return left / right;
        }
        case MOD:
        {
            if (right == 0)
            {
                fprintf(stderr, "Error: Modulo by zero\n");
                exit(1);
            }
            return left % right;
        }
        case EXP:
        {
            if (left == 0 && right == 0)
            {
                fprintf(stderr, "Error: 0^0 is undefined\n");
                exit(1);
            }
            if (left == 0 && right < 0)
            {
                fprintf(stderr, "Error: 0^negative is undefined\n");
                exit(1);
            }
            if (right < 0)
            {
                return 0;
            }
            return bin_exp(left, right);
        }
        default:
            fprintf(stderr, "Error: unknown operator %d\n", node->val.op);
            exit(1);
        }
    }
}

// this function is used to set the value of a ID symbol
void set_value(symbolTable s, int value)
{
    s->value = value;
}
int main()
{
    yyparse();
}

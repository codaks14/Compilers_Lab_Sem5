#include <iostream>
#include <stack>
#include <cstring>
#include "lex.yy.c"
#define END 0
#define EXPR 10
#define OP 11
#define ARG 12

using namespace std;

typedef struct _node
{
    char *name;
    int val;
    struct _node *next;
} _node;
typedef _node *symboltable;

typedef struct const_node
{
    int value;
    struct const_node *next;
} const_node;
typedef const_node *const_table;

typedef struct node
{
    int type;
    union
    {
        char op;         
        symboltable id;  
        const_table num;
    };
    struct node *left, *right, *parent;
} Node;

// Function to create a new node
Node *createNode(int type)
{
    Node *newNode = new Node;
    newNode->type = type;
    newNode->left = newNode->right = newNode->parent = nullptr;
    return newNode;
}

const_table addtoconsttable(const_table T, char *id)
{
    const_table p = T;
    int num = stoi(id);
    while (p)
    {
        if (num == p->value)
        {
            return T; // Identifier already exists
        }
        p = p->next;
    }

    p = new const_node;
    p->value = num;
    p->next = T;
    return p;
}
const_table find_const(const_table T, char *id)
{
    const_table p = T;
    int num = stoi(id);
    while (p)
    {
        if (num == p->value)
        {
            return p; 
        }
        p = p->next;
    }
    return NULL;
}
symboltable addtosymboltable(symboltable T, char *id)
{
    symboltable p = T;

    while (p)
    {
        if (!strcmp(p->name, id))
        {
            return T;
        }
        p = p->next;
    }

    p = new _node;
    p->name = new char[strlen(id) + 1];
    strcpy(p->name, id);
    p->next = T;
    return p;
}
symboltable find_symbol(symboltable T, char *id)
{
    symboltable p = T;

    while (p)
    {
        if (!strcmp(p->name, id))
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void printParseTree(Node *root, int level = 0)
{
    if (!root)
        return;

    for (int i = 0; i < level; ++i)
    {
        cout << "\t";
    }
    if (level)
        cout << "---> ";

    // Print the current node
    if (root->type == OP)
    {
        cout << "OP(" << root->op << ")" << endl;
    }
    else if (root->type == ID)
    {
        cout << "ID(" << root->id->name << ")" << endl;
    }
    else if (root->type == NUM)
    {
        cout << "NUM(" << root->num->value << ")" << endl;
    }

    // Recursively print left and right children
    printParseTree(root->left, level + 1);
    printParseTree(root->right, level + 1);
}
int evaluate(Node *root)
{
    if (!root)
        return 0;

    if (root->type == NUM)
    {
        return root->num->value;
    }
    else if (root->type == ID)
    {
        return root->id->val;
    }
    else if (root->type == OP)
    {
        int leftVal = evaluate(root->left);
        int rightVal = evaluate(root->right);

        switch (root->op)
        {
        case '+':
            return leftVal + rightVal;
        case '-':
            return leftVal - rightVal;
        case '*':
            return leftVal * rightVal;
        case '/':
        {
            if (rightVal == 0)
            {
                cout << "Error : Division by 0 is not possible" << endl;
                exit(1);
            }
            return leftVal / rightVal;
        }
        case '%':
        {
            if (rightVal == 0)
            {
                cout << "Error : Modulo by 0 is not possible" << endl;
                exit(1);
            }

            return leftVal % rightVal;
        }
        }
    }

    return 0;
}

extern int yylex();
extern int yylineno;
extern char *yytext;

int main()
{
    const_table C = NULL;
    symboltable T = NULL;
    int nextok;

    int parse_tbl[13][13] = {0};
    parse_tbl[EXPR][LEFT_PAR] = 1;
    parse_tbl[OP][PLUS] = 2;
    parse_tbl[OP][MINUS] = 3;
    parse_tbl[OP][MULT] = 4;
    parse_tbl[OP][DIV] = 5;
    parse_tbl[OP][MOD] = 6;
    parse_tbl[ARG][ID] = 7;
    parse_tbl[ARG][NUM] = 8;
    parse_tbl[ARG][RIGHT_PAR] = 9;

    stack<int> s;
    stack<Node *> tree;
    Node *root = nullptr;

    s.push(EXPR);
    int token = yylex();
    Node *currentNode = nullptr;
    while (!s.empty())
    {
        int top = s.top();
        s.pop();

        if (top == EXPR)
        {
            if (parse_tbl[EXPR][token] == 1)
            {
                s.push(RIGHT_PAR);
                s.push(ARG);
                s.push(ARG);
                s.push(OP);
                s.push(LEFT_PAR);
            }
            else
            {
                cout << "Error : Expected ( in place of " << yytext << endl;
                exit(1);
            }
        }
        else if (top == OP)
        {
            // cout<<token<<endl;
            if (parse_tbl[OP][token] >= 2 && parse_tbl[OP][token] <= 6)
            {
                if (currentNode == NULL)
                {
                    currentNode = createNode(OP);
                    root = currentNode;
                }
                else if (currentNode->left == NULL)
                {
                    currentNode->left = createNode(OP);
                    currentNode->left->parent = currentNode;
                    currentNode = currentNode->left;
                }
                else if (currentNode->right == NULL)
                {
                    currentNode->right = createNode(OP);
                    currentNode->right->parent = currentNode;
                    currentNode = currentNode->right;
                }
                char c;
                switch (token)
                {
                case 3:
                    c = '+';
                    break;
                case 4:
                    c = '-';
                    break;
                case 5:
                    c = '*';
                    break;
                case 6:
                    c = '/';
                    break;
                case 7:
                    c = '%';
                    break;
                }
                currentNode->op = c;
                token = yylex();
            }
            else
            {
                cout << "Error : Expected operator in place of " << yytext << endl;
                exit(1);
            }
        }
        else if (top == ARG)
        {
            if (token == ID)
            {
                s.push(ID);
            }
            else if (token == NUM)
            {
                s.push(NUM);
            }
            else if (token == LEFT_PAR)
            {
                s.push(EXPR);
            }
            else
            {
                cout << "Error: Expected ID, NUM, or '(' in place of '" << yytext << "'" << endl;
                exit(1);
            }
        }
        else if (top == LEFT_PAR)
        {
            if (token != LEFT_PAR)
            {
                cout << "Error: Expected '(' in place of '" << yytext << "'" << endl;
                exit(1);
            }
            token = yylex();
        }
        else if (top == RIGHT_PAR)
        {
            if (token != RIGHT_PAR)
            {
                cout << "Error: Expected ')' in place of '" << yytext << "'" << endl;
                exit(1);
            }
            token = yylex();
        }
        else if (top == ID)
        {
            if (token != ID)
            {
                cout << "Error: Expected ID in place of '" << yytext << "'" << endl;
                exit(1);
            }
            T = addtosymboltable(T, yytext);
            if (currentNode == NULL)
            {
                currentNode = createNode(ID);
                root = currentNode;
            }
            else if (currentNode->left == NULL)
            {
                currentNode->left = createNode(ID);
                currentNode->left->parent = currentNode;
                currentNode = currentNode->left;
            }
            else
            {
                currentNode->right = createNode(ID);
                currentNode->right->parent = currentNode;
                currentNode = currentNode->right;
            }
            currentNode->id = find_symbol(T, yytext);
            currentNode = currentNode->parent;
            while (currentNode && currentNode->right != nullptr)
            {
                currentNode = currentNode->parent;
            }
            token = yylex();
        }
        else if (top == NUM)
        {
            if (token != NUM)
            {
                cout << "Error: Expected NUM in place of '" << yytext << "'" << endl;
                exit(1);
            }
            C = addtoconsttable(C, yytext);
            if (currentNode == NULL)
            {
                currentNode = createNode(NUM);
                root = currentNode;
            }
            else if (currentNode->left == NULL)
            {
                currentNode->left = createNode(NUM);
                currentNode->left->parent = currentNode;
                currentNode = currentNode->left;
            }
            else
            {
                currentNode->right = createNode(NUM);
                currentNode->right->parent = currentNode;
                currentNode = currentNode->right;
            }
            currentNode->num = find_const(C, yytext);
            currentNode = currentNode->parent;
            while (currentNode && currentNode->right != nullptr)
            {
                currentNode = currentNode->parent;
            }
            token = yylex();
        }
        else
        {
            cout << "Error : Unknown token!!!!" << endl;
            exit(1);
        }
    }

    printParseTree(root, 0);
    symboltable p = T;
    stack<symboltable> input_values;
    if (p != NULL)
    {
        while (p)
        {
            input_values.push(p);
            p = p->next;
        }
    }
    p = T;
    if (p != NULL)
    {

        while (!input_values.empty())
        {
            auto value_ = input_values.top();
            input_values.pop();
            int value = stoi(yytext);
            token = yylex();
            cout << value_->name << " = " << value << endl;
            value_->val = value;
        }
    }
    int result = evaluate(root);
    cout << "The result of the expression is: " << result << endl;
    return 0;
}

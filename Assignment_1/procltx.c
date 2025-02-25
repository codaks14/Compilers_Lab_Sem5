#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lex.yy.c"

// Define node structure for symbol tables
typedef struct _node {
    char *name;
    int count;
    struct _node *next;
} Node;
typedef Node* SymbolTable;

// Function to add or update the count of a command or environment in the symbol table
SymbolTable add_to_table(SymbolTable table, char *name) {
    Node *current = table;

    // Check if the name already exists in the table
    while (current) {
        if (strcmp(current->name, name) == 0) {
            current->count++;
            return table;
        }
        current = current->next;
    }

    // Add new entry to the table
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    strcpy(new_node->name, name);
    new_node->count = 1;
    new_node->next = table;
    return new_node;
}

// Function to print the symbol table
void print_table(SymbolTable table, const char *type) {
    printf("%s used:\n", type);
    Node *current = table;
    while (current) {
        printf("\t%s (%d)\n", current->name, current->count);
        current = current->next;
    }
}

void print_table1(SymbolTable table, const char *type) {
    printf("%s used:\n", type);
    Node *current = table;
    while (current) {
        printf("\t");
        for(int i=7;i<strlen(current->name)-1;i++) printf("%c", current->name[i]);
        printf(" (%d)\n",current->count);
        current = current->next;
    }
}

int main() {
    int token;
    SymbolTable commands = NULL;
    SymbolTable environments = NULL;
    int inline_math_count = 0;
    int displayed_math_count = 0;
    int next_line_math=0;

    // Lexical analysis
    while ((token = yylex())) {
        switch (token) {
            case TILDA:
                break;
            case CMD1:
                commands = add_to_table(commands, yytext);
                break;
            case CMD2:
                commands = add_to_table(commands, yytext);
                break;
            case BEGINN:
                environments = add_to_table(environments, yytext);
                break;
            case END:
                break;
            case MATH:
                inline_math_count++;
                break;
            case MATHDISPLAY:
                displayed_math_count++;
                break;
            case COMMENT:
                break;
            case MATHMULTILINE:
                 next_line_math++;
                 break;    
            default:
                break;
        }
    }
    print_table(commands, "Commands");
    print_table1(environments, "Environments");
    printf("%d inline math equations found\n", inline_math_count);
    printf("%d displayed equations found\n", displayed_math_count+(next_line_math/2));
    while (commands) {
        Node *temp = commands;
        commands = commands->next;
        free(temp->name);
        free(temp);
    }

    while (environments) {
        Node *temp = environments;
        environments = environments->next;
        free(temp->name);
        free(temp);
    }

    return 0;
}

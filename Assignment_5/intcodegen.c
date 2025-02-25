#include "lex.yy.c"

symbolTable add_symbol(symbolTable table, char *name)
{
    symbol *t = find_symbol(table, name);
    if (t != NULL)
    {
        return table;
    }
    symbolTable new_table = (symbolTable)malloc(sizeof(symbolTable));
    char *temp = (char *)malloc(strlen(name) + 1);
    strcpy(temp, name);
    new_table->name = temp;
    new_table->next = table;
    new_table->offset = memcnt++;
    return new_table;
}

symbolTable find_symbol(symbolTable table, char *name)
{

    symbolTable s;
    for (s = table; s != NULL; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

expression expression_assign(int op_ptr, expression arg1, expression arg2)
{
    char c;
    if (op_ptr == PLUS)
        c = '+';
    else if (op_ptr == MINUS)
        c = '-';
    else if (op_ptr == MUL)
        c = '*';
    else if (op_ptr == DIV)
        c = '/';
    else if (op_ptr == MOD)
        c = '%';
    else if (op_ptr == EXP)
        c = '^';
    expression *temp = (expression *)malloc(sizeof(expression));
    temp->type = expr;

    if (arg1.type == num && arg2.type == num)
    {
        if (regcnt <= 11)
        {
            if (c != '^')
                fprintf(fptr, "R[%d] = %d %c %d;\n\t", regcnt, arg1.value, c, arg2.value);
            else
                fprintf(fptr, "R[%d] = pwr(%d,%d);\n\t", regcnt, arg1.value, arg2.value);

            temp->regindex = regcnt;
            temp->memoryindex = -1;
            regcnt++;

            temp->index_type = 0;
        }
        else
        {
            if (c != '^')
                fprintf(fptr, "R[0] = %d %c %d;\n\t", arg1.value, c, arg2.value);
            else
                fprintf(fptr, "R[0] = pwr(%d,%d);\n\t", arg1.value, arg2.value);
            fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

            temp->regindex = -1;
            temp->memoryindex = memcnt;
            memcnt++;
            temp->index_type = 1;
        }
    }
    else if (arg1.type == num && arg2.type == expr)
    {

        if (regcnt <= 12)
        {
            if (arg2.index_type == 0)
            {
                regcnt--;

                if (c != '^')
                {
                    fprintf(fptr, "R[%d] = %d %c R[%d];\n\t", regcnt, arg1.value, c, arg2.regindex);
                }
                else
                    fprintf(fptr, "R[%d] = pwr(%d,R[%d]);\n\t", regcnt, arg1.value, arg2.regindex);
            }
            else if (arg2.index_type == 1)
            {
                fprintf(fptr, "R[0] = MEM[%d];\n\t", arg2.memoryindex);
                if (c != '^')
                {
                    fprintf(fptr, "R[%d] = %d %c R[0];\n\t", regcnt, arg1.value, c);
                }
                else
                {
                    fprintf(fptr, "R[%d] = pwr(%d,R[0]);\n\t", regcnt, arg1.value);
                }
            }

            temp->regindex = regcnt;
            temp->memoryindex = -1;
            regcnt++;
            temp->index_type = 0;
        }
        else
        {
            if (arg2.index_type == 0)
            {
                regcnt--;
                if (c != '^')
                    fprintf(fptr, "R[0] = %d %c R[%d];\n\t", arg1.value, c, arg2.regindex);
                else
                    fprintf(fptr, "R[0] = pwr(%d,R[%d]);\n\t", arg1.value, arg2.regindex);
            }
            else if (arg2.index_type == 1)
            {
                fprintf(fptr, "R[0] = MEM[%d];\n\t", arg2.memoryindex);
                if (c != '^')
                {
                    fprintf(fptr, "R[0] = %d %c R[0];\n\t", arg1.value, c);
                }
                else
                {
                    fprintf(fptr, "R[0] = pwr(%d,R[0]);\n\t", arg1.value);
                }
            }
            fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

            temp->regindex = -1;
            temp->memoryindex = memcnt;
            memcnt++;
            temp->index_type = 1;
        }
    }
    else if (arg1.type == num && arg2.type == id)
    {
        fprintf(fptr, "R[0] = MEM[%d];\n\t", arg2.memoryindex);
        if (regcnt <= 11)
        {
            if (c != '^')
                fprintf(fptr, "R[%d] = %d %c R[0];\n\t", regcnt, arg1.value, c);
            else
                fprintf(fptr, "R[%d] = pwr(%d,R[0]);\n\t", regcnt, arg1.value);

            temp->regindex = regcnt;
            temp->memoryindex = -1;
            regcnt++;
            temp->index_type = 0;
        }
        else
        {
            if (c != '^')
                fprintf(fptr, "R[0] = %d %c R[0];\n\t", arg1.value, c);
            else
                fprintf(fptr, "R[0] = pwr(%d,R[0]);\n\t", arg1.value);
            fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

            temp->regindex = -1;
            temp->memoryindex = memcnt;
            memcnt++;
            temp->index_type = 1;
        }
    }
    else if (arg1.type == id && arg2.type == num)
    {
        fprintf(fptr, "R[0] = MEM[%d];\n\t", arg1.memoryindex);
        if (regcnt <= 11)
        {
            if (c != '^')
                fprintf(fptr, "R[%d] = R[0] %c %d;\n\t", regcnt, c, arg2.value);
            else
                fprintf(fptr, "R[%d] = pwr(R[0],%d);\n\t", regcnt, arg2.value);

            temp->regindex = regcnt;
            temp->memoryindex = -1;
            regcnt++;
            temp->index_type = 0;
        }
        else
        {
            if (c != '^')
                fprintf(fptr, "R[0] = R[0] %c %d;\n\t", c, arg2.value);
            else
                fprintf(fptr, "R[0] = pwr(R[0],%d);\n\t", arg2.value);
            fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

            temp->regindex = -1;
            temp->memoryindex = memcnt;
            memcnt++;
            temp->index_type = 1;
        }
    }
    else if (arg1.type == id && arg2.type == id)
    {
        fprintf(fptr, "R[0] = MEM[%d];\n\t", arg1.memoryindex);
        fprintf(fptr, "R[1] = MEM[%d];\n\t", arg2.memoryindex);
        if (regcnt <= 11)
        {
            if (c != '^')
                fprintf(fptr, "R[%d] = R[0] %c R[1];\n\t", regcnt, c);
            else
                fprintf(fptr, "R[%d] = pwr(R[0],R[1]);\n\t", regcnt);

            temp->regindex = regcnt;
            temp->memoryindex = -1;
            regcnt++;
            temp->index_type = 0;
        }
        else
        {
            if (c != '^')
                fprintf(fptr, "R[0] = R[0] %c R[1];\n\t", c);
            else
                fprintf(fptr, "R[0] = pwr(R[0],R[1]);\n\t");
            fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

            temp->regindex = -1;
            temp->memoryindex = memcnt;
            memcnt++;
            temp->index_type = 1;
        }
    }
    else if (arg1.type == id && arg2.type == expr)
    {
        fprintf(fptr, "R[0] = MEM[%d];\n\t", arg1.memoryindex);
        if (arg2.index_type == 0)
        {
            regcnt--;
            if (regcnt <= 11)
            {
                if (c != '^')
                    fprintf(fptr, "R[%d] = R[0] %c R[%d];\n\t", regcnt, c, arg2.regindex);
                else
                    fprintf(fptr, "R[%d] = pwr(R[0],R[%d]);\n\t", regcnt, arg2.regindex);

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                    fprintf(fptr, "R[0] = R[0] %c R[%d];\n\t", c, arg2.regindex);
                else
                    fprintf(fptr, "R[0] = pwr(R[0],R[%d]);\n\t", arg2.regindex);
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
        else if (arg2.index_type == 1)
        {
            fprintf(fptr, "R[1] = MEM[%d];\n\t", arg2.memoryindex);
            if (regcnt <= 11)
            {
                regcnt--;
                regcnt--;
                if (c != '^')
                    fprintf(fptr, "R[%d] = R[0] %c R[1];\n\t", regcnt, c);
                else
                    fprintf(fptr, "R[%d] = pwr(R[0],R[1]);\n\t", regcnt);

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                    fprintf(fptr, "R[0] = R[0] %c R[1];\n\t", c);
                else
                    fprintf(fptr, "R[0] = pwr(R[0],R[1]);\n\t");
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
    }
    else if (arg1.type == expr && arg2.type == id)
    {
        fprintf(fptr, "R[0] = MEM[%d];\n\t", arg2.memoryindex);
        if (arg1.index_type == 0)
        {
            if (regcnt <= 12)
            {
                regcnt--;
                if (c != '^')
                    fprintf(fptr, "R[%d] = R[%d] %c R[0];\n\t", regcnt, arg1.regindex, c);
                else
                    fprintf(fptr, "R[%d] = pwr(R[%d],R[0]);\n\t", regcnt, arg1.regindex);

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                regcnt--;
                if (c != '^')
                    fprintf(fptr, "R[0] = R[%d] %c R[0];\n\t", arg1.regindex, c);
                else
                    fprintf(fptr, "R[0] = pwr(R[%d],R[0]);\n\t", arg1.regindex);
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
        else if (arg1.index_type == 1)
        {
            fprintf(fptr, "R[1] = MEM[%d];\n\t", arg1.memoryindex);
            if (regcnt <= 11)
            {
                if (c != '^')
                    fprintf(fptr, "R[%d] = R[1] %c R[0];\n\t", regcnt, c);
                else
                    fprintf(fptr, "R[%d] = pwr(R[1],R[0]);\n\t", regcnt);

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                    fprintf(fptr, "R[0] = R[1] %c R[0];\n\t", c);
                else
                    fprintf(fptr, "R[0] = pwr(R[1],R[0]);\n\t");
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
    }
    else if (arg1.type == expr && arg2.type == num)
    {
        if (arg1.index_type == 0)
        {
            if (regcnt <= 12)
            {
                regcnt--;
                if (c != '^')
                    fprintf(fptr, "R[%d] = R[%d] %c %d;\n\t", regcnt, arg1.regindex, c, arg2.value);
                else
                    fprintf(fptr, "R[%d] = pwr(R[%d],%d);\n\t", regcnt, arg1.regindex, arg2.value);

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                regcnt--;
                if (c != '^')
                    fprintf(fptr, "R[0] = R[%d] %c %d;\n\t", arg1.regindex, c, arg2.value);
                else
                    fprintf(fptr, "R[0] = pwr(R[%d],%d);\n\t", arg1.regindex, arg2.value);
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
        else if (arg1.index_type == 1)
        {
            fprintf(fptr, "R[0] = MEM[%d];\n\t", arg1.memoryindex);
            if (regcnt <= 11)
            {
                if (c != '^')
                    fprintf(fptr, "R[%d] = R[0] %c %d;\n\t", regcnt, c, arg2.value);
                else
                    fprintf(fptr, "R[%d] = pwr(R[0],%d);\n\t", regcnt, arg2.value);

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                    fprintf(fptr, "R[0] = R[0] %c %d;\n\t", c, arg2.value);
                else
                    fprintf(fptr, "R[0] = pwr(R[0],%d);\n\t", arg2.value);
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
    }
    else if (arg1.type == expr && arg2.type == expr)
    {
        if (arg1.index_type == 0 && arg2.index_type == 0)
        {
            if (regcnt <= 12)
            {
                regcnt--;
                regcnt--;

                if (c != '^')
                {
                    fprintf(fptr, "R[%d] = R[%d] %c R[%d];\n\t", regcnt, arg1.regindex, c, arg2.regindex);
                }
                else
                {
                    fprintf(fptr, "R[%d] = pwr(R[%d],R[%d]);\n\t", regcnt, arg1.regindex, arg2.regindex);
                }

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                regcnt--;
                regcnt--;

                if (c != '^')
                {
                    fprintf(fptr, "R[0] = R[%d] %c R[%d];\n\t", arg1.regindex, c, arg2.regindex);
                }
                else
                {
                    fprintf(fptr, "R[0] = pwr(R[%d],R[%d]);\n\t", arg1.regindex, arg2.regindex);
                }
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
        else if (arg1.index_type == 0 && arg2.index_type == 1)
        {
            fprintf(fptr, "R[0] = MEM[%d];\n\t", arg2.memoryindex);
            regcnt--;
            if (regcnt <= 11)
            {
                if (c != '^')
                {
                    fprintf(fptr, "R[%d] = R[%d] %c R[0];\n\t", regcnt, arg1.regindex, c);
                }
                else
                {
                    fprintf(fptr, "R[%d] = pwr(R[%d],R[0]);\n\t", regcnt, arg1.regindex);
                }

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                {
                    fprintf(fptr, "R[0] = R[%d] %c R[0];\n\t", arg1.regindex, c);
                }
                else
                {
                    fprintf(fptr, "R[0] = pwr(R[%d],R[0]);\n\t", arg1.regindex);
                }
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
        else if (arg1.index_type == 1 && arg2.index_type == 0)
        {
            fprintf(fptr, "R[0] = MEM[%d];\n\t", arg1.memoryindex);
            regcnt--;
            if (regcnt <= 11)
            {
                if (c != '^')
                {
                    fprintf(fptr, "R[%d] = R[0] %c R[%d];\n\t", regcnt, c, arg2.regindex);
                }
                else
                {
                    fprintf(fptr, "R[%d] = pwr(R[0],R[%d]);\n\t", regcnt, arg2.regindex);
                }

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                {
                    fprintf(fptr, "R[0] = R[0] %c R[%d];\n\t", c, arg2.regindex);
                }
                else
                {
                    fprintf(fptr, "R[0] = pwr(R[0],R[%d]);\n\t", arg2.regindex);
                }
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
        else if (arg1.index_type == 1 && arg2.index_type == 1)
        {
            fprintf(fptr, "R[0] = MEM[%d];\n\t", arg1.memoryindex);
            fprintf(fptr, "R[1] = MEM[%d];\n\t", arg2.memoryindex);
            if (regcnt <= 11)
            {
                if (c != '^')
                {
                    fprintf(fptr, "R[%d] = R[0] %c R[1];\n\t", regcnt, c);
                }
                else
                {
                    fprintf(fptr, "R[%d] = pwr(R[0],R[1]);\n\t", regcnt);
                }

                temp->regindex = regcnt;
                temp->memoryindex = -1;
                regcnt++;
                temp->index_type = 0;
            }
            else
            {
                if (c != '^')
                {
                    fprintf(fptr, "R[0] = R[0] %c R[1];\n\t", c);
                }
                else
                {
                    fprintf(fptr, "R[0] = pwr(R[0],R[1]);\n\t");
                }
                fprintf(fptr, "MEM[%d] = R[0];\n\t", memcnt);

                temp->regindex = -1;
                temp->memoryindex = memcnt;
                memcnt++;
                temp->index_type = 1;
            }
        }
    }
    return *temp;
}

void printSymbolTable(symbolTable table)
{
    symbolTable s;
    for (s = table; s != NULL; s = s->next)
    {
        printf("%s %d;\n\t", s->name, s->offset);
    }
}
void freeSymbolTable(symbolTable table)
{
    symbolTable s;
    for (s = table; s != NULL; s = s->next)
    {
        free(s->name);
        free(s);
    }
}

int main()
{
    fptr = fopen("intcode.c", "w");
    fprintf(fptr, "#include <stdio.h>\n#include <stdlib.h>\n#include \"Au.c\"\n");
    fprintf(fptr, "\nint main( )\n{\n\t");
    fprintf(fptr, "int R[12];\n\tint MEM[65536];\n\t");
    fprintf(fptr, "\n\t");
    yyparse();
    fprintf(fptr, "\n\t");
    fprintf(fptr,"exit(0);\n");
    fprintf(fptr, "}\n");
    fclose(fptr);
    freeSymbolTable(S);
    return 0;
}
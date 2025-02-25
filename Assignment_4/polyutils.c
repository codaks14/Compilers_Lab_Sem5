#include "lex.yy.c"

node *create_node(char *name)
{
    node *temp = (node *)malloc(sizeof(node));
    temp->left = NULL;
    temp->neither_left_nor_right = NULL;
    temp->right = NULL;
    temp->type = name;
    temp->val = 0;
    temp->inh = 1;

    return temp;
}

void add_child(node *parent, node *child)
{
    if (parent->left == NULL)
    {
        parent->left = child;
    }
    else if (parent->neither_left_nor_right == NULL)
    {
        parent->neither_left_nor_right = child;
    }
    else
    {
        parent->right = child;
    }
    return;
}
void add_leaf(node *parent, char *name)
{
    node *temp = create_node(name);
    add_child(parent, temp);
    return;
}
void add_digit_leaf(node *parent, int num)
{
    node *temp = create_node("D");
    temp->val = num;
    add_child(parent, temp);
    return;
}

node *root = NULL;
void create_root(node *temp)
{
    root = temp;
}

void print_tree(node *temp, int depth)
{
    if (temp == NULL)
    {
        return;
    }
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }
    if (!strcmp(temp->type, "D"))
    {
        printf("==> %d ", temp->val);
    }
    else
        printf("==> %s ", temp->type);

    if (!strcmp(temp->type, "P") || !strcmp(temp->type, "T"))
    {
        if (temp->inh == -1)
            printf("[inh = -] \n");
        else
            printf("[inh = +] \n");
    }
    else if (!strcmp(temp->type, "D") || !strcmp(temp->type, "N"))
    {
        printf("[val = %d] \n", temp->val);
    }
    else if (!strcmp(temp->type, "M"))
    {
        printf("[inh = %d , val = %d] \n", temp->inh, temp->val);
    }
    else if (!strcmp(temp->type, "1"))
    {
        printf("[val = 1] \n");
    }
    else if (!strcmp(temp->type, "0"))
    {
        printf("[val = 0] \n");
    }
    else
    {
        printf("[]\n");
    }

    print_tree(temp->left, depth + 1);
    print_tree(temp->neither_left_nor_right, depth + 1);
    print_tree(temp->right, depth + 1);
    return;
}
int flag = 0;
void setatt(node *parent, node *child, int value)
{
    if (child == NULL || parent == NULL)
    {
        return;
    }
    if (!strcmp(child->type, "D"))
    {

        child->val = child->val;
        return;
    }
    else if (!strcmp(child->type, "1"))
    {
        child->val = 1;
        return;
    }
    else if (!strcmp(child->type, "0"))
    {
        child->val = 0;
        return;
    }
    else if (!strcmp(child->type, "T"))
    {

        child->inh = parent->inh;
        setatt(child, child->left, value);
        if (child->neither_left_nor_right != NULL)
        {
            setatt(child, child->neither_left_nor_right, value);
        }
    }
    else if (!strcmp(child->type, "X"))
    {
        flag = 1;
        if (child->right != NULL && !strcmp(child->right->type, "N"))
        {
            setatt(child, child->right, value);
        }
        return;
    }
    else if (!strcmp(child->type, "P"))
    {
        if (!strcmp(parent->type, "S"))
        {
            if (parent->left != NULL)
            {
                if (!strcmp(parent->left->type, "-"))
                {
                    child->inh = -1;
                }
                else if (!strcmp(parent->left->type, "+"))
                {
                    child->inh = 1;
                }
            }
        }
        else
        {
            if (parent->neither_left_nor_right != NULL && !strcmp(parent->neither_left_nor_right->type, "-"))
            {
                child->inh = -1;
            }
            else if (parent->neither_left_nor_right != NULL && !strcmp(parent->neither_left_nor_right->type, "+"))
            {
                child->inh = 1;
            }
        }
        setatt(child, child->left, value);
        if (child->right != NULL)
        {
            setatt(child, child->right, value);
        }
    }
    else if (!strcmp(child->type, "M"))
    {
        if (!strcmp(parent->type, "N"))
        {
            if (parent->left != NULL)
                child->inh = parent->left->val;
        }
        else if (!strcmp(parent->type, "M"))
        {
            if (parent->left != NULL)
                child->inh = parent->inh * 10 + parent->left->val;
        }
        setatt(child, child->left, value);
        if (child->neither_left_nor_right != NULL)
        {
            setatt(child, child->neither_left_nor_right, value);
        }
        if (child->neither_left_nor_right == NULL)
        {
            child->val = child->inh * 10 + child->left->val;
        }
        if (child->neither_left_nor_right != NULL)
        {
            child->val = child->neither_left_nor_right->val;
        }
    }
    else if (!strcmp(child->type, "N"))
    {
        setatt(child, child->left, value);
        if (child->neither_left_nor_right != NULL)
        {
            setatt(child, child->neither_left_nor_right, value);
        }
        if (!strcmp(child->left->type, "D") && child->neither_left_nor_right == NULL)
        {
            child->val = child->left->val;
        }
        else if (!strcmp(child->left->type, "D") && child->neither_left_nor_right != NULL)
        {
            child->val = child->neither_left_nor_right->val;
        }
        else
        {
            child->val = child->neither_left_nor_right->val;
        }
    }
}
int eval_poly(node *temp, int value)
{
    if (temp == NULL)
    {
        return 0;
    }
    if (!strcmp(temp->type, "X"))
    {
        if (temp->right != NULL)
        {

            int power = eval_poly(temp->right, value);
            int result = 1;
            for (int i = 1; i <= power; i++)
            {
                result *= value;
            }
            return result;
        }
        else
            return value;
    }
    else if (!strcmp(temp->type, "T"))
    {
        if (temp->neither_left_nor_right != NULL)
        {
            return temp->inh * eval_poly(temp->left, value) * eval_poly(temp->neither_left_nor_right, value);
        }
        else
        {
            return temp->inh * eval_poly(temp->left, value);
        }
    }
    else if (!strcmp(temp->type, "N"))
    {
        return temp->val;
    }
    else if (!strcmp(temp->type, "P"))
    {
        if (temp->right != NULL)
        {
            return eval_poly(temp->left, value) + eval_poly(temp->right, value);
        }
        else
        {
            return eval_poly(temp->left, value);
        }
    }
    else if (!strcmp(temp->type, "S"))
    {
        if (temp->neither_left_nor_right != NULL)
        {
            return eval_poly(temp->neither_left_nor_right, value);
        }
        else
            return eval_poly(temp->left, value);
    }
    else if (!strcmp(temp->type, "1"))
    {
        return 1;
    }
}
void printderivative(node *temp)
{
    if (temp == NULL)
    {
        return;
    }
    if (!strcmp(temp->type, "S"))
    {
        if (temp->neither_left_nor_right != NULL)
        {
            printderivative(temp->neither_left_nor_right);
        }
        else
        {
            printderivative(temp->left);
        }
    }
    else if (!strcmp(temp->type, "P"))
    {
        if (temp->right != NULL)
        {
            printderivative(temp->left);
            printderivative(temp->right);
        }
        else
        {
            printderivative(temp->left);
        }
    }
    else if (!strcmp(temp->type, "T"))
    {
        if (!strcmp(temp->left->type, "X"))
        {
            if (temp->left->right != NULL)
            {
                if (temp->inh == -1)
                {
                    printf(" - ");
                }
                else if (temp->inh == 1)
                {
                    printf(" + ");
                }
                printf("%d", temp->left->right->val);
                printf("x");
                if (temp->left->right->val != 2)
                {
                    printf("^");
                    printf("%d", temp->left->right->val - 1);
                }
            }
            else
            {
                if (temp->inh == -1)
                {
                    printf(" - ");
                }
                else if (temp->inh == 1)
                {
                    printf(" + ");
                }
                printf("1");
            }
        }
        else if (temp->neither_left_nor_right != NULL)
        {
            if (temp->neither_left_nor_right->right != NULL)
            {
                if (temp->inh == -1)
                {
                    printf(" - ");
                }
                else if (temp->inh == 1)
                {
                    printf(" + ");
                }
                printf("%d", temp->neither_left_nor_right->right->val * temp->left->val);
                printf("x");
                if (temp->neither_left_nor_right->right->val != 2)
                {
                    printf("^");
                    printf("%d", temp->neither_left_nor_right->right->val - 1);
                }
            }
            else
            {
                if (temp->inh == -1)
                {
                    printf(" - ");
                }
                else if (temp->inh == 1)
                {
                    printf(" + ");
                }
                printf("%d", temp->left->val);
            }
        }
    }
}
void free_node(node* root)
{
    if (root == NULL)
    {
        return;
    }
    free_node(root->left);
    free_node(root->neither_left_nor_right);
    free_node(root->right);
    free(root);
}
int main()
{
    yyparse();
    if (root->neither_left_nor_right != NULL)
        setatt(root, root->neither_left_nor_right, 0);
    else
        setatt(root, root->left, 0);
    print_tree(root, 0);
    printf("\n");
    for (int i = -5; i <= 5; i++)
    {
        printf("f(%d) = %d\n", i, eval_poly(root, i));
    }
    printf("\n");
    printf("f'(x) = ");
    if (flag == 0)
    {
        printf("0");
    }
    else
        printderivative(root);
    free_node(root);
    return 0;
}

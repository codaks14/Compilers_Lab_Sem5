#include <vector>
#include <string>
#include <fstream>
#include "lex.yy.c"

using namespace std;

vector<int> gotos;
int blkcnt = 1;

Symbol *Symbol_table::lookup(string name)
{
    for (auto &sym : table)
    {
        if (sym->name == name)
        {
            return sym;
        }
    }
    Symbol *s = new Symbol();
    s->name = name;
    s->value = "";
    s->loc = "-1";
    s->in_mem = 1;
    if (name[0] == '$')
    {
        s->in_mem = 0;
    }
    symtab.table.push_back(s);
    return s;
}

void print_symbol_table()
{
    for (auto &x : symtab.table)
    {
        cout << x->name << " " << x->value << endl;
    }
    cout << endl;
}

void Register::set_value(string val)
{
    value = val;
    is_free = 0;
    auto it = symtab.check(val);
    if (it != NULL && it->loc != "-1")
    {

        int flag = -1;
        for (int i = 0; i < regs.reg.size(); i++)
        {
            if (regs.reg[i].name == it->loc && regs.reg[i].name != name)
            {
                for (int i = 0; i < regs.reg[i].var_stored.size(); i++)
                {
                    string z = regs.reg[i].var_stored[i];
                    if (z == it->name)
                    {
                        flag = i;
                        break;
                    }
                }
            }
            if (flag != -1)
                break;
        }
        if (flag != -1)
        {
            vector<string> temp;
            for (int i = 0; i < regs.reg[flag].var_stored.size(); i++)
            {
                if (regs.reg[flag].var_stored[i] != it->name)
                {
                    temp.push_back(regs.reg[flag].var_stored[i]);
                }
            }
            regs.reg[flag].var_stored = temp;
            if (regs.reg[flag].var_stored.size() == 0)
            {
                regs.reg[flag].is_free = 1;
            }
            this->var_stored.push_back(val);
            it->loc = name;
            return;
        }
    }
    this->var_stored.push_back(val);
    if (it != NULL)
    {
        it->loc = name;
    }
    return;
}

void Register::free()
{

    for (auto &y : var_stored)
    {
        auto x = symtab.check(y);
        /* if(x==NULL)
        {
            continue;
        } */
        if (x->in_mem == 0 && x->name[0] != '$')
        {
            emit1("ST", x->name, name, "");
            x->in_mem = 1;
        }
        x->loc = "-1";
    }
    var_stored.clear();
    is_free = 1;
    value = "";
}

void Register_Vector::print_registers()
{
    for (auto &x : reg)
    {
        cout << x.name << " = " << x.value << endl;
    }
    cout << endl;
}

void Register_Vector::Initialize_registers(int num_reg)
{
    for (int i = 0; i < num_reg; i++)
    {
        Register r;
        r.is_free = 1;
        r.value = "";
        r.name = "R" + to_string(i + 1);
        regs.reg.push_back(r);
    }
}

Symbol *Symbol_table::check(string name)
{
    for (auto &x : table)
    {
        if (x->name == name)
        {
            return x;
        }
    }
    return NULL;
}

int Check_temp(int i, int regn)
{
    int cnt = 0;
    for (int j = 0; j < regs.reg[regn].var_stored.size(); j++)
    {
        string z = regs.reg[regn].var_stored[j];
        for (int k = i; k <= quads.quad.size(); k++)
        {
            if (isleader[k] == 1 && k != i)
            {
                break;
            }
            if (quads.quad[k - 1].arg1 == z || quads.quad[k - 1].arg2 == z || quads.quad[k - 1].result == z)
            {
                return -1;
            }
        }
    }
    return 1;
}

vector<int> isleader1;

int func(string s)
{
    int ans = 0;
    for (int i = 0; i < s.size(); i++)
    {
        ans = ans * 10 + (s[i] - '0');
    }
    return ans;
}

void backpatch1()
{
    int count = 1;
    for (auto &x : quads_target.quad)
    {
        if (x.op == "Block")
        {
            isleader1.push_back(count);
            continue;
        }
        count++;
    }
    int z = 0;
    for (int i = 0; i < quads_target.quad.size(); i++)
    {
        if (quads_target.quad[i].op == "JMP" || quads_target.quad[i].op == "JLT" || quads_target.quad[i].op == "JGT" || quads_target.quad[i].op == "JLE" || quads_target.quad[i].op == "JGE" || quads_target.quad[i].op == "JEQ" || quads_target.quad[i].op == "JNE")
        {
            string line1 = quads.quad[gotos[z] - 1].result;
            int line = func(line1);

            int flag = 0;
            int cnt = 0;
            z++;
            string ans = "-1";
            for (int j = 0; j < leaders.size() - 1; j++)
            {
                if (line >= leaders[j] && line < leaders[j + 1])
                {
                    ans = to_string(isleader1[cnt]);
                    flag = 1;
                    cnt++;
                    break;
                }
                cnt++;
            }
            if (line == quads.quad.size() + 1 && flag == 0)
            {
                ans = to_string(quads_target.quad.size() + 1 - blkcnt + 1);
                flag = 1;
            }
            if (flag == 0)
            {
                ans = to_string(isleader1[cnt]);
                cout << cnt << endl;
                flag = 1;
            }

            quads_target.quad[i].result = ans;
        }
    }
}

void Generate_Target_Code()
{
    int regcnt = 1;

    cout << endl;
    for (int ii = 0; ii < quads.quad.size(); ii++)
    {
        if (isleader[ii + 1] == 1)
        {
            for (int i = 0; i < regs.reg.size(); i++)
            {
                regs.reg[i].free();
            }
            emit1("Block", to_string(blkcnt), "", "");
            blkcnt++;
        }
        auto x = quads.quad[ii];
        /* cout<<x.result<<" "<<x.arg1<<" "<<x.op<<" "<<x.arg2<<endl; */
        string regA = "$", regT = "$";
        int regtno = -1;
        if (x.arg2 == "set")
        {
            if (symtab.check(x.arg1) != NULL)
            {
                if (symtab.check(x.arg1) != NULL && symtab.check(x.arg1)->loc != "-1")
                {

                    regA = symtab.check(x.arg1)->loc;
                    if (symtab.check(x.result) != NULL && symtab.check(x.result)->loc != "-1")
                    {
                        int flag = -1;
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            if (regs.reg[i].name == symtab.check(x.result)->loc)
                            {
                                flag = i;
                                break;
                            }
                        }
                        if (flag != -1)
                        {
                            vector<string> temp;
                            for (int i = 0; i < regs.reg[flag].var_stored.size(); i++)
                            {
                                if (regs.reg[flag].var_stored[i] != x.result)
                                {
                                    temp.push_back(regs.reg[flag].var_stored[i]);
                                }
                            }
                            regs.reg[flag].var_stored = temp;
                            if (regs.reg[flag].var_stored.size() == 0)
                            {
                                regs.reg[flag].is_free = 1;
                            }
                        }
                        symtab.check(x.result)->loc = regA;
                    }
                    symtab.check(x.arg1)->loc = regA;

                    // emit
                }
                else
                {
                    for (int i = 0; i < regs.reg.size(); i++) // point 2
                    {
                        if (regs.reg[i].is_free == 1)
                        {
                            regA = regs.reg[i].name;
                            regs.reg[i].set_value(x.arg1);
                            emit1("LD", regs.reg[i].name, x.arg1, "");
                            break;
                        }
                    }
                    if (regA == "$") // point3
                    {
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            int cnt = 0;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL && it->in_mem == 1 && z != x.arg2)
                                {
                                    cnt++;
                                }
                            }
                            if (cnt == regs.reg[i].var_stored.size())
                            {
                                regA = regs.reg[i].name;
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                regs.reg[i].var_stored.clear();
                                regs.reg[i].set_value(x.arg1);
                                emit1("LD", regs.reg[i].name, x.arg1, "");
                                break;
                            }
                        }
                    }
                    if (regA == "$")
                    {
                        // nahi aayega ye case (hopefully :p)
                    }
                    if (regA == "$") // point5
                    {
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            int cnt = 0;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                if (z[0] != '$')
                                {
                                    cnt++;
                                    break;
                                }
                            }
                            if (cnt == 0)
                            {
                                int regno = Check_temp(ii + 1, i);
                                if (regno != -1)
                                {
                                    for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                    {
                                        string z = regs.reg[i].var_stored[j];
                                        auto it = symtab.check(z);
                                        if (it != NULL)
                                        {
                                            it->loc = "-1";
                                        }
                                    }
                                    regs.reg[i].var_stored.clear();
                                    regA = regs.reg[i].name;
                                    regs.reg[i].set_value(x.arg1);
                                    emit1("LDI", regs.reg[i].name, x.arg1, "");
                                    break;
                                }
                            }
                        }
                    }
                    if (regA == "$") // point6
                    {
                        int min1 = 10000;
                        int minreg = -1;
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            int cnt = 0;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it->in_mem != 1)
                                {
                                    cnt++;
                                }
                            }
                            if (cnt < min1)
                            {
                                min1 = cnt;
                                minreg = i;
                            }
                        }
                        for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                        {
                            string z = regs.reg[minreg].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL)
                            {
                                it->loc = "-1";
                                if (it->in_mem == 0)
                                    emit1("ST", z, regs.reg[minreg].name, "");
                                it->in_mem = 1;
                            }
                        }
                        regs.reg[minreg].var_stored.clear();
                        regA = regs.reg[minreg].name;
                        regs.reg[minreg].set_value(x.arg1);
                        emit1("LD", regs.reg[minreg].name, x.arg1, "");
                    }
                }

                for (int i = 0; i < regs.reg.size(); i++)
                {
                    if (regs.reg[i].name == regA)
                    {
                        regs.reg[i].set_value(x.result);
                    }
                }
                if (symtab.check(x.result) != NULL)
                {
                    symtab.check(x.result)->loc = regA;
                    symtab.check(x.result)->in_mem = 0;
                }
            }
            else
            {
                if (symtab.check(x.result) != NULL && symtab.check(x.result)->loc != "-1") // point1
                {

                    regT = symtab.check(x.result)->loc;
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        if (regs.reg[i].name == regT)
                        {
                            regtno = i;
                            break;
                        }
                    }
                }
                else
                {

                    for (int i = 0; i < regs.reg.size(); i++) // point2
                    {
                        if (regs.reg[i].is_free == 1)
                        {

                            regtno = i;
                            // emit

                            break;
                        }
                    }
                    if (regtno == -1) // point3
                    {
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            int cnt = 0;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL && it->in_mem == 1 && z != x.arg1 && z != x.arg2)
                                {
                                    cnt++;
                                }
                            }
                            if (cnt == regs.reg[i].var_stored.size())
                            {
                                /* regT=regs.reg[i].name; */
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                /* regs.reg[i].var_stored.clear();
                                regs.reg[i].set_value(x.result); */
                                regtno = i;
                                // emit
                                break;
                            }
                        }
                    }
                    if (regtno == -1) // point4
                    {
                        // nahi aayega ye case (hopefully :p)
                    }
                    if (regtno == -1) // point5
                    {
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            int cnt = 0;
                            int flag1 = 0;

                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                if (z[0] != '$')
                                {
                                    cnt++;
                                    /* break; */
                                }
                                if (z == x.arg1 || z == x.arg2)
                                {
                                    flag1 = 1;
                                }
                            }
                            if (cnt==0 && flag1 == 0)
                            {
                                int regno = Check_temp(ii + 1, i);
                                if (regno != -1)
                                {
                                    for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                    {
                                        string z = regs.reg[i].var_stored[j];
                                        auto it = symtab.check(z);
                                        if (it != NULL)
                                        {
                                            it->loc = "-1";
                                        }
                                    }
                                    /* regs.reg[i].var_stored.clear();
                                    regT=regs.reg[i].name;
                                    regs.reg[i].set_value(x.result); */
                                    regtno = i;
                                    // emit
                                    break;
                                }
                            }
                        }
                    }
                    if (regtno == -1) // point6
                    {
                        int min1 = 10000;
                        int minreg = -1;
                        for (int i = 0; i < regs.reg.size(); i++)
                        {
                            int cnt = 0;
                            int flag1 = 0;

                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it->in_mem != 1)
                                {
                                    cnt++;
                                }
                            }
                            if (cnt < min1 && flag1 == 0)
                            {
                                min1 = cnt;
                                minreg = i;
                            }
                        }
                        // for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                        // {
                        //     string z = regs.reg[minreg].var_stored[j];
                        //     auto it = symtab.check(z);
                        //     if (it != NULL)
                        //     {
                        //         it->loc = "-1";
                        //         // it->in_mem = 1;
                        //     }
                        // }
                        regtno = minreg;
                    }
                }

                for (int i = 0; i < regs.reg[regtno].var_stored.size(); i++)
                {
                    string z = regs.reg[regtno].var_stored[i];
                    auto it = symtab.check(z);
                    if (it != NULL)
                    {
                        it->loc = "-1";
                        if (it->in_mem == 0)
                            emit1("ST", z, regs.reg[regtno].name, "");
                        it->in_mem = 1;
                    }
                }

                regs.reg[regtno].var_stored.clear();
                regs.reg[regtno].set_value(x.result);
                regT = regs.reg[regtno].name;
                emit1("LDI", regT, x.arg1, "");

                symtab.check(x.result)->in_mem = 0;
                symtab.check(x.result)->loc = regT;
            }
        }
        else if (x.op == "+" || x.op == "-" || x.op == "*" || x.op == "/" || x.op == "%")
        {
            string regA = "$", regB = "$"; // point1
            /* cout<<"Line no"<<ii+1<<endl; */
            if (symtab.check(x.arg1) == NULL)
                ;
            else if (symtab.check(x.arg1) != NULL && symtab.check(x.arg1)->loc != "-1")
            {
                regA = symtab.check(x.arg1)->loc;
                int flag = -1;
                for (int i = 0; i < regs.reg.size(); i++)
                {
                    if (regs.reg[i].name == symtab.check(x.arg1)->loc)
                    {
                        flag = i;
                        break;
                    }
                }
                if (flag != -1)
                {
                    vector<string> temp;
                    for (int i = 0; i < regs.reg[flag].var_stored.size(); i++)
                    {
                        if (regs.reg[flag].var_stored[i] != x.result)
                        {
                            temp.push_back(regs.reg[flag].var_stored[i]);
                        }
                    }
                    regs.reg[flag].var_stored = temp;
                    if (regs.reg[flag].var_stored.size() == 0)
                    {
                        regs.reg[flag].is_free = 1;
                    }
                }
                symtab.check(x.arg1)->loc = regA;
                // emit
            }
            else
            {
                for (int i = 0; i < regs.reg.size(); i++) // point 2
                {
                    if (regs.reg[i].is_free == 1)
                    {
                        regA = regs.reg[i].name;
                        regs.reg[i].set_value(x.arg1);
                        emit1("LD", regs.reg[i].name, x.arg1, "");
                        break;
                    }
                }
                if (regA == "$") // point3
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL && it->in_mem == 1 && z != x.arg2)
                            {
                                cnt++;
                            }
                        }
                        if (cnt == regs.reg[i].var_stored.size())
                        {
                            regA = regs.reg[i].name;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL)
                                {
                                    it->loc = "-1";
                                }
                            }
                            regs.reg[i].var_stored.clear();
                            regs.reg[i].set_value(x.arg1);
                            emit1("LD", regs.reg[i].name, x.arg1, "");
                            break;
                        }
                    }
                }
                if (regA == "$")
                {
                    // nahi aayega ye case (hopefully :p)
                }
                if (regA == "$") // point5
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            if (z[0] != '$')
                            {
                                cnt++;
                                break;
                            }
                        }
                        if (cnt == 0)
                        {
                            int regno = Check_temp(ii + 1, i);
                            if (regno != -1)
                            {
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                regs.reg[i].var_stored.clear();
                                regA = regs.reg[i].name;
                                regs.reg[i].set_value(x.arg1);
                                emit1("LD", regs.reg[i].name, x.arg1, "");
                                break;
                            }
                        }
                    }
                }
                if (regA == "$") // point6
                {
                    int min1 = 10000;
                    int minreg = -1;
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it->in_mem != 1)
                            {
                                cnt++;
                            }
                        }
                        if (cnt < min1)
                        {
                            min1 = cnt;
                            minreg = i;
                        }
                    }
                    for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                    {
                        string z = regs.reg[minreg].var_stored[j];
                        auto it = symtab.check(z);
                        if (it != NULL)
                        {
                            it->loc = "-1";
                            if (it->in_mem == 0)
                                emit1("ST", z, regs.reg[minreg].name, "");
                            it->in_mem = 1;
                        }
                    }
                    regs.reg[minreg].var_stored.clear();
                    regA = regs.reg[minreg].name;
                    regs.reg[minreg].set_value(x.arg1);
                    emit1("LD", regs.reg[minreg].name, x.arg1, "");
                }
            }
            // arg2
            if (symtab.check(x.arg2) == NULL)
                ;
            else if (symtab.check(x.arg2) != NULL && symtab.check(x.arg2)->loc != "-1") // point1
            {
                regB = symtab.check(x.arg2)->loc;
                int flag = -1;
                for (int i = 0; i < regs.reg.size(); i++)
                {
                    if (regs.reg[i].name == symtab.check(x.arg2)->loc)
                    {
                        flag = i;
                        break;
                    }
                }
                if (flag != -1)
                {
                    vector<string> temp;
                    for (int i = 0; i < regs.reg[flag].var_stored.size(); i++)
                    {
                        if (regs.reg[flag].var_stored[i] != x.result)
                        {
                            temp.push_back(regs.reg[flag].var_stored[i]);
                        }
                    }
                    regs.reg[flag].var_stored = temp;
                    if (regs.reg[flag].var_stored.size() == 0)
                    {
                        regs.reg[flag].is_free = 1;
                    }
                }
                symtab.check(x.arg2)->loc = regB;
            }
            else
            {

                for (int i = 0; i < regs.reg.size(); i++) // point2
                {
                    if (regs.reg[i].is_free == 1)
                    {
                        regB = regs.reg[i].name;
                        regs.reg[i].set_value(x.arg2);
                        emit1("LD", regs.reg[i].name, x.arg2, "");

                        break;
                    }
                }
                if (regB == "$") // point3
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL && it->in_mem == 1 && z != x.arg1)
                            {
                                cnt++;
                            }
                        }
                        if (cnt == regs.reg[i].var_stored.size())
                        {
                            regB = regs.reg[i].name;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL)
                                {
                                    it->loc = "-1";
                                }
                            }
                            regs.reg[i].var_stored.clear();
                            regs.reg[i].set_value(x.arg2);
                            emit1("LD", regs.reg[i].name, x.arg2, "");
                            break;
                        }
                    }
                }
                if (regB == "$") // point4
                {
                    // nahi aayega ye case (hopefully :p)
                }
                if (regB == "$") // point5
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        int flag1 = 0;

                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            if (z[0] != '$')
                            {
                                cnt++;
                                /* break; */
                            }
                            if (z == x.arg1)
                            {
                                flag1 = 1;
                            }
                        }
                        if (cnt == 0 && flag1 == 0)
                        {
                            int regno = Check_temp(ii + 1, i);
                            if (regno != -1)
                            {
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                regs.reg[i].var_stored.clear();
                                regB = regs.reg[i].name;
                                regs.reg[i].set_value(x.arg2);
                                emit1("LD", regs.reg[i].name, x.arg2, "");
                                break;
                            }
                        }
                    }
                }
                if (regB == "$") // point6
                {
                    int min1 = 10000;
                    int minreg = -1;
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        int flag1 = 0;

                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it->in_mem != 1)
                            {
                                cnt++;
                            }
                            if (z == x.arg1)
                            {
                                flag1 = 1;
                            }
                        }
                        if (cnt < min1 && flag1 == 0)
                        {
                            min1 = cnt;
                            minreg = i;
                        }
                    }
                    for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                    {
                        string z = regs.reg[minreg].var_stored[j];
                        auto it = symtab.check(z);
                        if (it != NULL)
                        {
                            it->loc = "-1";
                            if (it->in_mem == 0)
                                emit1("ST", z, regs.reg[minreg].name, "");
                            it->in_mem = 1;
                        }
                    }
                    regs.reg[minreg].var_stored.clear();
                    regB = regs.reg[minreg].name;
                    regs.reg[minreg].set_value(x.arg2);
                    emit1("LD", regs.reg[minreg].name, x.arg2, "");
                }
            }

            // result

            string regT = "$";
            int regtno = -1;
            if (symtab.check(x.result) != NULL && symtab.check(x.result)->loc != "-1") // point1
            {
                regT = symtab.check(x.result)->loc;
                for (int i = 0; i < regs.reg.size(); i++)
                {
                    if (regs.reg[i].name == regT)
                    {
                        regtno = i;
                        break;
                    }
                }
            }
            else
            {

                for (int i = 0; i < regs.reg.size(); i++) // point2
                {
                    if (regs.reg[i].is_free == 1)
                    {
                        regtno = i;
                        // emit
                        break;
                    }
                }
                if (regtno == -1) // point3
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL && it->in_mem == 1 && z != x.arg1 && z != x.arg2)
                            {
                                cnt++;
                            }
                        }
                        if (cnt == regs.reg[i].var_stored.size())
                        {
                            /* regT=regs.reg[i].name; */
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL)
                                {
                                    it->loc = "-1";
                                }
                            }
                            /* regs.reg[i].var_stored.clear();
                            regs.reg[i].set_value(x.result); */
                            regtno = i;
                            // emit
                            break;
                        }
                    }
                }
                if (regtno == -1) // point4
                {
                    // nahi aayega ye case (hopefully :p)
                }
                if (regtno == -1) // point5
                {

                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        int flag1 = 0;

                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            if (z[0] != '$')
                            {
                                cnt++;
                                /* break; */
                            }
                            if (z == x.arg1 || z == x.arg2)
                            {
                                flag1 = 1;
                            }
                        }
                        if (cnt == 0 && flag1 == 0)
                        {

                            int regno = Check_temp(ii + 1, i);

                            if (regno != -1)
                            {
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                regtno = i;

                                break;
                            }
                        }
                    }
                }
                if (regtno == -1) // point6
                {
                    int min1 = 10000;
                    int minreg = -1;
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        /* cout<<"silemt"<<endl; */

                        int cnt = 0;
                        int flag1 = 0;

                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL && it->in_mem != 1)
                            {
                                cnt++;
                            }
                        }
                        if (cnt < min1 && flag1 == 0)
                        {
                            min1 = cnt;
                            minreg = i;
                        }
                    }
                    /* cout<<min1<<" "<<minreg<<endl; */
                    // for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                    // {
                    //     string z = regs.reg[minreg].var_stored[j];
                    //     auto it = symtab.check(z);
                    //     if (it != NULL)
                    //     {
                    //         it->loc = "-1";
                    //         // it->in_mem = 1;
                    //     }
                    // }
                    regtno = minreg;
                }
            }
            for (int i = 0; i < regs.reg[regtno].var_stored.size(); i++)
            {
                string z = regs.reg[regtno].var_stored[i];
                auto it = symtab.check(z);
                if (it != NULL)
                {
                    it->loc = "-1";
                    if (it->in_mem == 0)
                        emit1("ST", z, regs.reg[regtno].name, "");

                    it->in_mem = 1;
                }
            }

            /* cout<<regtno<<endl; */
            regs.reg[regtno].var_stored.clear();
            regs.reg[regtno].set_value(x.result);
            regT = regs.reg[regtno].name;

            string operation;
            if (x.op == "+")
            {
                operation = "ADD";
            }
            else if (x.op == "-")
            {
                operation = "SUB";
            }
            else if (x.op == "*")
            {
                operation = "MUL";
            }
            else if (x.op == "/")
            {
                operation = "DIV";
            }
            else if (x.op == "%")
            {
                operation = "REM";
            }
            if (symtab.check(x.arg1) == NULL && symtab.check(x.arg2) == NULL)
            {
                emit1("LDI", regT, x.arg1, "");
                emit1(operation, regT, regT, x.arg2);
            }
            else if (symtab.check(x.arg1) != NULL && symtab.check(x.arg2) == NULL)
            {
                emit1(operation, regT, regA, x.arg2);
            }
            else if (symtab.check(x.arg1) == NULL && symtab.check(x.arg2) != NULL)
            {
                emit1(operation, regT, x.arg1, regB);
            }
            else
            {

                emit1(operation, regT, regA, regB);
            }

            symtab.check(x.result)->in_mem = 0;
            symtab.check(x.result)->loc = regT;
        }
        else if (x.op == "==" || x.op == "!=" || x.op == ">=" || x.op == ">" || x.op == "<" || x.op == "<=")
        {
            string regA = "$", regB = "$"; // point1
            /* cout<<"Line no"<<ii+1<<endl; */
            if (symtab.check(x.arg1) == NULL)
                ;
            else if (symtab.check(x.arg1) != NULL && symtab.check(x.arg1)->loc != "-1")
            {
                regA = symtab.check(x.arg1)->loc;
                // emit
            }
            else
            {
                for (int i = 0; i < regs.reg.size(); i++) // point 2
                {
                    if (regs.reg[i].is_free == 1)
                    {
                        regA = regs.reg[i].name;
                        regs.reg[i].set_value(x.arg1);
                        emit1("LD", regs.reg[i].name, x.arg1, "");
                        break;
                    }
                }
                if (regA == "$") // point3
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL && it->in_mem == 1 && z != x.arg2)
                            {
                                cnt++;
                            }
                        }
                        if (cnt == regs.reg[i].var_stored.size())
                        {
                            regA = regs.reg[i].name;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL)
                                {
                                    it->loc = "-1";
                                }
                            }
                            regs.reg[i].var_stored.clear();
                            regs.reg[i].set_value(x.arg1);
                            emit1("LD", regs.reg[i].name, x.arg1, "");
                            break;
                        }
                    }
                }
                if (regA == "$") // point5
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            if (z[0] != '$')
                            {
                                cnt++;
                                break;
                            }
                        }
                        if (cnt == 0)
                        {
                            int regno = Check_temp(ii + 1, i);
                            if (regno != -1)
                            {
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                regs.reg[i].var_stored.clear();
                                regA = regs.reg[i].name;
                                regs.reg[i].set_value(x.arg1);
                                emit1("LD", regs.reg[i].name, x.arg1, "");
                                break;
                            }
                        }
                    }
                }
                if (regA == "$") // point6
                {
                    int min1 = 10000;
                    int minreg = -1;
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it->in_mem != 1)
                            {
                                cnt++;
                            }
                        }
                        if (cnt < min1)
                        {
                            min1 = cnt;
                            minreg = i;
                        }
                    }
                    for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                    {
                        string z = regs.reg[minreg].var_stored[j];
                        auto it = symtab.check(z);
                        if (it != NULL)
                        {
                            it->loc = "-1";
                            if (it->in_mem == 0)
                                emit1("ST", z, regs.reg[minreg].name, "");
                            it->in_mem = 1;
                        }
                    }
                    regs.reg[minreg].var_stored.clear();
                    regA = regs.reg[minreg].name;
                    regs.reg[minreg].set_value(x.arg1);
                    emit1("LD", regs.reg[minreg].name, x.arg1, "");
                }
            }
            // arg2
            if (symtab.check(x.arg2) == NULL)
                ;
            else if (symtab.check(x.arg2) != NULL && symtab.check(x.arg2)->loc != "-1") // point1
            {
                regB = symtab.check(x.arg2)->loc;
            }
            else
            {

                for (int i = 0; i < regs.reg.size(); i++) // point2
                {
                    if (regs.reg[i].is_free == 1)
                    {
                        regB = regs.reg[i].name;
                        regs.reg[i].set_value(x.arg2);
                        emit1("LD", regs.reg[i].name, x.arg2, "");

                        break;
                    }
                }
                if (regB == "$") // point3
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it != NULL && it->in_mem == 1 && z != x.arg1)
                            {
                                cnt++;
                            }
                        }
                        if (cnt == regs.reg[i].var_stored.size())
                        {
                            regB = regs.reg[i].name;
                            for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                            {
                                string z = regs.reg[i].var_stored[j];
                                auto it = symtab.check(z);
                                if (it != NULL)
                                {
                                    it->loc = "-1";
                                }
                            }
                            regs.reg[i].var_stored.clear();
                            regs.reg[i].set_value(x.arg2);
                            emit1("LD", regs.reg[i].name, x.arg2, "");
                            break;
                        }
                    }
                }
                if (regB == "$") // point5
                {
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        int flag1 = 0;

                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            if (z[0] != '$')
                            {
                                cnt++;
                                /* break; */
                            }
                            if (z == x.arg1)
                            {
                                flag1 = 1;
                            }
                        }
                        if (cnt==0 && flag1 == 0)
                        {
                            int regno = Check_temp(ii + 1, i);
                            if (regno != -1)
                            {
                                for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                                {
                                    string z = regs.reg[i].var_stored[j];
                                    auto it = symtab.check(z);
                                    if (it != NULL)
                                    {
                                        it->loc = "-1";
                                    }
                                }
                                regs.reg[i].var_stored.clear();
                                regB = regs.reg[i].name;
                                regs.reg[i].set_value(x.arg2);
                                emit1("LD", regs.reg[i].name, x.arg2, "");
                                break;
                            }
                        }
                    }
                }
                if (regB == "$") // point6
                {
                    int min1 = 10000;
                    int minreg = -1;
                    for (int i = 0; i < regs.reg.size(); i++)
                    {
                        int cnt = 0;
                        int flag1 = 0;

                        for (int j = 0; j < regs.reg[i].var_stored.size(); j++)
                        {
                            string z = regs.reg[i].var_stored[j];
                            auto it = symtab.check(z);
                            if (it->in_mem != 1)
                            {
                                cnt++;
                            }
                            if (z == x.arg1)
                            {
                                flag1 = 1;
                            }
                        }
                        if (cnt < min1 && flag1 == 0)
                        {
                            min1 = cnt;
                            minreg = i;
                        }
                    }
                    for (int j = 0; j < regs.reg[minreg].var_stored.size(); j++)
                    {
                        string z = regs.reg[minreg].var_stored[j];
                        auto it = symtab.check(z);
                        if (it != NULL)
                        {
                            it->loc = "-1";
                            if (it->in_mem == 0)
                                emit1("ST", z, regs.reg[minreg].name, "");
                            it->in_mem = 1;
                        }
                    }
                    regs.reg[minreg].var_stored.clear();
                    regB = regs.reg[minreg].name;
                    regs.reg[minreg].set_value(x.arg2);
                    emit1("LD", regs.reg[minreg].name, x.arg2, "");
                }
            }
            string oper;
            if (x.op == "==")
            {
                oper = "JNE";
            }
            else if (x.op == "!=")
            {
                oper = "JEQ";
            }
            else if (x.op == ">=")
            {
                oper = "JLT";
            }
            else if (x.op == ">")
            {
                oper = "JLE";
            }
            else if (x.op == "<=")
            {
                oper = "JGT";
            }
            else if (x.op == "<")
            {
                oper = "JGE";
            }

            for (int i = 0; i < regs.reg.size(); i++)
            {
                regs.reg[i].free();
            }

            if (symtab.check(x.arg1) == NULL && symtab.check(x.arg2) == NULL)
            {
                emit1("LDI", regT, x.arg1, "");
                emit1(oper, x.result, x.arg1, x.arg2);
            }
            else if (symtab.check(x.arg1) != NULL && symtab.check(x.arg2) == NULL)
            {
                emit1(oper, x.result, regA, x.arg2);
            }
            else if (symtab.check(x.arg1) == NULL && symtab.check(x.arg2) != NULL)
            {
                emit1(oper, x.result, x.arg1, regB);
            }
            else
            {
                emit1(oper, x.result, regA, regB);
            }
            gotos.push_back(ii + 1);
        }
        else if (x.op == "goto")
        {
            for (int i = 0; i < regs.reg.size(); i++)
            {
                regs.reg[i].free();
            }

            emit1("JMP", x.result, "", "");
            gotos.push_back(ii + 1);
        }
    }
    backpatch1();
    for (int i = 0; i < regs.reg.size(); i++)
    {
        regs.reg[i].free();
    }
}

void emit1(string op, string result, string arg1, string arg2 = "")
{

    Quad_element e;
    e.op = op;
    e.result = result;
    e.arg1 = arg1;
    e.arg2 = arg2;
    quads_target.quad.push_back(e);
}

void print_quad_table1()
{
    // Output to target.txt
    ofstream cout("target.txt");

    cout << endl;
    cout << "Target code" << endl;
    cout << endl;
    int count = 1;
    for (auto &x : quads_target.quad)
    {
        if (x.op == "Block")
        {
            cout << endl;
            cout << x.op << " " << x.result << endl;
            isleader1.push_back(count);
            continue;
        }
        cout << count << " : ";
        count++;
        if (x.op == "ST")
        {
            cout << x.op << " " << x.result << " " << x.arg1 << endl;
        }
        else if (x.op == "LDI")
        {
            cout << x.op << " " << x.result << " " << x.arg1 << endl;
        }
        else if (x.op == "LD")
        {
            cout << x.op << " " << x.result << " " << x.arg1 << endl;
        }
        else if (x.op == "ADD" || x.op == "SUB" || x.op == "MUL" || x.op == "DIV" || x.op == "REM")
        {
            cout << x.op << " " << x.result << " " << x.arg1 << " " << x.arg2 << endl;
        }
        else if (x.op == "JMP")
        {
            cout << x.op << " " << x.result << endl;
        }
        else
        {
            cout << x.op << " " << x.arg1 << " " << x.arg2 << " " << x.result << endl;
        }
    }

    cout.close();
}

int main()
{
    yyparse();
    // print_symbol_table();
    check_leader();
    print_quad_table();
    int num_reg;
    num_reg = 5;
    regs.Initialize_registers(num_reg);
    Generate_Target_Code();
    print_quad_table1();
}
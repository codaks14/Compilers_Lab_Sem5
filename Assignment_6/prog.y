%{
    #include<iostream>
    #include <vector>
    using namespace std;
    void yyerror(const char *s);
    int yylex();
    int yyparse();  

class Statement
{
    public:
    int nextlist;
};

vector<int> leaders;

class Expression
{
    public:
    string name;
};

class Bool
{
    public:
    int truelist;
    int falselist;
};

class Quad_element
{
    public:
    string op;
    string arg1;
    string arg2;
    string result;
};

class Quad_Vector
{
    public:
    vector<Quad_element> quad;

};

class Symbol
{
    public:
    string name;
    string value;
    string loc;
    int in_mem;
    
    void st_into_reg(string name);

};

class Register
{
   public:
   int is_free=1;
   string value;
   string name;
   vector<string> var_stored;

   void set_value(string val);
   void free();
   void print_register();
};

class Register_Vector
{
    public:
    vector<Register> reg;
    void print_registers();
    void Initialize_registers(int num_reg);
};

class Symbol_table
{
    public:
    vector<Symbol *> table;
    Symbol *lookup(string name);
    Symbol *check(string name);

};

void emit(string op,string result,string arg1,string arg2);
void emit1(string op,string result,string arg1,string arg2);
void backpatch(int index,int z);
void print_quad_table();
void print_quad_table1();
vector<int> isleader;
Symbol_table symtab;
Register_Vector regs;

Quad_Vector quads;
Quad_Vector quads_target;
int quadindex=1;
int tempcnt=0;





%}



%union {
    int num;
    char *str;
    Statement *statement;
    Bool *boo;
    Expression *expre;
}



%token <num> NUMB
%token <str> IDEN
%token SET WHEN LOOP WHILE LP RP PLUS MINUS MUL DIV MOD ASSIGN NE LT LE GT GE
%type <statement> list stmt asgn cond loop
%type <num> reln oper M
%type <expre> expr atom 
%type <boo> bool

%%

program: list
        {

        }    
        ;

list : stmt
        {

        }
        | stmt list
        {

        }
        ;

stmt : asgn
        {

        }
        | cond
        {

        }
        | loop
        {

        }
        ;

asgn : LP SET IDEN atom RP
        {
            symtab.lookup($3);
            $$= new Statement();
            emit("=", $3,$4->name,"set");
        }
        ;

cond : LP WHEN bool list RP
        {
            $$= new Statement();
            backpatch($3->falselist, quadindex);

        }
        ;

loop : LP LOOP WHILE M bool list RP
        {
            $$= new Statement();
            emit("goto",to_string($4),"","");
            backpatch($5->falselist, quadindex);
        }
        ;

expr : LP oper atom atom RP
        {
            $$= new Expression();
            tempcnt++;
            string name="$"+to_string(tempcnt);
            symtab.lookup(name);
            $$->name=name;
            emit(to_string($2),name,$3->name,$4->name);
        }
        ;

bool : LP reln atom atom RP
        {
            $$=new Bool();
            $$->falselist=quads.quad.size()+1;
            emit(to_string($2),"",$3->name,$4->name);

        }
        ;

atom : IDEN 
        {
            symtab.lookup($1);
            $$= new Expression();
            $$->name=$1;
        }
        | NUMB
        {
            $$=new Expression();
            $$->name=to_string($1);
        }
        | expr
        {
            $$=new Expression();
            $$->name=$1->name;
        }
        ;

oper : PLUS
        {
            $$=PLUS;
        }
        | MINUS
        {
            $$=MINUS;
        }
        | MUL
        {
            $$=MUL;
        }
        | DIV
        {
            $$=DIV;
        }
        | MOD
        {
            $$=MOD;
        }
        ;

reln : ASSIGN
        {
           $$=ASSIGN; 
        }
        | NE
        {
            $$=NE;
        }
        | LT
        {
            $$=LT;
        }
        | LE
        {
            $$=LE;
        }
        | GT
        {
            $$=GT;
        }
        | GE
        {
            $$=GE;
        }
        ;

M :  {
        $$=quadindex;
     }
     ;





%%

void emit(string op,string result,string arg1,string arg2="")
{
    Quad_element q;
    if(op==to_string(NE))
    {
        op="!=";
    }
    if(op==to_string(LT))
    {
        op="<";
    }
    if(op==to_string(LE))
    {
        op="<=";
    }
    if(op==to_string(GT))
    {
        op=">";
    }
    if(op==to_string(GE))
    {
        op=">=";
    }
    if(op==to_string(PLUS))
    {
        op="+";
    }
    if(op==to_string(MINUS))
    {
        op="-";
    }
    if(op==to_string(MUL))
    {
        op="*";
    }
    if(op==to_string(DIV))
    {
        op="/";
    }
    if(op==to_string(MOD))
    {
        op="%";
    }
    if(op==to_string(ASSIGN))
    {
        op="==";
    }

    q.op=op;
    q.result=result;
    q.arg1=arg1;
    q.arg2=arg2;
    quads.quad.push_back(q);
    quadindex++;
}

void backpatch(int index,int z)
{
    string s=to_string(z);
    quads.quad[index-1].result=s;
}


void print_quad_table()
{
    // Output to intcode.txt
    ofstream cout("intcode.txt");

    cout<<"3-address-code"<<endl;
    int count=1;
    int count2=1;
    string op;
    for(auto &quad:quads.quad)
    {
        if(isleader[count]==1)
        {
            cout<<endl;
            cout<<"Block "<<count2<<endl;
            leaders.push_back(count);
            count2++;
        }
        cout<<count<<" : ";
        count++;
        
        if(quad.arg2=="set")
        {
            cout<<quad.result<<" = "<<quad.arg1<<endl;
        }
        else if(quad.op=="goto")
        {
            cout<<"goto "<<quad.result<<endl;
        }
        else if(quad.op=="!=" || quad.op=="<" || quad.op=="<=" || quad.op==">" || quad.op==">=" || quad.op=="==" )
        {
            cout<<"ifFalse ("<<quad.arg1<<" "<<quad.op<<" "<<quad.arg2<<")"<<" goto "<<quad.result<<endl;
        }
        else{
            cout<<quad.result<<" = "<<quad.arg1<<" "<<quad.op<<" "<<quad.arg2<<endl;
        }
    }
    cout.close();
}

void check_leader()
{
    isleader.resize(quads.quad.size()+2,0);
    isleader[1]=1;
    for(int i=0;i<quads.quad.size();i++)
    {
        if(quads.quad[i].op=="goto" || quads.quad[i].op=="!=" || quads.quad[i].op=="<" || quads.quad[i].op=="<=" || quads.quad[i].op==">" || quads.quad[i].op==">=" || quads.quad[i].op=="==")
        {
            isleader[i+2]=1;
            isleader[stoi(quads.quad[i].result)]=1;
        }
    }
}


void yyerror(const char *s)
{
    fprintf(stderr, "%s parsing %s at %d!\n", s, yytext, yylineno);
}
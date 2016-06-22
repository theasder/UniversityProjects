#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <stack>
#include <string>
#include <map>
#include <string.h>
#include <cmath>

using namespace std;

enum type_of_val {NUL, NUMERIC, STRING, LOGICAL, STRINGCONST, OPERATION, VARIABLE};

class Ident;

class Poliz
{
public:
    map<string, Ident> id;
    vector<Ident> pol;
    stack<Ident> ex;
    vector<Ident> print;
    
    int pos;
    
    Poliz() : pos(0) {}
    
    Poliz(const Poliz &p) : id(p.id), pol(p.pol), ex(p.ex), print(p.print), pos(p.pos) {}
    
    Poliz(const Poliz &p, int p1, int p2);
    
    void Clear()
    {
        pos = 0;
        while(ex.size()) ex.pop();
    }
};

class Lex
{
public:
    type_of_val type;
    // all possible types for lexeme
    char c;
    bool LogValue;
    double NumValue;
    string StrValue;
    string StrConst;
    
    int nlinks;
    
    Lex(type_of_val t_lex = NUL, void *v_lex = NULL) : c(0), LogValue(false), NumValue(0), StrValue(""), StrConst(""), nlinks(1)
    {
        // assigning lexeme
        type = t_lex;
        if(! v_lex)
            return;
        
        switch (t_lex) {
            case LOGICAL:
                LogValue = *((bool*) v_lex);
                break;
            case NUMERIC:
                NumValue = *((double*) v_lex);
                break;
            case STRINGCONST:
                StrConst = *((string*) v_lex);
                break;
            case STRING:
                StrValue = *((string*) v_lex);
                break;
            case OPERATION:
                c = *((char *) v_lex);
            default:
                break;
        }
    }
    
    Lex(const Lex &L) : type(L.type), c(L.c), LogValue(L.LogValue), NumValue(L.NumValue), StrValue(L.StrValue), StrConst(L.StrConst), nlinks(1)
    {}
    
    Lex& operator = (const Lex & L)
    {
        nlinks = 1;
        
        type = L.type;
        c = L.c;
        LogValue = L.LogValue;
        NumValue = L.NumValue;
        StrValue = L.StrValue;
        StrConst = L.StrConst;
        
        return (*this);
    }
    
    string ToStr()
    {
        // converting variable to string
        char oper[3], num[256];
        
        switch(type)   {
            case STRING:
                return StrValue;
                
            case STRINGCONST:
                return StrConst;
                
            case OPERATION: {
                oper[0] = c;
                oper[1] = 0;
                return string(oper);
            }
                
            case NUMERIC:   {
                size_t len;
                sprintf(num, "%lf", NumValue);
                
                // minimizing length of string
                
                for(len = strlen(num); len > 1 && num[len - 1] == '0'; len--)
                    ;
                
                if(num[len - 1] == '.') len--;
                
                num[len] = '\0';
                return string(num);
            }
                
            case LOGICAL:
                if(LogValue)
                    return string("TRUE");
                else return string("FALSE");
                
            case NUL:
                return string("NULL");
                
            default:
                return string("error");
        }
    }
    
    bool ToBool()
    {
        switch (type) {
            case LOGICAL:
                return LogValue;
            case NUMERIC:
                return NumValue != 0;
            default:
                throw string("Error: Unable convert to Logic value\n");
        }
    }
    
    double ToNum()
    {
        switch (type) {
            case LOGICAL:
                return (int)LogValue;
            case NUMERIC:
                return NumValue;
            default:
                throw string("Error : Unable convert to Numeric value\n");
        }
    }
};

class Ident
{
public:
    string name;
    type_of_val type;
    
    vector<Lex*> val;
    
    bool arg;
    
    Ident(int n, type_of_val t_lex, string str = "") : type(t_lex), arg(false), name(str)
    {
        val.resize(n);
        for(int i = 0; i < n; i++)
            val[i] = new Lex(t_lex);
    }
    
    Ident(Lex *L, type_of_val t_lex = NUL, string str = "") : name(str), type(t_lex), arg(false)
    {
        val.clear();
        val.push_back(L);
    }
    
    Ident() : name(""), type(NUL), arg(false)
    {
        val.clear();
    }
    
    Ident(const Ident &I) : name(I.name), type(I.type)
    {
        arg = I.arg;
        
        val.resize(I.val.size(), NULL);
        
        for(size_t i = 0; i < val.size(); i++)   {
            val[i] = I.val[i];
            val[i] -> nlinks++;
        }
    }
    
    Ident& operator = (const Ident &I)
    {
        name = I.name;
        arg = I.arg;
        
        type = I.type;
        
        for(size_t i = 0; i < val.size(); i++)
            if(--(val[i] -> nlinks) < 1)
                delete val[i];
        
        val.clear();
        val.resize(I.val.size());
        
        for(size_t i = 0; i < val.size(); i++)
            val[i] = new Lex(*I.val[i]);
        
        return (*this);
    }
    
    void ToStr()
    {
        type = STRINGCONST;
        for(size_t i = 0; i < val.size(); i++)
            if(val[i] -> type != NUL) {
                val[i] -> type = STRINGCONST;
                val[i] -> StrConst = val[i] -> ToStr();
            }
    }
    
    void ToNum()
    {
        type = NUMERIC;
        for(size_t i = 0; i < val.size(); i++)
            if(val[i] -> type != NUL)   {
                val[i] -> type = NUMERIC;
                val[i] -> NumValue = val[i] -> ToNum();
            }
    }
    
    void ToBool()
    {
        type = LOGICAL;
        for(size_t i = 0; i < val.size(); i++)
            if(val[i] -> type != NUL) {
                val[i] -> type = LOGICAL;
                val[i] -> LogValue = val[i] -> ToBool();
            }
    }
    
    ~Ident()
    {
        for(size_t i = 0; i < val.size(); i++)
            if(--(val[i] -> nlinks) < 1)
                delete val[i];
    }
};

Poliz::Poliz(const Poliz &p, int p1, int p2)
{
    pos = 0;
    pol.resize(p2 - p1);
    for(int i = p1; i < p2; i++)
        pol[i - p1] = p.pol[i];
    
    id.clear();
    print.clear();
    while(ex.size() > 0) ex.pop();
}


class Execution
{
public:
    
    static void BinaryOp(Poliz &p, Ident &I1, Ident &I2, Ident &I3, string c, type_of_val type, bool StrAvalible, bool NewIdent);
    static void UnaryOp(Poliz &p, Ident &I1, Ident &I2, string c, type_of_val type, bool StrAvalible);
    
    static void OpGoTo(Poliz&);
    static void OpAssign(Poliz&);
    static void OpAnd(Poliz&);
    static void OpOr(Poliz&);
    static void OpNo(Poliz&);
    static void OpLess(Poliz&);
    static void OpGreat(Poliz&);
    static void OpLessEq(Poliz&);
    static void OpGreatEq(Poliz&);
    static void OpEq(Poliz&);
    static void OpNoEq(Poliz&);
    static void OpNeg(Poliz&);
    static void OpPlus(Poliz&);
    static void OpMinus(Poliz&);
    static void OpMul(Poliz&);
    static void OpDiv(Poliz&);
    static void OpVect(Poliz&);
    static void OpPrint(Poliz&);
    static void OpCall(Poliz&);
    static void OpC(Poliz&, vector<Ident>&);
    static void OpMode(Poliz&, vector<Ident>&);
    static void OpLength(Poliz&, vector<Ident>&);
    static void OpInd(Poliz&);
    
    static void Execute(Poliz&);
};

class Scanner
{
public:
    char c;
    string buf;
    bool console;
    
    char ReadStr[1000];
    size_t pos, len;
    
    bool read;
    
    FILE *f;
    
    Scanner(const char *prog = NULL): console(false) {
        if(prog == NULL)
        {
            f = stdin;
            console = true;
        }
        else f = fopen(prog, "r");
        
        read = true;
        pos = len = 0;
    }
    
    void NextChar()
    {
        if(pos == len)   {
            fgets(ReadStr, 256, f);
            pos = 0;
            len = strlen(ReadStr);
        }
        
        c = ReadStr[pos];
        pos++;
    }
    
    Lex DoubleOperation(char c1, char nc1, char c2, char nc2);
    Lex GetLex();
};


class Parser
{
public:
    Lex CurrLex;
    Scanner scan;
    bool console;
    
    queue<Lex> LexList;
    
    Poliz prog;
    
    Parser(const char *prog = NULL) : scan(prog) , console(false)
    {
        if(prog == NULL)   {
            console = true;
            cout << ">";
        }
    }
    
    void Continue()
    {
        prog.print.clear();
        prog.pol.clear();
        while(prog.ex.size() > 0) prog.ex.pop();
        prog.pos = 0;
        
        while(CurrLex.c != '\n')
            NextLex(false);
        
        cout << ">";
    }
    
    void NextLex(bool WaitSth = true)
    {
        if(LexList.size() > 0)   {
            CurrLex = LexList.front();
            LexList.pop();
        }
        else CurrLex = scan.GetLex();
        
        if(WaitSth && CurrLex.c == '\n')   {
            if(console) cout << "+";
            NextLex(WaitSth);
        }
    }
    
    void Push(char c)
    {
        prog.pol.push_back(Ident(new Lex(OPERATION, &c), OPERATION));
    }
    
    void Push(int x)
    {
        double temp = x;
        prog.pol.push_back(Ident(new Lex(NUMERIC, &temp), NUMERIC));
    }
    
    void Push(Ident I)
    {
        prog.pol.push_back(I);
    }
    
    void Program(bool);
    void Exp0(bool);
    void Exp(bool);
    void Exp1(bool);
    void Exp2(bool);
    void Exp3(bool);
    void Exp4(bool);
    void Exp5(bool);
    void Exp6(bool);
    void Exp7(bool);
    void Exp8(bool);
    void Var();
    void IndOrFunc();
    int ArgList(bool);
    void ArgListItem(bool);
};

Lex Scanner::DoubleOperation(char c1, char nc1, char c2 = '~', char nc2 = '~')
{
    char temp;
    if(pos < len)
        temp = ReadStr[pos];
    else return Lex(OPERATION, &c);
    
    if(temp == c1)   {
        NextChar();
        NextChar();
        return Lex(OPERATION, &nc1);
    }
    else if(temp == c2)   {
        NextChar();
        NextChar();
        return Lex(OPERATION, &nc2);
    }
    
    temp = c;
    NextChar();
    return Lex(OPERATION, &temp);
}

Lex Scanner::GetLex()
{
    char prev_c = '\0';
    if(read) {prev_c = c; NextChar(); read = false;}
    
    buf = "";
    
    while ( c == ' ' || c == '\t' || c == '\r')
    {
        prev_c = c;
        NextChar();
    }
    
    if(c == '.' || c == '_' || isalpha(c) )
    {
        buf += c;
        prev_c = c;
        NextChar();
        while(c == '.' || c == '_' || isalpha(c) || isdigit(c))   {
            buf += c;
            prev_c = c;
            NextChar();
        }
        
        if(buf == "TRUE")   {
            bool temp = true;
            return Lex(LOGICAL, &temp);
        }
        
        if(buf == "FALSE")   {
            bool temp = false;
            return Lex(LOGICAL, &temp);
        }
        
        if(buf == "NULL")
            return Lex(NUL, NULL);
        
        else return Lex(STRING, &buf);
    }
    
    if(isdigit(c))
    {
        double temp;
        sscanf(ReadStr + pos - 1, "%lf", &temp);
        
        while(isdigit(c)) {prev_c = c; NextChar(); }
        if(c == '.') {
            prev_c = c;
            NextChar();
            while(isdigit(c)) { prev_c = c; NextChar(); }
            if(c == 'e' || c == 'E')   {
                prev_c = c;
                NextChar();
                if(c == '+' || c == '-')   {
                    prev_c = c;
                    NextChar();
                }
                while(isdigit(c)) {prev_c = c; NextChar();}
            }
        }
        
        return Lex(NUMERIC, &temp);
    }
    
    switch (c) {
        case '<':
            return DoubleOperation('-', 'A', '=', 'L');
        case '>':
            return DoubleOperation('=', 'G');
        case '=':
            return DoubleOperation('=', 'E');
        case '!':
            return DoubleOperation('=', 'D');
        case '"':

            buf = string("");
            prev_c = c;
            NextChar();
            
            if(c != '"')
                while((!(c == '"' && buf.length() > 0 && buf[buf.length() - 1] != '/') && c != EOF) || prev_c == '\\') {
                    if(c == '\n' && console)
                        cout << "+";
                    
                    buf += c;
                    prev_c = c;
                    NextChar();
                }
            if(c == EOF)
                throw string("Error : Wrong presentation of string constant\n");
            prev_c = c;
            NextChar();
     
            return Lex(STRINGCONST, &buf);
        case '#':
            while(c != EOF && c != '\n')
            {
                prev_c = c;
                NextChar();
            }
            
            break;
        default:
            break;
    }
    
    
    char temp = c;
    if(c != '\n')  {
        prev_c = c;
        NextChar();
    }
    else read = true;
    
    return Lex(OPERATION, &temp);
}

void Parser::Program(bool block = false)
{
    NextLex(false);
    
    if(CurrLex.c == EOF)
        return;
    
    Exp(!block);
    
    if(CurrLex.c == '\n' || CurrLex.c == ';')   {
        if(!block)   {
            Execution::Execute(prog);
            if(CurrLex.c == ';')   {
                prog.pol.clear();
                while(prog.ex.size() > 0) prog.ex.pop();
            }
            
            if(CurrLex.c == '\n')   {
                for(size_t i = 0; i < prog.print.size(); i++)
                {
                    if(prog.print[i].val.size() == 0)
                    {
                        type_of_val type = prog.print[i].type;
                        
                        string temp = "---";
                        switch (type) {
                            case NUL:
                                temp = "null";
                                break;
                            case STRINGCONST:
                                temp = "character";
                                break;
                            case NUMERIC:
                                temp = "numeric";
                                break;
                            case LOGICAL:
                                temp = "logical";
                                break;
                            default:
                                break;
                        }
                    
                        cout << temp << "(0)";
                    }
                    
                    for(size_t j = 0; j < prog.print[i].val.size(); j++)   {
                        if(prog.print[i].val[j] -> type == STRINGCONST) cout << "\"";
                        cout << prog.print[i].val[j] -> ToStr();
                        if(prog.print[i].val[j] -> type == STRINGCONST) cout << "\"";
                        cout << " ";
                    }
                    cout << endl;
                }
                prog.print.clear();
                prog.pol.clear();
                while(prog.ex.size() > 0) prog.ex.pop();
                
                if(console)cout << ">";
            }
        }
        
        if(block && console && CurrLex.c == '\n') cout << "+";
        
        Program(block);
    }
    
    if(CurrLex.c != EOF && !(block && CurrLex.c == '}'))
        throw string("Error: Unxpected ") + CurrLex.ToStr() + "\n";
    
    return;
}

void Parser::Exp(bool print = false)
{
    if(CurrLex.type == STRING)
        Var();
    else
    {
        Exp1(false);
        if(print) Push('p');
        return;
    }
    
    if(CurrLex.c == 'A')   {
        NextLex(true);
        
        Exp(false);
        
        Push('A');
        return;
    }
    
    Exp1(true);
    if(print) Push('p');
    return;
}

void Parser::Exp1(bool flag = false)
{
    Exp2(flag);
    
    if(CurrLex.c == '&' || CurrLex.c == '|')   {
        char op =  CurrLex.c;
        
        NextLex(true);
        Exp1();
        
        Push(op);
    }
}

void Parser::Exp2(bool flag = false)
{
    if(flag)   {
        Exp3(flag);
        return;
    }
    
    if(CurrLex.c == '!')   {
        NextLex(true);
        Exp3(false);
        
        Push('!');
        return;
    }
    
    Exp3(false);
}

void Parser::Exp3(bool flag = false)
{
    Exp4(flag);
    
    if(CurrLex.c == '<' || CurrLex.c == '>' || CurrLex.c == 'L' || CurrLex.c == 'G' || CurrLex.c == 'E' || CurrLex.c == 'D')   {
        char op = CurrLex.c;
        
        NextLex(true);
        Exp4(false);
        
        Push(op);
    }
}

void Parser::Exp4(bool flag = false)
{
    if(flag)   {
        Exp5(flag);
        return;
    }
    
    if(CurrLex.c == '-')   {
        NextLex(true);
        Exp5(false);
        
        Push('N');
        return;
    }
    
    Exp5(false);
}

void Parser::Exp5(bool flag = false)
{
    Exp6(flag);
    
    if(CurrLex.c == '+' || CurrLex.c == '-')   {
        char op = CurrLex.c;
        
        NextLex(true);
        Exp5(false);
        
        Push(op);
        return;
    }
}

void Parser::Exp6(bool flag = false)
{
    Exp7(flag);
    
    if(CurrLex.c == '*' || CurrLex.c == '/')   {
        char op = CurrLex.c;
        
        NextLex(true);
        Exp6(false);
        
        Push(op);
        return;
    }
}

void Parser::Exp7(bool flag = false)
{
    Exp8(flag);
    
    if(CurrLex.c == ':')   {
        NextLex(true);
        Exp7(false);
        
        Push(':');
        return;
    }
}

void Parser::Exp8(bool flag = false)
{
    if(flag)
        return;
    
    if(CurrLex.c == '(')   {
        NextLex(true);
        
        Exp(false);
        
        if(CurrLex.c != ')')
            throw string("Error : Wrong brackets sequence\n");
        
        NextLex(false);
        IndOrFunc();
        
        return;
    }
    
    if(CurrLex.type == STRINGCONST || CurrLex.type == NUMERIC || CurrLex.type == LOGICAL || CurrLex.type == NUL)   {
        Push(Ident(new Lex(CurrLex), CurrLex.type));
        NextLex(false);
        return;
    }
    
    if(CurrLex.type == STRING)   {
        Var();
        return;
    }
}

void Parser::Var()
{
    Push(Ident(new Lex(NUL), VARIABLE, CurrLex.StrValue));
    
    NextLex(false);
    IndOrFunc();
}

void Parser :: IndOrFunc()
{
    
    if(CurrLex.c == '[')   {
        NextLex(true);
        
        Exp(false);
        
        if(CurrLex.c != ']')
            throw string("Error : Wrong brackets sequence\n");
        
        Push(']');
        
        NextLex(false);
    }
    
    if(CurrLex.c == '(')   {
        NextLex(true);
        
        if(CurrLex.c == ')')   {
            Push(0);
            Push('c');
            
            NextLex(false);
            return;
        }
        
        int n = ArgList(false);
        
        if(CurrLex.c != ')')
            throw string("Error : Expected ')' \n");
        
        Push(n);
        Push('c');
        
        NextLex(false);
    }
}

int Parser::ArgList(bool def = false)
{
    ArgListItem(def);
    
    if(CurrLex.c == ',')   {
        NextLex(true);
        
        return ArgList(def) + 1;
    }
    
    return 1;
}

void Parser::ArgListItem(bool def = false)
{
    if(CurrLex.type != STRING)   {
        if(def)
            throw string("Error : Expected Variable in arguments\n");
        
        Exp(false);
        
        return;
    }
    
    Lex TempLex = CurrLex;
    
    NextLex(true);
    
    if(CurrLex.c == '=')   {
        Push(Ident(new Lex(TempLex), VARIABLE, TempLex.StrValue));
        
        NextLex(true);
        Exp(false);
        
        Push('=');
        
        return;
    }
    
    if(def)
    {
        if(CurrLex.c != ',' && CurrLex.c != ')')
            throw string("Error : Expected Variable in arguments\n");
        
        Push(Ident(new Lex(TempLex), VARIABLE, TempLex.StrValue));
        return;
    }
    
    LexList.push(CurrLex);
    CurrLex = TempLex;
    Exp(false);
}


void Execution::Execute(Poliz &p)
{
    while(p.pos < (int)p.pol.size())
    {
        if(p.pol[p.pos].type == OPERATION)
        {
            char c = p.pol[p.pos].val[0] -> c;

            switch (c) {
                case 'A':
                    OpAssign(p);
                    break;
                case '&':
                    OpAnd(p);
                    break;
                case '|':
                    OpOr(p);
                    break;
                case '!':
                    OpNo(p);
                    break;
                case '<':
                    OpLess(p);
                    break;
                case '>':
                    OpGreat(p);
                    break;
                case 'L':
                    OpLessEq(p);
                    break;
                case 'G':
                    OpGreatEq(p);
                    break;
                case 'E':
                    OpEq(p);
                    break;
                case 'D':
                    OpNoEq(p);
                    break;
                case 'N':
                    OpNeg(p);
                    break;
                case '+':
                    OpPlus(p);
                    break;
                case '-':
                    OpMinus(p);
                    break;
                case '*':
                    OpMul(p);
                    break;
                case '/':
                    OpDiv(p);
                    break;
                case ':':
                    OpVect(p);
                    break;
                case 'p':
                    OpPrint(p);
                    break;
                case 'c':
                    OpCall(p);
                    break;
                case ']':
                    OpInd(p);
                    break;
                default:
                    break;
            }
            
        }
        else p.ex.push(p.pol[p.pos]);
        
        p.pos++;
    }
    
    p.pos = 0;
}

void Execution::OpAssign(Poliz & p)
{
    Ident I2(p.ex.top());
    p.ex.pop();
    
    Ident I1(p.ex.top());
    p.ex.pop();
    
    if(I1.name == "")
        throw string("Error : Expected Variable before '<-'\n");
    
    if(I2.type == VARIABLE)   {
        if(p.id.count(I2.name))
            I2 = p.id[I2.name];
        else throw string("Error : Undeclared ") + I2.name + '\n';
    }
    
    Ident temp(0, NUL);
    temp = I2;
    
    if(I1.type == VARIABLE)
    {
        p.id.erase(I1.name);
        temp.name = I1.name;
        p.id[I1.name] = temp;
    }
    else
    {
        if(I1.type == STRINGCONST || temp.type == STRINGCONST)   {
            temp.ToStr();
            p.id[I1.name].ToStr();
        }
        else if(I1.type == NUMERIC || temp.type == NUMERIC) {
            temp.ToNum();
            p.id[I1.name].ToNum();
        }
        else if(I1.type == LOGICAL || temp.type == LOGICAL) {
            temp.ToBool();
            p.id[I1.name].ToBool();
        }
        
        for(size_t i = 0; i < I1.val.size(); i++)   {
            if(temp.val.size() == 0)   {
                I1.val[i] -> type = NUL;
                continue;
            }
            
            Lex TempLex(*(temp.val[i % temp.val.size()]));
            
            I1.val[i] -> type = TempLex.type;
            
            switch (TempLex.type) {
                case STRINGCONST:
                    I1.val[i] -> StrConst = TempLex.StrConst;
                    break;
                case NUMERIC:
                    I1.val[i] -> NumValue = TempLex.NumValue;
                    break;
                case LOGICAL:
                    I1.val[i] -> LogValue = TempLex.LogValue;
                    break;
                default:
                    break;
            }
        }
        
    }
    
    p.ex.push(I2);
}


void Execution::BinaryOp(Poliz &p, Ident &I1, Ident &I2, Ident &I3, string c, type_of_val type, bool StrAvalible, bool NewIdent = true)
{
    if(p.ex.size() == 0)
        throw string("Error \n");
    
    I2 = p.ex.top();
    p.ex.pop();
    
    if(p.ex.size() == 0)
        throw string("Error \n");
    
    I1 = p.ex.top();
    p.ex.pop();
    
    if(I1.type == VARIABLE)   {
        if(p.id.count(I1.name))
            I1 = p.id[I1.name];
        else throw string("Error : Undeclared ") + I1.name + '\n';
    }
    
    if(I2.type == VARIABLE)   {
        if(p.id.count(I2.name))
            I2 = p.id[I2.name];
        else throw string("Error : Undeclared ") + I2.name + '\n';
    }
    
    if(I1.type != LOGICAL && I1.type != NUMERIC && (I1.type != STRINGCONST || !StrAvalible) && I1.type != NUL )
        throw string("Error : Operator '") + c + string("' work only with Logival or Numeric or String value\n");
    
    if(I2.type != LOGICAL && I2.type != NUMERIC && (I2.type != STRINGCONST || !StrAvalible) && I2.type != NUL )
        throw string("Error : Operator '") + c + string("' work only with Logival or Numeric or String value\n");
    
    if(NewIdent)   {
        size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = max(n1,n2);
        I3 = Ident(n3, type);
    }
}

void Execution::UnaryOp(Poliz &p, Ident &I1, Ident &I2, string c, type_of_val type, bool StrAvalible)
{
    if(p.ex.size() == 0)
        throw string("Error \n");
    
    I1 = p.ex.top();
    p.ex.pop();
    
    if(I1.type == VARIABLE)   {
        if(p.id.count(I1.name))
            I1 = p.id[I1.name];
        else throw string("Error : Undeclared ") + I1.name + '\n';
    }
    
    if(I1.type != LOGICAL && I1.type != NUMERIC && (I1.type != STRINGCONST || !StrAvalible) && I1.type != NUL )
        throw string("Error : Operator ") + c + string(" work only with Logival or Numeric or String value\n");
    
    size_t n = I1.val.size();
    
    I2 = Ident(n, type);
}

void Execution::OpAnd(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "&", LOGICAL, false);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++)   {
        if((I1.val[i % n1] -> type == NUL && !I2.val[i % n2] -> ToBool()) || (I2.val[i % n1] -> type == NUL && !I1.val[i % n2] -> ToBool()))
            I3.val[i] -> LogValue = false;
        else
            if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
                I3.val[i] -> type = NUL;
            else I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() && I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpOr(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "|", LOGICAL, false);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if((I1.val[i % n1] -> type == NUL && I2.val[i % n2] -> ToBool()) || (I2.val[i % n1] -> type == NUL && I1.val[i % n2] -> ToBool()))
            I3.val[i] -> LogValue = true;
        else
            if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
                I3.val[i] -> type = NUL;
            else I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() || I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpNo(Poliz &p)
{
    Ident I1, I2;
    
    UnaryOp(p, I1, I2, "!", LOGICAL, false);
    
    if(I1.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n = I1.val.size();
    for(size_t i = 0; i < n; i++) {
        if(I1.val[i] -> type == NUL)
            I2.val[i] -> type = NUL;
        else I2.val[i] -> LogValue = !I1.val[i] -> ToBool();
    }
    
    p.ex.push(I2);
}

void Execution::OpLess(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "<", LOGICAL, true);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++)   {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else
            if(I1.type == STRINGCONST || I2.type == STRINGCONST)
                I3.val[i] -> LogValue = I1.val[i % n1] -> ToStr() < I2.val[i % n2] -> ToStr();
            else
                if(I1.type == NUMERIC || I2.type == NUMERIC)
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToNum() < I2.val[i % n2] -> ToNum();
                else
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() < I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpGreat(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, ">", LOGICAL, true);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++)   {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else
            if(I1.type == STRINGCONST || I2.type == STRINGCONST)
                I3.val[i] -> LogValue = I1.val[i % n1] -> ToStr() > I2.val[i % n2] -> ToStr();
            else
                if(I1.type == NUMERIC || I2.type == NUMERIC)
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToNum() > I2.val[i % n2] -> ToNum();
                else
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() > I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpLessEq(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "<=", LOGICAL, true);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else
            if(I1.type == STRINGCONST || I2.type == STRINGCONST)
                I3.val[i] -> LogValue = I1.val[i % n1] -> ToStr() <= I2.val[i % n2] -> ToStr();
            else
                if(I1.type == NUMERIC || I2.type == NUMERIC)
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToNum() <= I2.val[i % n2] -> ToNum();
                else
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() <= I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpGreatEq(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, ">=" , LOGICAL, true);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else
            if(I1.type == STRINGCONST || I2.type == STRINGCONST)
                I3.val[i] -> LogValue = I1.val[i % n1] -> ToStr() >= I2.val[i % n2] -> ToStr();
            else
                if(I1.type == NUMERIC || I2.type == NUMERIC)
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToNum() >= I2.val[i % n2] -> ToNum();
                else
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() >= I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpEq(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "==", LOGICAL, true);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else
            if(I1.type == STRINGCONST || I2.type == STRINGCONST)
                I3.val[i] -> LogValue = I1.val[i % n1] -> ToStr() == I2.val[i % n2] -> ToStr();
            else
                if(I1.type == NUMERIC || I2.type == NUMERIC)
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToNum() == I2.val[i % n2] -> ToNum();
                else
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() == I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpNoEq(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "!=", LOGICAL, true);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, LOGICAL));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++)  {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else
            if(I1.type == STRINGCONST || I2.type == STRINGCONST)
                I3.val[i] -> LogValue = I1.val[i % n1] -> ToStr() != I2.val[i % n2] -> ToStr();
            else
                if(I1.type == NUMERIC || I2.type == NUMERIC)
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToNum() != I2.val[i % n2] -> ToNum();
                else
                    I3.val[i] -> LogValue = I1.val[i % n1] -> ToBool() != I2.val[i % n2] -> ToBool();
    }
    
    p.ex.push(I3);
}

void Execution::OpNeg(Poliz &p)
{
    Ident I1, I2;
    
    UnaryOp(p, I1, I2, "-", NUMERIC, false);
    
    if(I1.val.size() == 0) {
        p.ex.push(Ident(0, NUMERIC));
        return;
    }
    size_t n = I1.val.size();
    for(size_t i = 0; i < n; i++) {
        if(I1.val[i] -> type == NUL)
            I2.val[i] -> type = NUL;
        else I2.val[i] -> NumValue = - I1.val[i] -> ToNum();
    }
    
    p.ex.push(I2);
}

void Execution::OpPlus(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "+", NUMERIC, false);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, NUMERIC));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else I3.val[i] -> NumValue = I1.val[i % n1] -> ToNum() + I2.val[i % n2] -> ToNum();
    }
    
    p.ex.push(I3);
}

void Execution::OpMinus(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "-", NUMERIC, false);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, NUMERIC));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i%n1] -> type == NUL || I2.val[i%n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else I3.val[i] -> NumValue = I1.val[i%n1] -> ToNum() - I2.val[i%n2] -> ToNum();
    }
    
    p.ex.push(I3);
    
}

void Execution::OpMul(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "*", NUMERIC, false);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, NUMERIC));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i%n1] -> type == NUL || I2.val[i%n2] -> type == NUL)
            I3.val[i] -> type = NUL;
        else I3.val[i] -> NumValue = I1.val[i%n1] -> ToNum() * I2.val[i%n2] -> ToNum();
    }
    
    p.ex.push(I3);
    
}

void Execution::OpDiv(Poliz &p)
{
    Ident I1, I2, I3;
    
    BinaryOp(p, I1, I2, I3, "/", NUMERIC, false);
    
    if(I1.val.size() == 0 || I2.val.size() == 0) {
        p.ex.push(Ident(0, NUMERIC));
        return;
    }
    
    size_t n1 = I1.val.size(), n2 = I2.val.size(), n3 = I3.val.size();
    for(size_t i = 0; i < n3; i++) {
        if(I1.val[i % n1] -> type == NUL || I2.val[i % n2] -> type == NUL || I2.val[i % n2] -> ToNum() == 0)
            I3.val[i] -> type = NUL;
        else I3.val[i] -> NumValue = I1.val[i % n1] -> ToNum() / I2.val[i % n2] -> ToNum();
    }
    
    p.ex.push(I3);
    
}

void Execution::OpVect(Poliz &p)
{
    Ident I1, I2, I3;
    BinaryOp(p, I1, I2, I3, ":", NUMERIC, false, false);
    
    if(I1.val.size() > 1 || I2.val.size() > 1) {} // Warning
    
    if(I1.val.size() == 0 || I2.val.size() == 0)
        throw string("Error : in operation ':' argument has length 0 \n");
    
    if(I1.val[0] -> type == NUL || I2.val[0] -> type == NUL)
        throw string("Error : NULL in operation ':' \n");
    
    double x1 = I1.val[0] -> ToNum(), x2 = I2.val[0] -> ToNum();
    
    int n = fabs(int(x2) - int(x1)) + 1;
    I3 = Ident(n, NUMERIC);
    
    for(int i = 0; i < n; i++)
        I3.val[i] -> NumValue = x1 + (double)((x2 > x1) ? i : (-i));
    
    p.ex.push(I3);
}


void Execution::OpCall(Poliz &p)
{
    Ident N(p.ex.top());
    p.ex.pop();
    
    int n = (int)N.val[0] -> NumValue;
    
    vector<Ident> arg(n);
    for(int i = n-1; i >= 0; i--)   {
        arg[i] = p.ex.top();
        p.ex.pop();
    }
    
    for(int i = 0; i < (int)arg.size(); i++)
    {
        if(!arg[i].arg && arg[i].type == VARIABLE)   {
            if(p.id.count(arg[i].name))
                arg[i] = p.id[arg[i].name];
            else throw string("Error : Undeclared ") + arg[i].name + '\n';
        }
    }
    
    Ident f(p.ex.top());
    p.ex.pop();
    
    if(f.type == VARIABLE)   {
        if(f.name == "c")   {
            OpC(p, arg);
            return;
        }
        
        if(f.name == "mode")   {
            OpMode(p, arg);
            return;
        }
        
        if(f.name == "length")   {
            OpLength(p, arg);
            return;
        }
    }
    
    if(f.type == VARIABLE)   {
        if(p.id.count(f.name))
            f = p.id[f.name];
        else throw string("Error : Undeclared ") + f.name + '\n';
    }
    
    if(f.val.size() > 1 || f.val.size() == 0)
        throw string("Error : Non applying to vector or vector length 0\n");
    
}


void Execution::OpC(Poliz &p, vector<Ident> &arg)
{
    bool isnum = false, isbool = false, isstr = false;
    
    int n = 0;
    for(size_t i = 0; i < arg.size(); i++)   {
        switch (arg[i].type) {
            case STRINGCONST:
                isstr = true;
                break;
            case NUMERIC:
                isnum = true;
                break;
            case LOGICAL:
                isbool = true;
                break;
            default:
                break;
        }
        n += arg[i].val.size();
    }
    
    Ident NewIdent;
    if(isstr)   {
        NewIdent = Ident(n, STRINGCONST);
        for(size_t i = 0; i < arg.size(); i++)
            arg[i].ToStr();
    }
    else if(isnum)   {
        NewIdent = Ident(n, NUMERIC);
        for(size_t i = 0; i < arg.size(); i++)
            arg[i].ToNum();
    }
    else NewIdent = Ident(n, LOGICAL);
    
    int k = 0;
    for(size_t i = 0; i < arg.size(); i++)   {
        for(size_t j = 0; j < arg[i].val.size(); j++)   {
            *(NewIdent.val[k]) = *(arg[i].val[j]);
            k++;
        }
    }
    
    p.ex.push(NewIdent);
}

void Execution::OpMode(Poliz &p, vector<Ident> &arg)
{
    if(arg.size() > 1)
        throw string("Error : Unused Argument\n");
    
    type_of_val type = arg[0].type;
    
    string temp = "---";
    switch (type) {
        case NUL:
            temp = "null";
            break;
        case STRINGCONST:
            temp = "character";
            break;
        case NUMERIC:
            temp = "numeric";
            break;
        case LOGICAL:
            temp = "logical";
            break;
        default:
            break;
    }
    
    p.ex.push(Ident(new Lex(STRINGCONST, &temp), STRINGCONST));
}

void Execution::OpLength(Poliz &p, vector<Ident> &arg)
{
    if(arg.size() > 1)
        throw string("Error : Unused Argument\n");
    
    double n = arg[0].val.size();
    p.ex.push(Ident(new Lex(NUMERIC, &n), NUMERIC));
}

void Execution::OpInd(Poliz &p)
{
    Ident I2(p.ex.top());
    p.ex.pop();
    
    Ident I(p.ex.top());
    p.ex.pop();
    
    if(I2.type == VARIABLE)   {
        if(p.id.count(I2.name))
            I2 = p.id[I2.name];
        else throw string("Error : Undeclared ") + I2.name + "\n";
    }
    
    if(I2.type != NUMERIC && I2.type != LOGICAL)
        throw string("Error : operator '[]' work only with Logical or Numeric value\n");
    
    if(I.type == VARIABLE && p.id.count(I.name) == 0)
        throw string("Error : Undeclared ") + I.name + "\n";
    
    Ident I1(I.type == VARIABLE ? p.id[I.name] : I);
    
    if(I2.type == NUMERIC)
    {
        Ident Res(I2.val.size(), I1.type, I1.name);
        for(size_t i = 0; i < I2.val.size(); i++)   {
            if(I2.val[i] -> type == NUL)   {
                Res.val[i] -> type = NUL;
                continue;
            }
            
            int j = (int)I2.val[i] -> NumValue - 1;
            if(j < 0)
                throw string("Error : Index < 1\n");
            
            if(j < (int)I1.val.size())   {
                delete Res.val[i];
                Res.val[i] = I1.val[j];
                (Res.val[i] -> nlinks) ++;
            }
            else Res.val[i] -> type = NUL;
        }
        
        p.ex.push(Res);
        return;
    }
    else
    {
        int n = max(I2.val.size(), I1.val.size());
        Ident Res(0, I1.type, I1.name);
        for(int i = 0; i < n; i++)   {
            int j = i%I2.val.size();
            
            if(I2.val[j] -> type == NUL)   {
                Res.val.push_back(new Lex(NUL));
                continue;
            }
            
            if(I2.val[j] -> LogValue)   {
                if(i < (int)I1.val.size())   {
                    Res.val.push_back(I1.val[i]);
                    I1.val[i] -> nlinks++;
                }
                else Res.val.push_back(new Lex(NUL));
            }
        }
        
        p.ex.push(Res);
        return;
    }
    
}

void Execution::OpPrint(Poliz &p)
{
    if(p.ex.size() > 0)   {
        Ident I(0, NUL);
        I = p.ex.top();
        p.ex.pop();
        
        if(I.type == VARIABLE)    {
            if(p.id.count(I.name) == 0)
                throw string("Error : Undeclared ") + I.name + '\n';
            
            I = p.id[I.name];
        }
        
        p.print.push_back(I);
    }
}

int main(int argc, char *argv[])
{
    char *FileName = NULL;
    if(argc > 1) FileName = argv[1];
    
    Parser par(FileName);
    
    bool stop = false;
    while(!stop)
    {
        try
        {
            par.Program();
            stop = true;
        }
        catch(string str)   {
            cout << str;
            par.Continue();
        }
        
    }
    
    int n;
    cin >> n;
    return 0;
}

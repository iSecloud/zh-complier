#include "common.h"
#include "parse.h"

#define FOLLOW vector<int>
#define LALO_FIRST ASSIGN, OOR, AAND, OR, XOR, AND, GT, GE, LT, LE, EQU, NEQU, ADD, SUB, MUL, \
MOD, DIV, INC, DEC
#define RALO_FIRST OOR, AAND, OR, XOR, AND, GT, GE, LT, LE, EQU, NEQU, ADD, SUB, MUL, DIV, MOD
#define EXPR_FIRST INC, DEC, MUL, NNOT, SUB, NOT, AND, LPAREN, NUM, CHAR, STR, ID
#define TYPE_FIRST KW_INT, KW_CHAR, KW_VOID
#define STAT_FIRST KW_WHILE, KW_DO, KW_SWITCH, KW_IF, KW_FOR, KW_SECLOUD, KW_READ, \
KW_WRITE, KW_RETURN, KW_BREAK, KW_CONTINUE, SEMICON

Parser::Parser(Lex &lexer1, SymTab &symtab1, GenIR &ir1): 
lexer(lexer1), symtab(symtab1) , ir(ir1)
{}

Parser::~Parser() 
{}

Tag Parser::GetTag()
{
    Tag t = lookahead->tag;
    move();
    return t;
}

void Parser::Analysis()
{
    printf("Begin to Analysis....\n");
    move();
    Program();
}

void Parser::move()
{
    lookahead = lexer.getToken();   
    cout << lookahead->toString() << endl; 
}

bool Parser::match(int need)
{
    if(lookahead->tag == END) 
    {
        printf("var size = %d\n", symtab.getVarList().size()); 
        for(auto i: symtab.getVarList()) cout << i << endl;

        printf("fun size = %d\n", symtab.getFunList().size());
        for(auto i: symtab.getFunList()) cout << i << endl;
        exit(0);
    }
    if(lookahead->tag == need)
    {
        move();
        return true;
    }
    return false;
}

//是否在select集里面
bool Parser::isInFollow(vector<int> a)
{
    if(find(a.begin(), a.end(), lookahead->tag) != a.end()) 
        return true;
    else 
        return false;
}

void Parser::recovery(bool real, Err errCode)
{
    Error::showError(errCode);
    //不可以与follow集匹配
    if(!real) move(); 
}

void Parser::Program()
{
    Segment();
    Program();
}

void Parser::Segment()
{
    //printf("I am in Segment!"); 
    bool ext = match(KW_EXTERN);
    Tag t = Type();
    Def(t, ext);
}

Tag Parser::Type()
{
    Tag t = KW_INT;
    if(isInFollow(FOLLOW{TYPE_FIRST}))
        t = GetTag();
    else 
        recovery(isInFollow(FOLLOW{ID, MUL}), TYPE_LOST);
    return t;
}

Var* Parser::Init(Tag t, bool ext, string name, bool ptr)
{
    Var* val = NULL;
    if(match(ASSIGN)) val = expr();
    return new Var(symtab.getScope(), ext, t, ptr, name, val);
}

void Parser::Def(Tag t, bool ext)
{
    bool mul = match(MUL);
    string idName = ""; //默认为空
    if(lookahead->tag == ID) 
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(!mul) DefTail(t, ext, idName, 0);
        else
        {
            symtab.addVar(Init(t, ext, idName, 1));
            VarList(t, ext);
        } 
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA}), IDNAME_LOST);
            symtab.addVar(Init(t, ext, idName, 1));
            VarList(t, ext);
        }
        else
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA, LPAREN, LBRACKET}), IDNAME_LOST);
            DefTail(t, ext, idName, 0);
        }
    }
}

void Parser::DefTail(Tag t, bool ext, string name, bool ptr)
{
    if(match(LPAREN)) //函数
    {
        symtab.enter();
        //Tag t = Type();
        vector<Var*> paraList; //参数列表
        Para(paraList);
        if(!match(RPAREN)) 
            recovery(isInFollow(FOLLOW{SEMICON, LBRACE}), RPAREN_LOST);
        Fun* fun = new Fun(ext, t, name, paraList);
        //cout << lookahead->toString() << endl;
        FunTail(fun);
        symtab.leave();
    }
    else //普通变量定义
    {
        symtab.addVar(NorVarDef(t, ext, name, ptr));
        VarList(t, ext);
    }
}

Var* Parser::NorVarDef(Tag t, bool ext, string name, bool ptr)
{
    if(match(LBRACKET))
    {
        int num = 0;
        if(lookahead->tag == NUM)
        {
            num = ((Num*)lookahead)->num;
            move();
        }
        else recovery(isInFollow(FOLLOW{RBRACKET}), NUM_LOST);

        if(!match(RBRACKET)) 
            recovery(isInFollow(FOLLOW{COMMA, SEMICON}), RBRACKET_LOST);

        return new Var(symtab.getScope(), ext, t, name, num);
    }
    else
    {
        return Init(t, ext, name, ptr);
    }
}

void Parser::VarList(Tag t, bool ext)
{
    if(match(SEMICON)) return;

    if(match(COMMA))
    {
        symtab.addVar(VarDef(t, ext));
        VarList(t, ext);
    }
    else
    {
        if(isInFollow(FOLLOW{ID, MUL}))
        {
            recovery(true, COMMA_LOST);
            symtab.addVar(VarDef(t, ext)); 
            VarList(t, ext);
        }
        else
        {
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
        }
    }
}

Var* Parser::VarDef(Tag t, bool ext)
{
    bool mul = match(MUL);
    string idName = "";
    if(lookahead->tag == ID)
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(!mul) return NorVarDef(t, ext, idName, mul);
        else return Init(t, ext, idName, mul);
    }
    else
    {
        if(!mul)
        {
            recovery(isInFollow(FOLLOW{LBRACKET, ASSIGN, COMMA, SEMICON}), IDNAME_LOST);
            return NorVarDef(t, ext, idName, mul);
        }
        else
        {
            recovery(isInFollow(FOLLOW{ASSIGN, COMMA, SEMICON}), IDNAME_LOST);
            return Init(t, ext, idName, mul);
        }
    }
}

void Parser::Para(vector<Var*> &paraList)
{
    if(lookahead->tag == RPAREN) return;
    Tag t = Type();
    Var* pr = ParaDef(t);
    symtab.addVar(pr);
    paraList.push_back(pr);
    ParaList(paraList);

}

Var* Parser::ParaDef(Tag t)
{
    bool mul = match(MUL);
    string idName = "";
    if(lookahead->tag == ID)
    {
        idName = ((Id*)lookahead)->id;
        move();
        return mul ? new Var(symtab.getScope(), false, t, true, idName) : ParaTail(t, idName);
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{COMMA, RPAREN}), IDNAME_LOST);
            return new Var(symtab.getScope(), false, t, true, idName);
        }
        else
        {
            recovery(isInFollow(FOLLOW{COMMA, RPAREN, LBRACKET}), IDNAME_LOST);
            return ParaTail(t, idName);
        }
    }
}

Var* Parser::ParaTail(Tag t, string name)
{
    if(!match(LBRACKET))
    {
        return new Var(symtab.getScope(), false, t, false, name);
    }
    else
    {
        int num = 0;
        if(lookahead->tag == NUM)
        {
            num = ((Num*)lookahead)->num;
            move();
        }//函数传数组可以不指定长度

        if(!match(RBRACKET)) 
            recovery(isInFollow(FOLLOW{COMMA, RPAREN}), RPAREN_LOST);
        return new Var(symtab.getScope(), false, t, name, num);
    }
}

void Parser::ParaList(vector<Var*> &paraList)
{
    if(!match(COMMA)) return;
    Tag t = Type();
    Var* pr = ParaDef(t);
    symtab.addVar(pr);
    paraList.push_back(pr);
    ParaList(paraList);
}

void Parser::FunDef()
{}

void Parser::FunTail(Fun* fun)
{
    if(match(SEMICON))
    {
        symtab.decFun(fun);
    }
    else
    {
        symtab.defFun(fun);
        FunBody();
        symtab.endDefFun();  
    }
}

void Parser::FunBody()
{
    //printf("I am in funbody"); 
    if(!match(LBRACE)) 
        recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST, RBRACE}), LBRACE_LOST);
    FunProgram();
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), RBRACE_LOST);
}

void Parser::FunProgram()
{
    //printf("I am in funprogram"); 
    if(isInFollow(FOLLOW{TYPE_FIRST})) 
    {
        LocalDef();
        FunProgram();
    }
    else if(isInFollow(FOLLOW{STAT_FIRST, EXPR_FIRST}))
    {
        Statement();
        FunProgram();
    }
}

void Parser::LocalDef()
{
    Tag t = Type();
    symtab.addVar(VarDef(t, false)); 
    VarList(t, false);
}

void Parser::Statement()
{
    Tag t = lookahead->tag;
    if(t == KW_WHILE) WhileStat();
    else if(t == KW_DO) DoWhileStat();
    else if(t == KW_FOR) ForStat();
    else if(t == KW_IF) IfStat();
    else if(t == KW_SWITCH) SwitchStat();
    else if(t == KW_SECLOUD) SecloudStat();
    else if(t == KW_READ) ReadStat();
    else if(t == KW_WRITE) WriteStat(0);
    else if(t == KW_BREAK) 
    {
        move();
        ir.genBreak();
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else if(t == KW_CONTINUE)
    {
        ir.genContinue();
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else if(t == KW_RETURN)
    {
        move();
        ir.genFunReturn(expr());
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else
    {
        altexpr();
        if(match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
}

void Parser::WhileStat()
{
    symtab.enter();
    move();
    Quaternion *_while, *_exit;
    ir.genWhileHead(_while, _exit);
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, RPAREN}), LPAREN_LOST);
    Var* condition = altexpr();
    ir.genWhilecond(condition, _exit);
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    ir.genWhileTail(_while, _exit);
    symtab.leave();
} 

void Parser::DoWhileStat()
{
    move();
    symtab.enter();
    Quaternion *_do, *_exit;
    ir.genDoWhileHead(_do, _exit);
    FunBody();
    symtab.leave();
    if(!match(KW_WHILE))
        recovery(isInFollow(FOLLOW{LPAREN}), WHILE_LOST);
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, RPAREN}), LPAREN_LOST);
    Var* condition = altexpr();
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{SEMICON}), RPAREN_LOST);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    ir.genDoWhileTail(_do, _exit, condition);
}

void Parser::ForStat()
{
    move();
    symtab.enter();
    Quaternion *_for, *_exit, *_step, *_block;
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, EXPR_FIRST, SEMICON}), LPAREN_LOST);
    ForInit();    
    ir.genForHead(_for, _exit, _block);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), SEMICON_LOST);
    Var* condition = altexpr();
    ir.genForStep(_exit, _block, _step, condition);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), SEMICON_LOST);
    altexpr();  
    ir.genForBlockHead(_for, _block);
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    ir.genForBlockTail(_step, _exit);
    symtab.leave();
}

void Parser::ForInit()
{
    if(isInFollow(FOLLOW{TYPE_FIRST}))
        LocalDef();
    else 
        altexpr();
}

void Parser::IfStat()
{
    symtab.enter();
    Quaternion *_else, *_exit;
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{EXPR_FIRST, RPAREN}), LPAREN_LOST);
    Var* condition = altexpr();
    ir.genIfHead(condition, _else);
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    symtab.leave();
    ElseStat(_else, _exit);
}

void Parser::ElseStat(Quaternion* _else, Quaternion* _exit)
{
    if(match(KW_ELSE))
    {
        ir.genElseHead(_else, _exit);
        symtab.enter();
        FunBody();
        symtab.leave();
        ir.genElseTail(_exit);
    }
    else
    {
        ir.genIfTail(_else);
    }
}

void Parser::SwitchStat()
{
    move();
    symtab.enter();
    Quaternion* _exit;
    ir.genSwitchHead(_exit);
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), LPAREN_LOST); //注意这里必须有表达式？
    Var* condition = expr();
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    if(!match(LBRACE))
        recovery(isInFollow(FOLLOW{KW_CASE, KW_DEFAULT}), LBRACE_LOST);
    CaseStat(condition);
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), RBRACE_LOST);
    ir.genSwitchTail(_exit);
    symtab.leave();
}

void Parser::CaseStat(Var* condition)
{
    if(match(KW_CASE))
    {
        Quaternion* _case;
        Var* lit = literal();  
        ir.genCaseHead(condition, lit, _case);
        if(!match(COLON))
            recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST}), COLON_LOST);
        symtab.enter();
        FunProgram();
        symtab.leave();
        ir.genCaseTail(_case);
        CaseStat(condition);
    }
    else if(match(KW_DEFAULT))
    {
        if(!match(COLON))
            recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST}), COLON_LOST);
        symtab.enter();
        FunProgram();
        symtab.leave();
    }
}

void Parser::SecloudStat()
{
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{NUM}), LPAREN_LOST);
    int num = 0;
    if(!match(NUM))
        recovery(isInFollow(FOLLOW(RPAREN)), NUM_LOST);
    else 
        WriteStat(RadNum());
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{SEMICON}), RPAREN_LOST);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
}

int Parser::RadNum()
{
    srand(time(NULL));
    int num[] = {123, 57, 152, 110};
    return num[rand() % 4];
}

void Parser::ReadStat() //TODO 以后进行完善
{}

void Parser::WriteStat(int num) //TODO 以后进行完善
{}

Var* Parser::altexpr()
{
    //printf("In altexpr!"); 
    if(isInFollow(FOLLOW{EXPR_FIRST}))
        return expr();
    else return Var::getVoid(); 
}

Var* Parser::expr()
{
    //printf("In expr!");
    return assexpr();
}

Var* Parser::assexpr()
{
    Var* leftVal = oorexpr();
    return asstail(leftVal);
}

Var* Parser::asstail(Var* leftVal)
{
    if(!match(ASSIGN)) return leftVal;
    Var* rightVal = assexpr();
    Var* ans = ir.genBinOp(ASSIGN, leftVal, rightVal); //TODO 记录左值与右值的运算答案
    return ans; 
    //return asstial(ans) ?
}

Var* Parser::oorexpr()
{
    Var* leftVal = aandexpr();
    return oortail(leftVal);
}

Var* Parser::oortail(Var* leftVal)
{
    if(!match(OOR)) return leftVal;
    Var* rightVal = aandexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return oortail(ans);
}

Var* Parser::aandexpr()
{
    Var* leftVal = orexpr();
    return aandtail(leftVal);
}

Var* Parser::aandtail(Var* leftVal)
{
    if(!match(AAND)) return leftVal;
    Var* rightVal = orexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return aandtail(ans);
}

Var* Parser::orexpr()
{
    Var* leftVal = xorexpr();
    return ortail(leftVal);
}

Var* Parser::ortail(Var* leftVal)
{
    if(!match(OR)) return leftVal;
    Var* rightVal = xorexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return ortail(ans);
}

Var* Parser::xorexpr()
{
    Var* leftVal = andexpr();
    return xortail(leftVal);
}

Var* Parser::xortail(Var* leftVal)
{
    if(!match(XOR)) return leftVal;
    Var* rightVal = andexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return xortail(ans);
}

Var* Parser::andexpr()
{
    Var* leftVal = cmpexpr();
    return andtail(leftVal);
}

Var* Parser::andtail(Var* leftVal)
{
    if(!match(AND)) return leftVal;
    Var* rightVal = cmpexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return andtail(ans);
}

Var* Parser::cmpexpr()
{
    Var* leftVal = aloexpr();
    return cmptail(leftVal);
}

Var* Parser::cmptail(Var* leftVal)
{
    if(!isInFollow(FOLLOW{GT, GE, LT, LE, NEQU, EQU})) 
        return leftVal;
    Tag cp = cmps();
    Var* rightVal = aloexpr();
    Var* ans; //TODO 记录左值与右值的运算答案
    return cmptail(ans);
}

Tag Parser::cmps()
{
    return GetTag();
}

Var* Parser::aloexpr()
{
    Var* leftVal = item();
    return alotail(leftVal);
}

Var* Parser::alotail(Var* leftVal)
{
    if(!isInFollow(FOLLOW{ADD, SUB})) return leftVal;
    Tag al = alos();
    Var* rightVal = item();
    Var* ans; //TODO 记录左值与右值的运算答案
    return alotail(ans);
}

Tag Parser::alos()
{
    return GetTag();
}

Var* Parser::item()
{
    Var* leftVal = factor();
    return itemtail(leftVal);
}

Var* Parser::itemtail(Var* leftVal)
{
    if(!isInFollow(FOLLOW{MUL, DIV, MOD})) return leftVal;
    Tag it = its();
    Var* rightVal = factor();
    Var* ans; //TODO 记录左值与右值的运算答案
    return itemtail(ans);
}

Tag Parser::its()
{
    return GetTag();
}

Var* Parser::factor()
{
    if(isInFollow(FOLLOW{NNOT, NOT, AND, MUL, SUB, INC, DEC}))
    {
        Tag lp = lop();
        Var* ans = factor();
        //TODO 记录单目运算的运算答案
        return ans;
    }
    else return val();
}

Tag Parser::lop()
{
    return GetTag();
}

Var* Parser::val()
{
    Var* ans = element();
    if(isInFollow(FOLLOW{INC, DEC}))
    {
        Tag rp = rop();
        //TODO 记录单目运算的运算答案
    }
    return ans;
}

Tag Parser::rop()
{
    return GetTag();
}

Var* Parser::element()
{
    //printf("I am in element!"); 
    if(isInFollow(FOLLOW{ID}))
    {
        //printf("++++++++++++++++++\n");
        string name = ((Id*)lookahead)->id;
        move();
        return idexpr(name);
    }
    else if(match(LPAREN))
    {
        //printf("-------------------\n");
        Var* exp = expr();
        if(!match(RPAREN))
            recovery(isInFollow(FOLLOW{LALO_FIRST}), RPAREN_LOST);
        return exp;
    }
    else return literal();
}

Var* Parser::idexpr(string name)
{
    //printf("I am in idexpr!"); 
    if(match(LBRACKET))
    {
        Var* len = expr();
        if(!match(RBRACKET))
            recovery(isInFollow(FOLLOW{LALO_FIRST}), RBRACKET_LOST);
        Var* array = symtab.getVar(name);
        return array; //TODO 数组运算表达式
    }
    else if(match(LPAREN))
    {
        vector<Var*> argList;
        realarg(argList);
        if(!match(RPAREN))
            recovery(isInFollow(FOLLOW{RALO_FIRST}), RPAREN_LOST);
        Fun* fun = symtab.getFun(name, argList);
        //fun->toStringFun(); //system("pause");
        Var* ans;        //TODO 调用函数并进行运算
        return ans;
    }
    else
    {
        Var* ans = symtab.getVar(name); 
        return ans;
    }
}

void Parser::realarg(vector<Var*> &argList)
{
    if(!isInFollow(FOLLOW{EXPR_FIRST})) return;
    Var* exp = expr();
    argList.push_back(exp);
    arglist(argList);
}

void Parser::arglist(vector<Var*> &argList)
{
    if(!match(COMMA)) return;
    Var* exp = expr();
    argList.push_back(exp);
    arglist(argList);
}

Var* Parser::literal()
{
    Var* ans = NULL;
    if(isInFollow(FOLLOW{NUM, CHAR, STR}))
    {
        ans = new Var(lookahead);
        if(isInFollow(FOLLOW{STR}))
            symtab.addStr(ans);
        else 
            symtab.addVar(ans);
        move();
    }
    else
        recovery(isInFollow(FOLLOW{RALO_FIRST}), LITERAL_LOST);
    return ans;
}   
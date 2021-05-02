#include <vector>
#include <ctime>
#include "parse.h"
#include "lex.h"
#include "token.h"
#include "symbol.h"
#include "error.h"

Parser::Parser(Lex &lexer1): lexer(lexer1) 
{}

Parser::~Parser() 
{}

void Parser::move()
{
    lookahead = lexer.getToken();
}

bool Parser::match(int need)
{
    if(lookahead->tag == need)
    {
        move();
        return true;
    }
    return false;
}

#define FOLLOW vector<int>
#define EXPR_FIRST INC, DEC, MUL, NNOT, SUB, NOT, AND, LPAREN, NUM, CHAR, STR, ID
#define TYPE_FIRST KW_INT, KW_CHAR, KW_VOID
#define STAT_FIRST KW_WHILE, KW_DO, KW_SWITCH, KW_IF, KW_FOR, KW_SECLOUD, KW_READ, \
KW_WRITE, KW_RETURN, KW_BREAK, KW_CONTINUE, SEMICON

//是否在follow集里面
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
    bool ext = match(KW_EXTERN);
    Tag t = Type();
    Def(t, ext);
}

Tag Parser::Type()
{
    Tag t = KW_INT;
    if(isInFollow(FOLLOW{TYPE_FIRST}))
    {
        t = lookahead->tag;
        move();
    }
    else 
    {
        recovery(isInFollow(FOLLOW{ID, MUL}), TYPE_LOST);
    }
    return t;
}

Var* Parser::Init(Tag t, bool ext, string name, bool ptr)
{
    Var* val = NULL;
    if(match(ASSIGN)) val = expr();
    //TODO 这里需要新建一个变量并返回
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
            Init(t, ext, idName, 1); //TODO:添加一个指针变量
            VarList(t, ext);
        } 
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA}), IDNAME_LOST);
            Init(t, ext, idName, 1); //TODO:添加一个指针变量
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
        //TODO: 记录函数路径, 进入函数
        Tag t = Type();
        vector<Var*> paraList; //参数列表
        Para(paraList);
        if(!match(RPAREN)) recovery(isInFollow(FOLLOW{SEMICON, LBRACE}), RPAREN_LOST);
        Fun* Fun; //TODO: 新建一个函数
        FunTail(Fun);
        //TODO: 记录函数路径, 退出函数
    }
    else //普通变量定义
    {
        NorVarDef(t, ext, name, ptr); //TODO: 添加一个变量
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

        if(!match(RBRACKET)) recovery(isInFollow(FOLLOW{COMMA, SEMICON}), RBRACKET_LOST);

        //TODO: 这里需要返回一个数组变量
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
        VarDef(t, ext); // TODO: 这里需要添加一个变量
        VarList(t, ext);
    }
    else
    {
        if(isInFollow(FOLLOW{ID, MUL}))
        {
            recovery(true, COMMA_LOST);
            VarDef(t, ext); // TODO: 这里需要添加一个变量
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
    // TODO 这里需要添加参数变量
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
        if(mul)
        {
            //TODO 这里需要返回一个指针变量
        }
        else
        {
            return ParaTail(t, idName);
        }
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{COMMA, RPAREN}), IDNAME_LOST);
            //TODO 这里需要返回一个指针变量
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
        //TODO 这里需要新建一个变量并返回
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
        //TODO 这里需要新建一个变量并返回
    }
}

void Parser::ParaList(vector<Var*> &paraList)
{
    if(!match(COMMA)) return;
    Tag t = Type();
    Var* pr = ParaDef(t);
    // TODO 这里需要添加参数变量
    paraList.push_back(pr);
    ParaList(paraList);
}

void Parser::FunDef()
{}

void Parser::FunTail(Fun* Fun)
{
    if(match(SEMICON))
    {
        //TODO 添加一个声明函数
    }
    else
    {
        //TODO 添加一个定义函数
        FunBody();
        //TODO 结束函数定义
    }
}

void Parser::FunBody()
{
    if(!match(LBRACE)) 
        recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST, RBRACE}), LBRACE_LOST);
    FunProgram();
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), RBRACE_LOST);
}

void Parser::FunProgram()
{
    if(isInFollow(FOLLOW{TYPE_FIRST})) 
        LocalDef();
    else if(isInFollow(FOLLOW{STAT_FIRST, EXPR_FIRST}))
        Statement();
    FunProgram();
}

void Parser::LocalDef()
{
    Tag t = Type();
    VarDef(t, false); //TODO 这里需要添加变量
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
        //TODO 产生break语句
        move();
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else if(t == KW_CONTINUE)
    {
        //TODO 产生continue语句
        if(!match(SEMICON))
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
    }
    else if(t == KW_RETURN)
    {
        move();
        //TODO 产生return语句，注意可能有表达式
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
    //TODO 进入作用域
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, RPAREN}), LPAREN_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    //TODO 离开作用域
} 

void Parser::DoWhileStat()
{
    move();
    //TODO 进入作用域
    FunBody();
    //TODO 离开作用域
    if(!match(KW_WHILE))
        recovery(isInFollow(FOLLOW{LPAREN}), WHILE_LOST);
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, RPAREN}), LPAREN_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{SEMICON}), RPAREN_LOST);
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, KW_CASE, KW_DEFAULT, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
}

void Parser::ForStat()
{
    move();
    //TODO 进入作用域 
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{TYPE_FIRST, EXPR_FIRST, SEMICON}), LPAREN_LOST);
    ForInit();
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), SEMICON_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(SEMICON))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), SEMICON_LOST);
    altexpr();
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    FunBody();
    //TODO 退出作用域
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
    move();
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{EXPR_FIRST, RPAREN}), LPAREN_LOST);
    altexpr();
    //TODO 得到判断条件
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    //TODO 进入作用域
    FunBody();
    //TODO 退出作用域
    ElseStat();
}

void Parser::ElseStat()
{
    if(match(KW_ELSE))
    {
        //TODO 进入作用域
        FunBody();
        //TODO 退出作用域
    }
}

void Parser::SwitchStat()
{
    move();
    //TODO 进入作用域
    if(!match(LPAREN))
        recovery(isInFollow(FOLLOW{EXPR_FIRST}), LPAREN_LOST); //注意这里必须有表达式？
    expr();
    //TODO 得到表达式的值
    if(!match(RPAREN))
        recovery(isInFollow(FOLLOW{LBRACE}), RPAREN_LOST);
    if(!match(LBRACE))
        recovery(isInFollow(FOLLOW{KW_CASE, KW_DEFAULT}), LBRACE_LOST);
    CaseStat();
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), RBRACE_LOST);
    //TODO 离开作用域
}

void Parser::CaseStat()
{
    if(match(KW_CASE))
    {
        literal(); //TODO 得到常量表达式
        if(!match(COLON))
            recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST}), COLON_LOST);
        //TODO 进入作用域
        FunProgram();
        //TODO 退出作用域
        CaseStat();
    }
    else if(match(KW_DEFAULT))
    {
        if(!match(COLON))
            recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST}), COLON_LOST);
        //TODO 进入作用域
        FunProgram();
        //TODO 退出作用域
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

void Parser::ReadStat()
{}

void Parser::WriteStat(ing num)
{}
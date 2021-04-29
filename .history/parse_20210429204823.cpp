#include <vector>
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
#define STAT_FIRST KW_WHILE, KW_DO, KW_SWITCH, KW_CASE, KW_DEFAULT, KW_IF, KW_FOR, KW_SECLOUD, KW_READ, \
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

void Parser::program()
{
    segment();
    program();
}

void Parser::segment()
{
    bool ext = match(KW_EXTERN);
    Tag t = type();
    def(t, ext);
}

Tag Parser::type()
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

Var* Parser::init(Tag t, bool ext, string name, bool ptr)
{
    Var* val = NULL;
    if(match(ASSIGN)) val = expr();
    //TODO 这里需要新建一个变量并返回
}

void Parser::def(Tag t, bool ext)
{
    bool mul = match(MUL);
    string idName = ""; //默认为空
    if(lookahead->tag == ID) 
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(!mul) deftail(t, ext, idName, 0);
        else
        {
            init(t, ext, idName, 1); //TODO:添加一个指针变量
            varlist(t, ext);
        } 
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA}), IDNAME_LOST);
            init(t, ext, idName, 1); //TODO:添加一个指针变量
            varlist(t, ext);
        }
        else
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA, LPAREN, LBRACKET}), IDNAME_LOST);
            deftail(t, ext, idName, 0);
        }
    }
}

void Parser::deftail(Tag t, bool ext, string name, bool ptr)
{
    if(match(LPAREN)) //函数
    {
        //TODO: 记录函数路径, 进入函数
        Tag t = type();
        vector<Var*> paraList; //参数列表
        para(paraList);
        if(!match(RPAREN)) recovery(isInFollow(FOLLOW{SEMICON, LBRACE}), RPAREN_LOST);
        Fun* fun; //TODO: 新建一个函数
        funtail(fun);
        //TODO: 记录函数路径, 退出函数
    }
    else //普通变量定义
    {
        norvardef(t, ext, name, ptr); //TODO: 添加一个变量
        varlist(t, ext);
    }
}

Var* Parser::norvardef(Tag t, bool ext, string name, bool ptr)
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
        return init(t, ext, name, ptr);
    }
}

void Parser::varlist(Tag t, bool ext)
{
    if(match(SEMICON)) return;

    if(match(COMMA))
    {
        vardef(t, ext); // TODO: 这里需要添加一个变量
        varlist(t, ext);
    }
    else
    {
        if(isInFollow(FOLLOW{ID, MUL}))
        {
            recovery(true, COMMA_LOST);
            vardef(t, ext); // TODO: 这里需要添加一个变量
            varlist(t, ext);
        }
        else
        {
            recovery(isInFollow(FOLLOW{KW_EXTERN, RBRACE, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}), SEMICON_LOST);
        }
    }
}

Var* Parser::vardef(Tag t, bool ext)
{
    bool mul = match(MUL);
    string idName = "";
    if(lookahead->tag == ID)
    {
        idName = ((Id*)lookahead)->id;
        move();
        if(!mul) return norvardef(t, ext, idName, mul);
        else return init(t, ext, idName, mul);
    }
    else
    {
        if(!mul)
        {
            recovery(isInFollow(FOLLOW{LBRACKET, ASSIGN, COMMA, SEMICON}), IDNAME_LOST);
            return norvardef(t, ext, idName, mul);
        }
        else
        {
            recovery(isInFollow(FOLLOW{ASSIGN, COMMA, SEMICON}), IDNAME_LOST);
            return init(t, ext, idName, mul);
        }
    }
}

void Parser::para(vector<Var*> &paraList)
{
    if(lookahead->tag == RPAREN) return;
    Tag t = type();
    Var* pr = paradef(t);
    // TODO 这里需要添加参数变量
    paraList.push_back(pr);
    paralist(paraList);

}

Var* Parser::paradef(Tag t)
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
            return paratail(t, idName);
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
            return paratail(t, idName);
        }
    }
}

Var* Parser::paratail(Tag t, string name)
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

void Parser::paralist(vector<Var*> &paraList)
{
    if(!match(COMMA)) return;
    Tag t = type();
    Var* pr = paradef(t);
    // TODO 这里需要添加参数变量
    paraList.push_back(pr);
    paralist(paraList);
}

void Parser::funtail(Fun* fun)
{
    if(match(SEMICON))
    {
        //TODO 添加一个声明函数
    }
    else
    {
        //TODO 添加一个定义函数
        funbody();
        //TODO 结束函数定义
    }
}

void Parser::funbody()
{
    if(!match(LBRACE)) 
        recovery(isInFollow(FOLLOW{TYPE_FIRST, STAT_FIRST, RBRACE}), LBRACE_LOST);
    fungrogram();
    if(!match(RBRACE))
        recovery(isInFollow(FOLLOW{KW_EXTERN, TYPE_FIRST, STAT_FIRST, EXPR_FIRST}, RBRACE_LOST));
}
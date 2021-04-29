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
#define EXPR
#define TYPE_FIRST KW_INT, KW_CHAR, KW_VOID
#define STAT_FIRST KW_WHILE, KW_DO, KW_SWITCH, KW_CASE, KW_DEFAULT, KW_IF, KW_ELSE, KW_FOR, KW_SECLOUD, KW_READ, KW_WRITE

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
            init(t, ext, idName, 1); //TODO:新建一个指针变量
            varlist(t, ext);
        } 
    }
    else
    {
        if(mul)
        {
            recovery(isInFollow(FOLLOW{ASSIGN, SEMICON, COMMA}), IDNAME_LOST);
            init(t, ext, idName, 1); //TODO:新建一个指针变量
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
        //TODO: 新建一个函数
        funtail();
        //TODO: 记录函数路径, 退出函数
    }
    else //普通变量定义
    {
        norvardef(t, ext, name, ptr); //TODO: 新建一个变量
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
        vardef(t, ext); // TODO: 这里需要新建一个变量
        varlist(t, ext);
    }
    else
    {
        if(isInFollow(FOLLOW{ID, MUL}))
        {
            recovery(true, COMMA_LOST);
            vardef(t, ext); // TODO: 这里需要新建一个变量
            varlist(t, ext);
        }
        else
        {
            recovery()
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


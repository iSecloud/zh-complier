#pragma once
#include "token.h"
#include "error.h"

class Scanner
{
    const int BUFFERLEN = 51212; //缓冲区长度
    int rowNum, colNum; //当前行号和列号
    int nowPos, lineNum; //当前读的位置与行长度
    //char lastChar, nowChar; //
    //Error &er; //错误类
    char charArray[51212]; //字符数组
    FILE *file; //文件指针

public: 
    //构造函数与析构函数
    Scanner(const char *fname, Error &er1);
    ~Scanner();
    
    char scan(); //读取字符
    std::pair<int, int> getRowCol(); //返回行列号 方便错误处理
};

class Lex
{
    Error &er; //错误类
    Keywords key; //关键字列表
    Scanner &sc; //定义一个扫描器引用
    char ch; // 记录当前读的字符
    Token *token; //令牌

public:
    Lex(Scanner &sc1, Error &er1);
    ~Lex();

    bool scan(char need);

    void debug(); //调试

    Token *getToken();
};

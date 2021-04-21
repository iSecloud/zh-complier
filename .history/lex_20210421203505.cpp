#include "lex.h"
#include "error.h"

Scanner::Scanner(const char *fname)
{
    file = fopen(fname, "r");
    if(!file) 
    {
        Error::printErrorInfo(make_pair(-1, -1), FILE_OPEN_ERROR);
        exit(0);
        //这里需要错误处理
    }
    rowNum = 1; colNum = 0;
    //nowChar = lastChar = '';
    nowPos = lineNum = 0;
}

Scanner::~Scanner()
{
    if(file)
    {
        Error::printErrorInfo(make_pair(-1, -1), FILE_OPEN_ERROR);
        exit(0);
        //这里需要错误处理
    }
}

char Scanner::scan()
{ 
    if(!file) return 0; 
    if(nowPos == lineNum) //需要新读一行
    {
        lineNum = fread(charArray, 1, BUFFERLEN, file);
        if(!lineNum) //文件读完了
        {
            //printf("+++++++++++++\n");
            charArray[0] = -1;
        }
        nowPos = 0;
    }
    char ch = charArray[nowPos ++];
    if(ch == -1)
    {
        fclose(file);
        file = NULL;
    }
    else if(ch == '\n')
    {
        rowNum ++;
        colNum = 0;
    }
    else colNum ++;
    return ch;
}

std::pair<int, int> Scanner::getRowCol()
{
    return std::make_pair(rowNum, colNum);
}

Lex::Lex(Scanner &sc1): sc(sc1)
{
    ch = 0;
    token = NULL;
}

Lex::~Lex() 
{
    delete token; //释放内存
}

void Lex::debug()
{
    printf("now char is %c", ch);
    system("pause");
}

bool Lex::scan(char need = 0)
{
    ch = sc.scan();
    if(!need) return true;
    if(need == ch) 
    {
        ch = sc.scan();
        return true;
    }
    return false;
}

Token *Lex::getToken()
{
    while(ch != -1)
    {
        if(token) 
        {
            //debug();
            delete token; //释放空间
        }
        //scan();
        //忽略空白符
        while(ch == 0 || ch == ' ' || ch == '\n' || ch == '\t') scan(); 
        //标识符
        if(ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        {
            string s = "";
            while(ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) s += ch, scan();
            int k = key.getTag(s);
            if(k) token = new Token((enum Tag)k); //是一个关键字
            else token = new Id(s); //一个普通id
        }
        //数字
        else if(ch >= '0' && ch <= '9')
        {
            //debug();
            int x = 0;
            while(ch >= '0' && ch <= '9') x = x * 10 + ch - '0', scan();
            token = new Num(x);
        }
        //字符
        else if(ch == '\'')
        {
            scan();
            char c = -1;
            if(ch == -1 || ch == '\n' || ch == '\'') 
            {
                er.printErrorInfo(sc.getRowCol(), ILLEGAL_CHARACTER);
                exit(0);
                //这里需要错误处理
            }
            else if(ch == '\\')
            {
                scan();
                if(ch == -1 || ch == '\n') 
                {
                    er.printErrorInfo(sc.getRowCol(), ILLEGAL_CHARACTER);
                    exit(0);
                    //这里需要错误处理
                }
                else if(ch == 'n') c = '\n';
                else if(ch == '\\') c = '\\';
                else if(ch == 't') c = '\t';
                else if(ch == '0') c = '\0';
                else if(ch == '\'') c = '\'';
                else c = ch;
                scan();
                if(ch != '\'')
                {
                    er.printErrorInfo(sc.getRowCol(), ILLEGAL_CHARACTER);
                    exit(0);
                    //这里需要错误处理
                }
                else token = new Char(c), scan(); //读取下一个字符
            }
            else
            {
                c = ch;
                scan();
                if(ch != '\'')
                {
                    er.printErrorInfo(sc.getRowCol(), ILLEGAL_CHARACTER);
                    exit(0);
                    //这里需要错误处理
                }
                else token = new Char(c), scan(); //读取下一个字符
            }
        }
        //字符串
        else if(ch == '\"')
        {
            scan();
            string s = "";
            while(ch != '\"')
            {
                if(ch == '\n' || ch == -1)
                {
                    er.printErrorInfo(sc.getRowCol(), ILLEGAL_STRING);
                    exit(0);
                    //这里需要错误处理
                }
                else if(ch == '\\')
                {
                    scan();
                    if(ch == '\n' || ch == -1)
                    {
                        er.printErrorInfo(sc.getRowCol(), ILLEGAL_STRING);
                        exit(0);
                        //这里需要错误处理
                    }
                    else if(ch == 'n') s += '\n';
                    else if(ch == '\\') s += '\\';
                    else if(ch == 't') s += '\t';
                    else if(ch == '0') s += '\0';
                    else if(ch == '\"') s += '\"';
                }
                else s += ch;
                scan();
            }
            token = new Str(s);
            scan(); //读取下一个字符
        }
        //界符
        else if(ch == '#') //暂时忽略宏定义
        {
            while(ch != '\n' && ch != -1) scan();
            token = new Token(ERR); //返回一个空？可能需要继续处理
        } 
        else if(ch == '+') token = new Token(scan('+') ? INC : ADD);
        else if(ch == '-') token = new Token(scan('-') ? DEC : SUB);
        else if(ch == '*') token = new Token(MUL), scan();
        else if(ch == '/') //处理除法或者注释
        {
            scan();
            if(ch == '/') //单行注释
            {
                while(ch != '\n' && ch != -1) scan();
                token = new Token(ERR); //返回一个空？可能需要继续处理
            }
            else if(ch == '*') //多行注释
            {
                char lastCh;
                scan();
                lastCh = ch;
                while(lastCh != '*' || ch != '/')
                {
                    //debug();
                    if(ch == -1)
                    {
                        er.printErrorInfo(sc.getRowCol(), ILLEGAL_COMMENT);
                        exit(0);
                        //这里需要错误处理
                    }
                    else lastCh = ch, scan();
                }
                token = new Token(ERR), scan(); //返回一个空？可能需要继续处理
            }
            else token = new Token(DIV), scan();
        }
        else if(ch == '&') token = new Token(scan('&') ? AAND : AND);
        else if(ch == '|') token = new Token(scan('|') ? OOR : OR);
        else if(ch == '!') token = new Token(scan('=') ? NEQU : NNOT);
        else if(ch == '>') token = new Token(scan('=') ? GE : GT);
        else if(ch == '<') token = new Token(scan('=') ? LE : LT);
        else if(ch == '=') token = new Token(scan('=') ? EQU : ASSIGN);
        else if(ch == '^') token = new Token(XOR), scan();
        else if(ch == '~') token = new Token(NOT), scan();
        else if(ch == ',') token = new Token(COMMA), scan();
        else if(ch == ':') token = new Token(COLON), scan();
        else if(ch == ';') token = new Token(SEMICON), scan();
        else if(ch == '(') token = new Token(LPAREN), scan();
        else if(ch == ')') token = new Token(RPAREN), scan();
        else if(ch == '[') token = new Token(LBRACKET), scan();
        else if(ch == ']') token = new Token(RBRACKET), scan();
        else if(ch == '{') token = new Token(LBRACE), scan();
        else if(ch == '}') token = new Token(RBRACE), scan();
        else token = new Token(ERR), scan(); //读到其他未知字符 暂时忽略
        
        if(token != NULL && token->tag != ERR) return token; //返回一个有效的token
    }
    //debug();
    if(token) delete token; //释放空间
    return token = new Token(END); //文件结束
}
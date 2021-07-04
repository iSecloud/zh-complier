#pragma once
#include "intercode.h"
#include "common.h"

class Block
{
    ~Block();
public:
    Block(vector<Quaternion* > code);
    list<Quaternion* > blockCode; //基本块中间代码序列
    list<Block* > preBlock; //块的前驱
    list<Block* > nextBlock; //块的后继
    vector<double > inVals; //in值集合
    vector<double > outVals; //out值集合
    bool visit;
    bool isreach;
};

class DFG
{   
    ~DFG();
public:
    DFG(InterCode& code);
    vector<Quaternion* > intercode;
    vector<Block* > blocks;
    void createBlock(); //创建数据块
    void linkBlock(); //链接数据块
    void delBlock(Block* begin, Block* end); //删除数据块
    void release(Block* block); //递归删除
    bool reachable(Block* block); //判断是否可达
    bool reach(Block* block);
    void getCode(list<Quaternion*> &optcode); //获得优化后的代码
    void getBlockInfo();
};
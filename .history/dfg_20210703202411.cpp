#include "dfg.h"

Block::Block(vector<Quaternion* > code, int id)
{
    visit = false;
    isreach = true; //假设每个数据块都可达
    this->id = id;
    for(auto i: code)
    {
        i->setBlock(this); //把指令标记为本块
        blockCode.push_back(i);
    }
}

Block::~Block() 
{}

DFG::DFG(InterCode& code)
{
    code.makeFirst(); //表示首指令
    intercode = code.getInterCode();
    createBlock(); //创建基本块
}

DFG::~DFG()
{}

void DFG::createBlock()
{
    int id = 0;
    vector<Quaternion* > tmpCode;
    tmpCode.push_back(intercode[0]);
    for(int i = 1; i < intercode.size(); i ++)
    {
        if(intercode[i]->getFirst())
        {
            blocks.push_back(new Block(tmpCode, ++ id));
            tmpCode.clear();
        }
        tmpCode.push_back(intercode[i]);
    }
    blocks.push_back(new Block(tmpCode, ++ id));
}

void DFG::linkBlock()
{
    for(int i = 0; i < blocks.size(); i ++)
    {
        Quaternion* last = blocks[i]->blockCode.back();
        if(last->isJmp() || last->isJmpCond()) //跳转指令
        {
            Block* tarBlock = last->getTarget()->getBlock();
            blocks[i]->nextBlock.push_back(tarBlock);
            tarBlock->preBlock.push_back(blocks[i]);
        }
        if(last->isJmp() && i != blocks.size() - 1)
        {
            blocks[i]->nextBlock.push_back(blocks[i + 1]);
            blocks[i + 1]->preBlock.push_back(blocks[i]);
        }
    }
}

void DFG::delBlock(Block* begin, Block* end)
{
    //解除数据块关联关系
    if(begin)
    {
        //begin->nextBlock.erase(find(begin->nextBlock.begin(), begin->nextBlock.end(), end));
        //end->preBlock.erase(find(end->preBlock.begin(), end->preBlock.end(), begin));
        begin->nextBlock.remove(end);
        end->preBlock.remove(begin);
    }
    release(end);
}

void DFG::release(Block* block)
{
    if(!reachable(block))
    {
        list<Block*> delList;
        for(auto next: block->nextBlock)
            delList.push_back(next);
        for(auto next: delList)
        {
            //next->preBlock.erase(next->preBlock.begin(), next->preBlock.end(), block);
            //block->nextBlock.erase(block->nextBlock.begin(), block->nextBlock.end(), next);
            next->preBlock.remove(block);
            block->nextBlock.remove(next);
        }
        for(auto next: delList)
            release(next);
    }
}

bool DFG::reachable(Block* block)
{
    for(auto blk: blocks) blk->visit = false;
    return reach(block);
}
    
bool DFG::reach(Block* block)
{
    if(block == blocks[0]) return true; //Entry
    if(block->visit == true) return false;
    block->visit = true; //标记已经访问过
    bool isReach = false;
    for(auto pre: block->preBlock)
    {
        isReach = reach(pre);
        if(isReach) break;
    }
    return isReach;
}

void DFG::getCode(list<Quaternion*> &optcode)
{
    for(auto blk: blocks) blk->visit = false;
    for(auto block: blocks)
    {
        list<Quaternion*> tmp;
        if(reachable(block))
        {
            for(auto code: block->blockCode)
                tmp.push_back(code);
            optcode.splice(optcode.end(), tmp);
        }
        else  
        {
            //记录数据块不可达
            block->isreach = false;
        }
    }
}

void DFG::getBlockInfo()
{
    printf("The block's num is: %d\n", blocks.size());
    for(auto block: blocks)
    {
        printf("----------Block id: %d----------\n", block->id);

        printf("The code is:\n");
        for(auto code: block->blockCode)
            code->toString();

        printf("The block's pre is:\n");
        for(auto bk: block->preBlock)
            printf("%d ", bk->id);

        printf("The block's next is:\n");
        for(auto bk: block->nextBlock)
            printf("%d ", bk->id);
    }
}
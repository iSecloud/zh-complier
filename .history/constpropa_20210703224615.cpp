#include "constpropa.h"
#include "token.h"

double ConstPropagation::UNDEF = 0.1;
double ConstPropagation::NAC = -0.1;

ConstPropagation::~ConstPropagation()
{}

ConstPropagation::ConstPropagation
(SymTab* symtab1, DFG* dfg1, vector<Var*>&paraVars):symtab(symtab1), dfg(dfg1)
{
    glbVars = symtab->getGlbVar(); //得到全局变量 Pro
    int index = 0;
    //处理全集变量集合
    for(int i = 0; i < glbVars.size(); i ++)
    {
        Var* var = glbVars[i];
        double val = 0; //未初始化全局变量为0
        var->index = index ++;
        vars.push_back(var);
        if(!var->isBaseType()) val = NAC;
        else if(var->isInitVar()) val = var->getVal();
        boundVals.push_back(val);
    }
    //处理参数变量集合
    for(int i = 0; i < paraVars.size(); i ++)
    {
        Var* var = paraVars[i];
        var->index = index ++;
        vars.push_back(var);
        boundVals.push_back(NAC); //默认参数都是可能会改变的
    }
    //处理局部变量集合
    for(int i = 0; i < dfg->intercode.size(); i ++)
    {
        if(!dfg->intercode[i]->isDec()) continue;
        Var* var = dfg->intercode[i]->getArg1();
        var->index = index ++;
        vars.push_back(var);
        double val = UNDEF;
        if(!var->isBaseType()) val = NAC;
        else if(var->isInitVar()) val = var->getVal();
        boundVals.push_back(val);
    }
    //将初值集合元素全部初始化为UNDEF
    for(int i = 0; i < index; i ++)
        initVals.push_back(UNDEF);

    // printf("The var init is:\n");
    // for(auto var: boundVals) cout << var << endl;
    // system("pause");
}

// 进行变量值的交汇运算
double ConstPropagation::joinVar(double left, double right)
{
    if(left == NAC || right == NAC) return NAC;
    else if(left == UNDEF) return right;
    else if(right == UNDEF) return left;
    else if(left == right) return left;
    else return NAC;
}

// 数据块交汇运算 B.in = f(B.out)
void ConstPropagation::joinBlock(Block* block)
{
    list<Block* > &pre = block->preBlock;
    vector<double > &in = block->inVals;
    for(int i = 0; i < in.size(); i ++)
    {
        double val = UNDEF;
        for(auto blk: pre)
            val = joinVar(val, blk->outVals[i]);
        in[i] = val;
    }
}

void ConstPropagation::transferVar(Quaternion* code, vector<double> &in, vector<double> &out)
{
    out = in; //默认直接传播
    Var* arg1 = code->getArg1();
    Var* arg2 = code->getArg2();
    Var* res = code->getResult();
    Operator op = code->getOp();
    //一元运算
    if(op == OP_ASSIGN || op == OP_NOT || op == OP_INV || op == OP_NEG)
    {
        double tmp = 0;
        if(arg1->isConst()) tmp = arg1->getVal();
        else tmp = in[arg1->index];
        if(tmp != UNDEF && tmp != NAC)
        {
            if(op == OP_NOT) tmp = !tmp;
            else if(op == OP_INV) tmp = ~((int)tmp);
            else if(op == OP_NEG) tmp = -tmp;
        }
        out[res->index] = tmp;
    }
    //二元运算
    else if((op >= OP_ADD && op <= OP_MOD) || (op >= OP_AND && op <= OP_NEQU))
    {
        double lval, rval, resval;
        double eps = 1e-7;

        if(arg1->isConst()) lval = arg1->getVal();
        else lval = in[arg1->index];
        if(arg2->isConst()) rval = arg2->getVal();
        else rval = in[arg2->index];

        if(lval == NAC || rval == NAC) resval = NAC;
        else if(lval == UNDEF || rval == UNDEF) resval = UNDEF;
        else if(op == OP_ADD) resval = lval + rval;
        else if(op == OP_SUB) resval = lval - rval;
        else if(op == OP_MUL) resval = lval * rval;
        else if(op == OP_DIV)
        {
            if(fabs(rval) < eps) resval = NAC; //不合法
            else resval = (int)lval / (int)(rval);
        }
        else
        {
            int lv = lval, rv = rval;
            if(op == OP_AND) resval = lv & rv;
            else if(op == OP_OR) resval = lv | rv;
            else if(op == OP_XOR) resval = lv ^ rv;
            else if(op == OP_AAND) resval = lv && rv;
            else if(op == OP_OOR) resval = lv || rv;
            else if(op == OP_GE) resval = lv > rv;
            else if(op == OP_GT) resval = lv >= rv;
            else if(op == OP_LE) resval = lv < rv;
            else if(op == OP_LT) resval = lv <= rv;
            else if(op == OP_EQU) resval = lv == rv;
            else if(op == OP_NEQU) resval = lv != rv;
        }

        out[res->index] = resval;
    }
    //GET 保守认为x = *y x会变
    else if(op == OP_GET) out[res->index] = NAC;
    //SET 保守认为*y = x *y会影响全局
    else if(op == OP_SET || (op == OP_ARG && !arg1->isBaseType()))
    {
        for(int i = 0; i < vars.size(); i ++)
            out[i] = NAC;
    }
    //函数调用 保守认为函数调用会影响全局变量
    else if(op == OP_CALL || op == OP_PROC)
    {
        for(int i = 0; i < glbVars.size(); i ++)
            out[glbVars[i]->index] = NAC;
        if(op == OP_CALL) out[res->index] = NAC;
    }
    //TODO 将in out赋值给指令的in集和out集
    code->inVals = in;
    code->outVals = out;
}

//进行数据块的传播，即B.out = fb(B.in)
bool ConstPropagation::transferBlk(Block* block)
{
    vector<double> in = block->inVals;
    vector<double> out = in;
    for(auto code: block->blockCode) // 对每一条指令进行传播
    {
        code->toString(); 
        transferVar(code, in, out);
        in = out;
    }
    bool isChange = true; //假设没有改变
    for(int i = 0; i < block->outVals.size(); i ++)
        if(block->outVals[i] != out[i])
        {
            isChange = false;
            break;
        }
    block->outVals = out;
    return isChange;
}

void ConstPropagation::analysis()
{
    dfg->blocks[0]->outVals = boundVals; //Entry = V
    for(int i = 1; i < dfg->blocks.size(); i ++)
    {
        dfg->blocks[i]->outVals = initVals; //初始其他block为UNDEF
        dfg->blocks[i]->inVals = initVals; //初始其他block为UNDEF
    }
    bool isChange = true;
    while(isChange)
    {   
        bool change = true;
        for(int i = 1; i < dfg->blocks.size(); i ++)
        {
            joinBlock(dfg->blocks[i]);
            change &= transferBlk(dfg->blocks[i]);
        }
        isChange = !change;
    }
}

void ConstPropagation::algSimple()
{
    for(auto block: dfg->blocks)
    {
        //得到当前数据块的指令流, 进行代数化简
        for(auto code: block->blockCode)
        {
            Operator op = code->getOp();
            Var* res = code->getResult();
            Var* arg1 = code->getArg1();
            Var* arg2 = code->getArg2();
            double tmp = code->outVals[res->index];
            Token* tk = new Num(0);
            if(op >= OP_ASSIGN && op <= OP_NEQU)
            {
                // 结果是常量 可以直接替换
                if(tmp != UNDEF && tmp != NAC)
                {
                    ((Num*)tk)->num = (int)tmp;
                    Var* constNum = new Var(tk);
                    symtab->addVar(constNum);
                    code->replace(OP_ASSIGN, res, constNum);
                }
                //只对二元运算进行化简
                else if((op >= OP_ADD && op <= OP_MOD) || (op >= OP_AND && op <= OP_OOR))
                {
                    double lval, rval; 
                    int lv, rv; //记录整数值
                    bool constl = false, constr = false; //是否是常量

                    if(arg1->isConst()) lval = arg1->getVal();
                    else lval = code->inVals[arg1->index];
                    if(arg2->isConst()) rval = arg2->getVal();
                    else rval = code->inVals[arg2->index];
                    //左右参数是否可以进行代数化简
                    if(lval != UNDEF && lval != NAC)
                        lv = lval, constl = true;
                    if(rval != UNDEF && rval != NAC)
                        rv = rval, constr = true;
                    
                    Var* newArg1 = NULL; 
                    Var* newArg2 = NULL;
                    Operator newOp = OP_ASSIGN;//代数化简默认是赋值

                    if(op == OP_ADD)
                    {
                        if(constl && lv == 0) newArg1 = arg2;
                        if(constr && rv == 0) newArg2 = arg1;
                    }
                    else if(op == OP_SUB)
                    {
                        if(constl && lv == 0) newArg1 = arg2, op = OP_NEG;
                        if(constr && rv == 0) newArg1 = arg1;
                    }
                    else if(op == OP_MUL)
                    {
                        if(constl && !lv || constr && !rv) newArg1 = SymTab::zeroVar;
                        if(constl && lv == 1) newArg1 = arg2;
                        if(constr && rv == 1) newArg1 = arg1;
                    }
                    else if(op == OP_DIV)
                    {
                        if(constl && !lv) newArg1 = SymTab::zeroVar;
                        if(constr && rv == 1) newArg1 = arg1;
                    }
                    else if(op == OP_MOD)
                    {
                        if(constl && !lv) newArg1 = SymTab::zeroVar;
                        if(constr && rv == 1) newArg1 = SymTab::zeroVar;
                    }
                    else if(op == OP_AND)
                    {
                        if(constl && !lv || constr && !rv) newArg1 = SymTab::zeroVar;
                    }
                    else if(op == OP_OR || op == OP_XOR)
                    {
                        if(constl && !lv) newArg1 = arg2;
                        if(constr && !rv) newArg1 = arg1;
                    }
                    else if(op == OP_AAND)
                    {
                        if(constl && !lv || constr && !rv) newArg1 = SymTab::zeroVar;
                        if(constl && lv == 1) op = OP_NEQU, newArg1 = arg1, newArg2 = SymTab::zeroVar;
                        if(constr && rv == 1) op = OP_NEQU, newArg1 = arg2, newArg2 = SymTab::zeroVar;
                    }
                    else if(op == OP_OOR)
                    {
                        if((constl && lv == 1) || (constr && rv == 1)) newArg1 = SymTab::oneVar;
                        if(constl && !lv) op = OP_NEQU, newArg1 = arg2, newArg2 = SymTab::zeroVar;
                        if(constr && !rv) op = OP_NEQU, newArg1 = arg1, newArg2 = SymTab::zeroVar;
                    }
                    //代数化简成功
                    if(newArg1 != NULL)
                        code->replace(op, res, newArg1, newArg2);
                    //没有成功化简，则正常的常量传播
                    else
                    {
                        newArg1 = arg1; newArg2 = arg2;
                        if(constl)
                        {
                            ((Num*)tk)->num = lv;
                            newArg1 = new Var(tk);
                            symtab->addVar(newArg1);
                            newArg2 = arg2;
                        }
                        if(constr)
                        {
                            ((Num*)tk)->num = rv;
                            newArg2 = new Var(tk);
                            symtab->addVar(newArg2);
                            newArg1 = arg1;
                        }
                        //完成后替换中间代码
                        code->replace(op, res, newArg1, newArg2);
                    }
                }
            }
            else if(op == OP_ARG || op == OP_RETX)
            {
                Var* arg1 = code->getArg1();
                if(!arg1->isConst())
                {
                    double tmp = code->outVals[arg1->index];
                    if(tmp != UNDEF && tmp != NAC)
                    {
                        Token* it = new Num((int)tmp);
                        Var* newVar = new Var(it);
                        symtab->addVar(newVar);
                        code->setArg1(newVar);
                    }
                }
            }
        }
    }
}

void ConstPropagation::jCondSimple()
{
    for(int bk = 0; bk < dfg->blocks.size(); bk ++)
    {
        Block* block = dfg->blocks[bk];
        list<Quaternion*>::iterator i, k; //设计i, k的作用是防止删除代码后找不到下一个对象
        for(i = block->blockCode.begin(), k = i; i != block->blockCode.end(); i = k)
        {
            k ++;
            Quaternion* code = (*i);
            Operator op = code->getOp();
            if(op == OP_JF || op == OP_JT)
            {
                Quaternion* target = code->getTarget();
                Var* arg1 = code->getArg1();
                double cond;
                if(arg1->isConst()) cond = arg1->getVal();
                else cond = code->outVals[arg1->index];
                if(cond == UNDEF || cond == NAC) continue; //无法优化
                if(op == OP_JT && !cond || op == OP_JF && cond)
                {
                    block->blockCode.remove(code);
                    if(target->getBlock() != dfg->blocks[bk + 1])
                        dfg->delBlock(block, target->getBlock());
                }
                else if(op == OP_JT && cond || op == OP_JF && !cond)
                {
                    code->replace(OP_JMP, target);
                    if(target->getBlock() != dfg->blocks[bk + 1])
                        dfg->delBlock(block, dfg->blocks[bk + 1]);
                }
            }
        }
    }
}

void ConstPropagation::propagate()
{
    analysis();
    for(auto block: dfg->blocks)
    {
        printf("The code is:\n");
        for(auto code: block->blockCode)
        {
            code->toString();
            printf("in %d out %d\n", code->inVals.size(), code->outVals.size());
        }
        printf("The outVals is:\n");
        for(auto val: block->outVals)
            cout << val << " ";
        system("pause");
    }
    algSimple();
    printf("I finish algSimple\n");
    jCondSimple();
    printf("I finish jCondSimple\n");
}
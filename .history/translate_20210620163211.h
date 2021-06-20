#pragma once
#include "common.h"
#include "obj.h"
#include "armplat.h"

class Translate
{
    vector<Quaternion*> &intercode;
    Obj& obj;
    void translate(Quaternion* code);
public:
    Translate(vector<Quaternion*> &intercode, Obj& obj)
};
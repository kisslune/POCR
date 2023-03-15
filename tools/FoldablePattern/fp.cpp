/* -------------------- fp.cpp ------------------ */
//
// Created by kisslune on 3/4/23.
//

#include <iostream>
#include "RSM/GFPattern.h"

using namespace SVF;
using namespace std;


int main(int argc, char** argv)
{
    GFPattern* p = new GFPattern();

    p->parseRSM(argv[1]);
    p->identify(argv[2]);

    return 0;
}
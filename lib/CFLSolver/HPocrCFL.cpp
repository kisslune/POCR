/* -------------------- HPocrCFL.cpp ------------------ */
//
// Created by kisslune on 3/30/23.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;


void HPocrCFL::solve()
{
    while (!primaryList.empty())
    {
        CFLItem item = primaryList.pop();
        procPrimaryItem(item);
    }


}
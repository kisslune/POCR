/*
 // Author: Kisslune
 */

#include "SVF-LLVM/LLVMUtil.h"
#include "CFLSolver/CFLSolver.h"

using namespace SVF;

static Option<bool> Default_CFL("std", "Standard CFL-reachability analysis", false);
static Option<bool> Pocr_CFL("pocr", "POCR CFL-reachability analysis", false);
static Option<bool> HPocr_CFL("hpocr", "Hierarchical POCR CFL-reachability analysis", false);
static Option<bool> Focr_CFL("focr", "Uni-directional CFL-reachability analysis", false);
static Option<bool> Tr_CFL("tr", "Uni-directional CFL-reachability analysis", false);


int main(int argc, char** argv)
{
    int arg_num = 0;
    char** arg_vec = new char* [argc];
    std::vector<std::string> moduleNameVec;
    std::vector<std::string> inFileVec;
    processArgs(argc, argv, arg_num, arg_vec, inFileVec);
    OptionBase::parseOptions(arg_num, arg_vec, "CFL-reachability analysis\n", "[options] <input>");

    StdCFL* cfl;

    if (Default_CFL())
    {
        cfl = new StdCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }
    else if (Pocr_CFL())
    {
        cfl = new PocrCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }
    else if (HPocr_CFL())
    {
        cfl = new HPocrCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }
    else if (Focr_CFL())
    {
        cfl = new FocrCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }
    else if (Tr_CFL())
    {
        cfl = new TRCFL(inFileVec[0],inFileVec[1]);
        cfl->analyze();
    }
    else
    {
        cfl = new StdCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }


    return 0;
}
/*
 // Author: Kisslune
 */

#include "SVF-LLVM/LLVMUtil.h"
#include "CFLSolver/CFLSolver.h"

using namespace SVF;

static llvm::cl::opt<bool> Default_CFL("std", llvm::cl::init(false), llvm::cl::desc("Standard alias analysis"));
static llvm::cl::opt<bool> Pocr_CFL("pocr", llvm::cl::init(false), llvm::cl::desc("POCR alias analysis"));


int main(int argc, char** argv)
{
    int arg_num = 0;
    char** arg_vec = new char* [argc];
    std::vector<std::string> moduleNameVec;
    std::vector<std::string> inFileVec;
    CFLBase::processArgs(argc, argv, arg_num, arg_vec, inFileVec);
    llvm::cl::ParseCommandLineOptions(arg_num, arg_vec, "CFL-reachability analysis\n");

    StdCFL* cfl;

    if (Default_CFL) {
        cfl = new StdCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }
    else if (Pocr_CFL) {
        cfl = new PocrCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }
    else {
        cfl = new StdCFL(inFileVec[0], inFileVec[1]);
        cfl->analyze();
    }


    return 0;
}
/*
 // Valueflow analysis
 //
 // Author: Kisslune
 */

#include "SVF-FE/LLVMUtil.h"
#include "CFLSolver/VFAnalysis.h"

using namespace SVF;

static llvm::cl::opt<bool> Default_VFA("std", llvm::cl::init(false), llvm::cl::desc("Standard valueflow analysis"));
static llvm::cl::opt<bool> Pocr_VFA("pocr", llvm::cl::init(false), llvm::cl::desc("POCR valueflow analysis"));
static llvm::cl::opt<bool> Gspan_VFA("gspan", llvm::cl::init(false), llvm::cl::desc("POCR valueflow analysis"));
static llvm::cl::opt<bool> Gr_VFA("gr", llvm::cl::init(false), llvm::cl::desc("POCR valueflow analysis"));
static llvm::cl::opt<bool> GrGspan_VFA("grgspan", llvm::cl::init(false), llvm::cl::desc("POCR valueflow analysis"));


int main(int argc, char** argv)
{

    int arg_num = 0;
    char** arg_value = new char* [argc];
    std::vector<std::string> moduleNameVec;
    std::vector<std::string> inFileVec;
    CFLBase::processArgs(argc, argv, inFileVec);

    LLVMUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    llvm::cl::ParseCommandLineOptions(arg_num, arg_value,
                                      "Alias analysis\n");

    StdVFA* vfa;
    if (Default_VFA) {
        vfa = new StdVFA(inFileVec[0]);
        vfa->analyze();
    }
    else if (Pocr_VFA) {
        vfa = new PocrVFA(inFileVec[0]);
        vfa->analyze();
    }
    else if (Gspan_VFA) {
        vfa = new GspanVFA(inFileVec[0]);
        vfa->analyze();
    }
    else if (Gr_VFA) {
        vfa = new GRVFA(inFileVec[0]);
        vfa->analyze();
    }
    else if (GrGspan_VFA) {
        vfa = new GRGspanVFA(inFileVec[0]);
        vfa->analyze();
    }
    else
        cout << "No valueflow analysis solver specified!" << endl;

    return 0;
}
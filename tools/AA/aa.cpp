/*
 // Alias analysis
 //
 // Author: Kisslune
 */

#include "SVF-FE/LLVMUtil.h"
#include "AA/AliasAnalysis.h"

using namespace SVF;

static llvm::cl::opt<bool> Default_AA("std", llvm::cl::init(false), llvm::cl::desc("Standard alias analysis"));
static llvm::cl::opt<bool> Pocr_AA("pocr", llvm::cl::init(false), llvm::cl::desc("POCR alias analysis"));
static llvm::cl::opt<bool> Gspan_AA("gspan", llvm::cl::init(false), llvm::cl::desc("POCR alias analysis"));
static llvm::cl::opt<bool> Gr_AA("gr", llvm::cl::init(false), llvm::cl::desc("POCR alias analysis"));
static llvm::cl::opt<bool> GrGspan_AA("grgspan", llvm::cl::init(false), llvm::cl::desc("POCR alias analysis"));


int main(int argc, char** argv)
{
    int arg_num = 0;
    char** arg_vec = new char* [argc];
    std::vector<std::string> moduleNameVec;
    std::vector<std::string> inFileVec;
    CFLBase::processArgs(argc, argv, arg_num, arg_vec, inFileVec);
    llvm::cl::ParseCommandLineOptions(arg_num, arg_vec, "Alias analysis\n");

    StdAA* aa;
    if (Default_AA) {
        aa = new StdAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Pocr_AA) {
        aa = new PocrAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Gspan_AA) {
        aa = new GspanAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Gr_AA) {
        aa = new GRAA(inFileVec[0]);
        aa->analyze();
    }
    else if (GrGspan_AA) {
        aa = new GRGspanAA(inFileVec[0]);
        aa->analyze();
    }
    else
        std::cout << "No alias analysis solver specified!" << std::endl;

    return 0;
}
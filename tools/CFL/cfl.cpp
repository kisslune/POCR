/*
 // Alias analysis
 //
 // Author: Kisslune
 */

#include "SVF-FE/LLVMUtil.h"
#include "CFLSolver/CFLSolver.h"

using namespace SVF;
using namespace std;

static llvm::cl::opt<bool> Default_CFL("std", llvm::cl::init(false), llvm::cl::desc("Standard alias analysis"));
static llvm::cl::opt<bool> Pocr_CFL("pocr", llvm::cl::init(false), llvm::cl::desc("POCR alias analysis"));


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

    StdCFL* cfl;

//    cfl = new StdCFL(inFileVec[0], inFileVec[1]);
//    cfl->initialize();
//
//    auto grammar = cfl->grammar();
//
//    for (auto lblIter: grammar->labelToIntMap) {
//        cout << lblIter.first << '\t' << (int) lblIter.second << endl;
//    }
//
//    for (auto id: grammar->variantLabels)
//        cout << (int) id << endl;
//
//    for (auto gIter: grammar->emptyRules)
//        cout << (int) gIter << endl;
//
//    for (auto gIter: grammar->unaryRules)
//        cout << (int) gIter.second << '\t' << (int) gIter.first << endl;
//
//    for (auto gIter: grammar->binaryRules)
//        cout << (int) gIter.second << '\t' << (int) gIter.first.first << '\t' << (int) gIter.first.second << endl;
//
//    auto graph = cfl->graph();
//    graph->writeGraph("outgraph");

    if (Default_CFL) {
        cfl = new StdCFL(inFileVec[0],inFileVec[1]);
        cfl->analyze();
    }
    else
        cout << "No CFL-reachability solver specified!" << endl;

    return 0;
}
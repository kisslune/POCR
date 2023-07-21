/*
 // Alias analysis
 //
 // Author: Kisslune
 */

#include "SVF-LLVM/LLVMUtil.h"
#include "AA/AliasAnalysis.h"

using namespace SVF;

static Option<bool> Default_AA("std", "Standard alias analysis", false);
static Option<bool> Pocr_AA("pocr", "POCR alias analysis", false);
static Option<bool> Gspan_AA("gspan", "Graspan alias analysis", false);
static Option<bool> Gr_AA("gr", "Grammar rewritting alias analysis", false);
static Option<bool> GrGspan_AA("grgspan", "Grammar rewritting Graspan alias analysis", false);
static Option<bool> Focr_AA("focr", "FOCR alias analysis", false);


int main(int argc, char** argv)
{
    int arg_num = 0;
    char** arg_vec = new char* [argc];
    std::vector<std::string> moduleNameVec;
    std::vector<std::string> inFileVec;
    processArgs(argc, argv, arg_num, arg_vec, inFileVec);
    OptionBase::parseOptions(arg_num, arg_vec, "Alias analysis\n", "[options] <input>");

    AliasAnalysis* aa;
    if (Default_AA())
    {
        aa = new StdAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Pocr_AA())
    {
        aa = new PocrAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Gspan_AA())
    {
        aa = new GspanAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Gr_AA())
    {
        aa = new GRAA(inFileVec[0]);
        aa->analyze();
    }
    else if (GrGspan_AA())
    {
        aa = new GRGspanAA(inFileVec[0]);
        aa->analyze();
    }
    else if (Focr_AA())
    {
        aa = new FocrAA(inFileVec[0]);
        aa->analyze();
    }
    else    // focr is the default solver
    {
        aa = new FocrAA(inFileVec[0]);
        aa->analyze();
    }

    return 0;
}
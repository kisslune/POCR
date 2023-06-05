//===- Options.cpp -- Command line options ------------------------//

#include <llvm/Support/CommandLine.h>
#include "CFLSolver/CFLOpt.h"

namespace SVF
{
const u32_t CFLOpt::timeOut = 24 * 3600;     // in second

const Option<bool> CFLOpt::PStat(
        "pstat",
        "Print statistics",
        true
);

const Option<bool> CFLOpt::solveCFL(
        "solve",
        "Perform dynamic CFL-reachability solving",
        true
);

const Option<bool> CFLOpt::writeGraph(
        "write-graph",
        "Write the graph into file",
        false
);

const Option<bool> CFLOpt::graphStat(
        "graph-stat",
        "Conduct graph statistics",
        true
);

const Option<bool> CFLOpt::scc(
        "scc",
        "Enable cycle elimination",
        false
);

const Option<bool> CFLOpt::gf(
        "gf",
        "Enable graph folding",
        false
);

const Option<bool> CFLOpt::interDyck(
        "interdyck",
        "Enable non-Dyck-contributing edge elimination",
        false   // slower than expected
);


void processArgs(int argc, char** argv, int& arg_num, char** arg_vec, std::vector<std::string>& inFileVec)
{
    for (int i = 0; i < argc; ++i)
    {
        std::ifstream f(argv[i]);
        if (i == 0)
        {
            arg_vec[arg_num] = argv[i];
            arg_num++;
        }
        else if (f.good())
        {
            inFileVec.push_back(argv[i]);
        }
        else
        {
            arg_vec[arg_num] = argv[i];
            arg_num++;
        }
    }
}

//std::vector<std::string> split(std::string str, char s)
//{
//    std::vector<std::string> sVec;
//    std::string::iterator it = str.begin();
//    std::string subStr;
//    while (it != str.end())
//    {
//        if (*it == s && !subStr.empty())
//        {
//            sVec.push_back(subStr);
//            subStr.clear();
//        }
//        else
//        {
//            subStr.push_back(*it);
//        }
//        it++;
//    }
//    if (!subStr.empty())
//        sVec.push_back(subStr);
//    return sVec;
//}

} // namespace SVF.

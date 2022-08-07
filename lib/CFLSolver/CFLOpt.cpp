//===- Options.cpp -- Command line options ------------------------//

#include <llvm/Support/CommandLine.h>
#include "CFLSolver/CFLOpt.h"

namespace SVF
{
const u32_t CFLOpt::timeOut = 24 * 3600;     // in second

const llvm::cl::opt<bool> CFLOpt::PStat(
        "pstat",
        llvm::cl::init(true),
        llvm::cl::desc("Statistic for Pointer analysis")
);

const llvm::cl::opt<bool> CFLOpt::solveCFL(
        "solve",
        llvm::cl::init(true),
        llvm::cl::desc("solve CFL-reachability")
);

const llvm::cl::opt<bool> CFLOpt::writeGraph(
        "write-graph",
        llvm::cl::init(false),
        llvm::cl::desc("solve CFL-reachability")
);

const llvm::cl::opt<bool> CFLOpt::graphStat(
        "graph-stat",
        llvm::cl::init(true),
        llvm::cl::desc("Graph statistics")
);

} // namespace SVF.

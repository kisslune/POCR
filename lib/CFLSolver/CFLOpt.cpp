//===- Options.cpp -- Command line options ------------------------//

#include <llvm/Support/CommandLine.h>
#include "CFLSolver/CFLOpt.h"

namespace SVF
{
const u32_t CFLOpt::timeOut = 24 * 3600;     // in second

const Option<bool> CFLOpt::PStat(
        "pstat",
        "Statistic for Pointer analysis",
        true
);

const Option<bool> CFLOpt::solveCFL(
        "solve",
        "solve CFL-reachability",
        true
);

const Option<bool> CFLOpt::writeGraph(
        "write-graph",
        "solve CFL-reachability",
        false
);

const Option<bool> CFLOpt::graphStat(
        "graph-stat",
        "Graph statistics",
        true
);

} // namespace SVF.

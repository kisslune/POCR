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
        "Solve CFL-reachability",
        true
);

const Option<bool> CFLOpt::writeGraph(
        "write-graph",
        "Solve CFL-reachability",
        false
);

const Option<bool> CFLOpt::graphStat(
        "graph-stat",
        "Graph statistics",
        true
);

const Option<bool> CFLOpt::scc(
        "scc",
        "Enable cycle elimination",
        true
);

const Option<bool> CFLOpt::gc(
        "gf",
        "Enable graph folding",
        true
);

const Option<bool> CFLOpt::interDyck(
        "interdyck",
        "Eliminating non-Dyck-contributing edges",
        true
);

} // namespace SVF.

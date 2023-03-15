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

} // namespace SVF.

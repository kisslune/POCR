//===- Options.h -- Command line options ------------------------//

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <sstream>
#include "FastCluster/fastcluster.h"
#include "MemoryModel/PTAStat.h"
#include "MemoryModel/PointerAnalysisImpl.h"
#include "Util/NodeIDAllocator.h"
#include "MSSA/MemSSA.h"
#include "WPA/WPAPass.h"

namespace SVF
{

/// Carries around command line options.
class CFLOpt
{
public:
    CFLOpt(void) = delete;

    /// CFL Options
    static const u32_t timeOut;
    static const llvm::cl::opt<bool> PStat;
    static const llvm::cl::opt<bool> solveCFL;
    static const llvm::cl::opt<bool> writeGraph;
    static const llvm::cl::opt<bool> graphStat;
};
}  // namespace SVF

#endif  // ifdef OPTIONS_H_

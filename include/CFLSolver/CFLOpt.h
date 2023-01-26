//===- Options.h -- Command line options ------------------------//

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <sstream>
#include "FastCluster/fastcluster.h"
#include "Util/PTAStat.h"
#include "MemoryModel/PointerAnalysisImpl.h"
#include "Util/NodeIDAllocator.h"
#include "MSSA/MemSSA.h"
#include "WPA/WPAPass.h"
#include "Util/CommandLine.h"

namespace SVF
{

/// Carries around command line options.
class CFLOpt
{
public:
    CFLOpt(void) = delete;

    /// CFL Options
    static const u32_t timeOut;
    static const Option<bool> PStat;
    static const Option<bool> solveCFL;
    static const Option<bool> writeGraph;
    static const Option<bool> graphStat;
};
}  // namespace SVF

#endif  // ifdef OPTIONS_H_

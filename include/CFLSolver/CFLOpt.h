//===- Options.h -- Command line options ------------------------//

#ifndef CFLOpt_H_
#define CFLOpt_H_

#include <Util/Options.h>

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

#endif

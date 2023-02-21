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
    static const Option<bool> PStat;
    static const Option<bool> solveCFL;
    static const Option<bool> writeGraph;
    static const Option<bool> graphStat;
};
}  // namespace SVF

#endif

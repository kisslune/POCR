//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFLSTAT_H
#define POCR_SVF_CFLSTAT_H

#include "SVF-LLVM/BasicTypes.h"
#include "SVFIR/SVFType.h"

namespace SVF
{
class StdCFL;

class CFLGraph;

/*!
 * Statistics of Andersen's analysis
 */
class CFLStat
{
public:
    typedef llvm::DenseMap<const char*, u64_t> NUMStatMap;
    typedef llvm::DenseMap<const char*, double> TIMEStatMap;

    /// num counters
    u32_t numOfIteration;
    u32_t checks;
    u32_t numOfSumEdges;
    u32_t numOfCountEdges;
    u32_t numOfNodes;
    u32_t numOfEdges;

    /// time counters
    double timeOfSolving;
    double startTime;
    double endTime;
    double gsTime;

    /// A set for S edges
    std::map<NodeID, NodeBS> sEdgeSet;

private:
    StdCFL* cfl;

    NUMStatMap generalNumMap;
    NUMStatMap PTNumStatMap;
    TIMEStatMap timeStatMap;

    /// Memory usage, in KB
    u32_t _vmrssUsageBefore;
    u32_t _vmrssUsageAfter;
    u32_t _vmsizeUsageBefore;
    u32_t _vmsizeUsageAfter;

public:
    CFLStat(StdCFL* p) : cfl(p),
                         numOfIteration(0),
                         checks(0),
                         numOfSumEdges(0),
                         numOfCountEdges(0),
                         numOfNodes(0),
                         numOfEdges(0),
                         timeOfSolving(0)
    {
        startClk();
    };

    virtual ~CFLStat()
    {}

    virtual inline void startClk()
    { startTime = CLOCK_IN_MS(); }

    virtual inline void endClk()
    { endTime = CLOCK_IN_MS(); }

    static inline double getClk()
    { return CLOCK_IN_MS(); }

    void setMemUsageBefore();
    void setMemUsageAfter();

    void performStat();
    void graphStat();
    virtual void printStat(std::string str = "");
    void writeSPairsIntoFile(std::string fName);
};
}

#endif //POCR_SVF_CFLSTAT_H

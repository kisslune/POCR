//===- IVFStat.h -- Program Expression Graph-----------------------------//

/*
 * IVFStat.h
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#ifndef IVFSTAT_H_
#define IVFSTAT_H_


#include "SVF-LLVM/BasicTypes.h"
#include "SVFIR/SVFType.h"
#include <iostream>
#include <map>
#include <string>


namespace SVF
{
class VFAnalysis;

class IVFG;

/*!
 * Statistics of Andersen's analysis
 */
class VFAStat
{
public:
    typedef llvm::DenseMap<const char*, u64_t> NUMStatMap;
    typedef llvm::DenseMap<const char*, double> TIMEStatMap;

private:
    VFAnalysis* ivf;

public:
    VFAStat(VFAnalysis* p) : ivf(p)
    {
        startClk();
    };

    virtual ~VFAStat()
    {
    }

    virtual inline void startClk()
    {
        startTime = CLOCK_IN_MS();
    }

    virtual inline void endClk()
    {
        endTime = CLOCK_IN_MS();
    }

    static inline double getClk()
    {
        return CLOCK_IN_MS();
    }

    NUMStatMap generalNumMap;
    NUMStatMap PTNumStatMap;
    TIMEStatMap timeStatMap;

    double startTime;
    double endTime;
    double sccTime;
    double gfTime;
    double interDyckTime;
    double gsTime;

    /// Memory usage, in KB
    u32_t _vmrssUsageBefore;
    u32_t _vmrssUsageAfter;
    u32_t _vmsizeUsageBefore;
    u32_t _vmsizeUsageAfter;

    void setMemUsageBefore();
    void setMemUsageAfter();

    virtual void performStat();
    void vfgStat();
    virtual void printStat(std::string str = "");
};

}

#endif

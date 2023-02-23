//===- AAStat.h -- Program Expression Graph-----------------------------//

/*
 * AAStat.h
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#ifndef AASTAT_H
#define AASTAT_H

#include "SVF-LLVM/BasicTypes.h"
#include "SVFIR/SVFType.h"
#include <iostream>
#include <map>
#include <string>

namespace SVF
{

class AliasAnalysis;
class PEG;

/*!
 * Statistics of Andersen's analysis
 */
class AAStat
{
public:
    typedef llvm::DenseMap<const char*, u64_t> NUMStatMap;
    typedef llvm::DenseMap<const char*, double> TIMEStatMap;

private:
    AliasAnalysis* aa;

public:
    AAStat(AliasAnalysis* p)
            : aa(p), startTime(0), endTime(0)
    {
        startClk();
    };

    virtual ~AAStat()
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

    virtual void performStat();
    void pegStat();
    virtual void printStat(std::string str = "");
};
}

#endif //PROJECT_AASTAT_H

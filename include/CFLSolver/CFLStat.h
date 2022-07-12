//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFLSTAT_H
#define POCR_SVF_CFLSTAT_H

#include "Util/BasicTypes.h"
//#include <iostream>
//#include <map>
//#include <string>

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

private:
    StdCFL* cfl;

public:
    CFLStat(StdCFL* p)
            : cfl(p), startTime(0), endTime(0)
    {
        startClk();
    };

    virtual ~CFLStat()
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

    virtual void performStat();
    void pegStat();
    virtual void printStat(std::string str = "");
};
}


#endif //POCR_SVF_CFLSTAT_H

//===- IVFStat.cpp -- Program Expression Graph-----------------------------//

/*
 * IVFStat.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#include <iomanip>
#include "VFA/VFAStat.h"
#include "VFA/VFAnalysis.h"

using namespace SVF;

void VFAStat::printStat(std::string statname)
{
    std::cout.flags(std::ios::left);
    unsigned field_width = 20;
    for (TIMEStatMap::iterator it = timeStatMap.begin(), eit = timeStatMap.end(); it != eit; ++it) {
        // format out put with width 20 space
        std::cout << it->first << "\t" << it->second << "\n";
    }
    for (NUMStatMap::iterator it = PTNumStatMap.begin(), eit = PTNumStatMap.end(); it != eit; ++it) {
        // format out put with width 20 space
        std::cout << it->first << "\t" << it->second << "\n";
    }

    std::cout.flush();
    generalNumMap.clear();
    PTNumStatMap.clear();
    timeStatMap.clear();
}


void VFAStat::vfgStat()
{
    IVFG* lg = ivf->graph();

    u32_t totalNodeNumber = 0;
    u32_t totalEdgeNumber = 0;
    u32_t pEdgeNumber = 0;

    for (auto nodeIt = lg->begin(); nodeIt != lg->end(); nodeIt++) {
        totalNodeNumber++;
    }

    for (auto it: lg->getIVFGEdges()) {
        if (it->getEdgeKind() == IVFG::CallVF || it->getEdgeKind() == IVFG::RetVF)
            pEdgeNumber++;
        totalEdgeNumber++;
    }

    PTNumStatMap["#Nodes"] = totalNodeNumber;
    PTNumStatMap["#Edges"] = totalEdgeNumber;
    PTNumStatMap["#PEdges"] = pEdgeNumber;
    timeStatMap["GraphSimpTime"] = gsTime;

    VFAStat::printStat("VFG Stats");
}


void VFAStat::performStat()
{
    endClk();

    if (CFLOpt::graphStat())
        vfgStat();

    if (!CFLOpt::PStat())
        return;

    ivf->countSumEdges();

    timeStatMap["AnalysisTime"] = timeOfSolving;
    timeStatMap["VmrssInGB"] = (_vmrssUsageAfter - _vmrssUsageBefore) / 1024.0 / 1024.0;
    PTNumStatMap["#Checks"] = checks;
    PTNumStatMap["#SumEdges"] = numOfSumEdges;

    VFAStat::printStat("CFL-reachability analysis Stats");
}


void VFAStat::setMemUsageBefore()
{
    u32_t vmrss, vmsize;
    SVFUtil::getMemoryUsageKB(&vmrss, &vmsize);
    _vmrssUsageBefore = vmrss;
    _vmsizeUsageBefore = vmsize;
}


void VFAStat::setMemUsageAfter()
{
    u32_t vmrss, vmsize;
    SVFUtil::getMemoryUsageKB(&vmrss, &vmsize);
    _vmrssUsageAfter = vmrss;
    _vmsizeUsageAfter = vmsize;
}
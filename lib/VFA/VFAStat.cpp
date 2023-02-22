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
    for (NUMStatMap::iterator it = generalNumMap.begin(), eit = generalNumMap.end(); it != eit; ++it) {
        // format out put with width 20 space
        std::cout << std::setw(field_width) << it->first << it->second << "\n";
    }
    for (TIMEStatMap::iterator it = timeStatMap.begin(), eit = timeStatMap.end(); it != eit; ++it) {
        // format out put with width 20 space
        std::cout << std::setw(field_width) << it->first << it->second << "\n";
    }
    for (NUMStatMap::iterator it = PTNumStatMap.begin(), eit = PTNumStatMap.end(); it != eit; ++it) {
        // format out put with width 20 space
        std::cout << std::setw(field_width) << it->first << it->second << "\n";
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

    for (auto nodeIt = lg->begin(); nodeIt != lg->end(); nodeIt++) {
        totalNodeNumber++;
    }

    for (auto it: lg->getIVFGEdges()) {
        totalEdgeNumber++;
    }

    PTNumStatMap["#Nodes"] = totalNodeNumber;
    PTNumStatMap["#Edges"] = totalEdgeNumber;

    VFAStat::printStat("LG Stats");
}


void VFAStat::performStat()
{
    endClk();

    if (CFLOpt::graphStat())
        vfgStat();

    if (!CFLOpt::PStat())
        return;

    ivf->countSumEdges();

    timeStatMap["AnalysisTime"] = ivf->timeOfSolving;
    PTNumStatMap["#Checks"] = ivf->checks;
    PTNumStatMap["#SumEdges"] = ivf->numOfSumEdges;

    VFAStat::printStat("CFL-reachability analysis Stats");
}

//===- AAStat.cpp -- Program Expression Graph-----------------------------//

/*
 * AAStat.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#include <iomanip>
#include "AA/AAStat.h"
#include "AA/AliasAnalysis.h"

using namespace SVF;


void AAStat::printStat(std::string statname)
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


void AAStat::pegStat()
{
    PEG* peg = aa->graph();

    u32_t totalNodeNumber = 0;
    u32_t totalEdgeNumber = 0;

    for (auto nodeIt = peg->begin(); nodeIt != peg->end(); nodeIt++) {
        totalNodeNumber++;
    }

    for (auto it: peg->getPEGEdges()) {
        totalEdgeNumber++;
    }

    PTNumStatMap["NumOfNodes"] = totalNodeNumber;
    PTNumStatMap["NumOfEdges"] = totalEdgeNumber * 2;

    AAStat::printStat("PEG Stats");
}


void AAStat::performStat()
{
    endClk();

    if (CFLOpt::graphStat)
        pegStat();

    if (!CFLOpt::PStat)
        return;

    aa->countSumEdges();
    timeStatMap["AnalysisTime"] = aa->timeOfSolving;
    PTNumStatMap["#Checks"] = aa->checks;
    PTNumStatMap["#SumEdges"] = aa->numOfSumEdges;

    AAStat::printStat("CFL-reachability analysis Stats");
}

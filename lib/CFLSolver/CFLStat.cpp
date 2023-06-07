//
// Created by kisslune on 7/5/22.
//



#include <iomanip>
#include "CFLSolver/CFLStat.h"
#include "CFLSolver/CFLSolver.h"

using namespace SVF;

void CFLStat::printStat(std::string statname)
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


void CFLStat::graphStat()
{
    PTNumStatMap["#Nodes"] = numOfNodes;
    PTNumStatMap["#Edges"] = numOfEdges;

    CFLStat::printStat("CFLGraph Stats");
}


void CFLStat::performStat()
{
    endClk();

    calcGraphInfo();
    graphStat();
    cfl->countSumEdges();

    timeStatMap["AnalysisTime"] = cfl->timeOfSolving;
    PTNumStatMap["#Checks"] = cfl->checks;
    PTNumStatMap["#SumEdges"] = cfl->numOfSumEdges;
    // - numOfEdges;

    CFLStat::printStat("CFL-reachability analysis Stats");
}


void CFLStat::calcGraphInfo()
{
    numOfNodes = 0;
    numOfEdges = 0;
    CFLGraph* g = cfl->graph();

    for (auto nodeIt = g->begin(); nodeIt != g->end(); nodeIt++) {
        numOfNodes++;
    }

    for (auto it: g->getCFLEdges()) {
        numOfEdges++;
    }
}
//===- IVFAnalysis.cpp -- Program Expression Graph-----------------------------//

/*
 * IVFAnalysis.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#include "VFA/VFAnalysis.h"

using namespace SVF;
using namespace SVFUtil;


void VFAnalysis::initialize()
{
    setGraph(new IVFG());
    graph()->readGraph(graphName);

    stat = new VFAStat(this);
    /// Graph simplification
    simplifyGraph();
}


void VFAnalysis::analyze()
{
    initialize();

    std::thread th(VFAnalysis::timer);     // timer thread

    // Start solving
    double propHorStart = stat->getClk();

    do {
        numOfIteration++;
        reanalyze = false;
        if (CFLOpt::solveCFL())
            solve();
    } while (reanalyze);

    double propHorEnd = stat->getClk();
    timeOfSolving += (propHorEnd - propHorStart) / TIMEINTERVAL;

    // Finalize the analysis
    finalize();

    pthread_cancel(th.native_handle());     // kill timer
    th.join();
}


void VFAnalysis::finalize()
{
    dumpStat();
    if (CFLOpt::writeGraph())
        graph()->writeGraph("vfg");
}



Label StdVFA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

    u32_t lInd = lty.second;
    u32_t rInd = rty.second;

    if (lWord == A && rWord == A)
        return std::make_pair(A, 0);

    if (lWord == call && rWord == A)
        return std::make_pair(Cl, lInd);

    if (lWord == Cl && rWord == ret && lInd == rInd)
        return std::make_pair(A, 0);

    return std::make_pair(fault, 0);
}


Label StdVFA::unarySumm(Label lty)
{
    char lWord = lty.first;
    if (lWord == a)
        return std::make_pair(A, 0);
    return std::make_pair(fault, 0);
}


void StdVFA::initialize()
{
    VFAnalysis::initialize();
    initSolver();
}


void StdVFA::initSolver()
{
    for (CFLEdge* edge: graph()->getIVFGEdges()) {
        NodeID srcId = edge->getSrcID();
        NodeID dstId = edge->getDstID();

        if (edge->getEdgeKind() == IVFG::DirectVF) {
            addEdge(srcId, dstId, std::make_pair(a, 0));
            pushIntoWorklist(srcId, dstId, std::make_pair(a, 0));
        }
        if (edge->getEdgeKind() == IVFG::CallVF) {
            addEdge(srcId, dstId, std::make_pair(call, edge->getEdgeIdx()));
            pushIntoWorklist(srcId, dstId, std::make_pair(call, edge->getEdgeIdx()));
        }
        if (edge->getEdgeKind() == IVFG::RetVF) {
            addEdge(srcId, dstId, std::make_pair(ret, edge->getEdgeIdx()));
            pushIntoWorklist(srcId, dstId, std::make_pair(ret, edge->getEdgeIdx()));
        }
    }

    /// A ::= epsilon
    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter) {
        NodeID nodeId = nIter->first;
        addEdge(nodeId, nodeId, std::make_pair(A, 0));
        pushIntoWorklist(nodeId, nodeId, std::make_pair(A, 0));
    }
}


bool StdVFA::pushIntoWorklist(NodeID src, NodeID dst, Label ty)
{
    if (ty.first == fault)
        return false;

    return VFAnalysis::pushIntoWorklist(src, dst, ty);
}


void StdVFA::processCFLItem(CFLItem item)
{
    Label newTy = unarySumm(item.type());
    if (addEdge(item.src(), item.dst(), newTy)) {
        checks++;
        pushIntoWorklist(item.src(), item.dst(), newTy);
    }

    for (auto iter: cflData()->getSuccMap(item.dst())) {
        Label rty = iter.first;
        newTy = binarySumm(item.type(), rty);
        NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
        checks += iter.second.count();
        for (NodeID diffDst: diffDsts)
            pushIntoWorklist(item.src(), diffDst, newTy);
    }

    for (auto iter: cflData()->getPredMap(item.src())) {
        Label lty = iter.first;
        newTy = binarySumm(lty, item.type());
        NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
        checks += iter.second.count();
        for (NodeID diffSrc: diffSrcs)
            pushIntoWorklist(diffSrc, item.dst(), newTy);
    }
}


void StdVFA::countSumEdges()
{
    numOfSumEdges = 0;
    std::set<u32_t> s = {A,Cl};

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1) {
        for (auto& iter2: iter1->second) {
            if (s.find(iter2.first.first) != s.end())
                numOfSumEdges += iter2.second.count();
        }
    }
}

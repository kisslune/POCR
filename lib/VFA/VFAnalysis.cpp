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


/// ------------------- VFA Base Methods -----------------------

void VFAnalysis::initialize()
{
    setGraph(new IVFG());
    graph()->readGraph(graphName);

    stat = new VFAStat(this);
    stat->setMemUsageBefore();

    /// Graph simplification
    simplifyGraph();
    /// initialize online solver
    initSolver();
}


void VFAnalysis::analyze()
{
    initialize();

    std::thread th(VFAnalysis::timer);     // timer thread

    // Start solving
    double propHorStart = stat->getClk();

    do
    {
        stat->numOfIteration++;
        reanalyze = false;
        if (CFLOpt::solveCFL())
            solve();
    } while (reanalyze);

    double propHorEnd = stat->getClk();
    stat->timeOfSolving += (propHorEnd - propHorStart) / TIMEINTERVAL;

    // Finalize the analysis
    finalize();

    pthread_cancel(th.native_handle());     // kill timer
    th.join();
}


void VFAnalysis::finalize()
{
    stat->setMemUsageAfter();

    dumpStat();
    if (CFLOpt::writeGraph())
        graph()->writeGraph("vfg");
}


bool VFAnalysis::pushIntoWorklist(NodeID src, NodeID dst, Label ty)
{
    if (ty.first == fault)
        return false;

    return CFLBase::pushIntoWorklist(src, dst, ty);
}


/// ------------------- Std VFA Methods ----------------------

Set<Label> StdVFA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

    u32_t lInd = lty.second;
    u32_t rInd = rty.second;

    if (lWord == A && rWord == A)
        return {std::make_pair(A, 0)};

    if (lWord == call && rWord == A)
        return {std::make_pair(Cl, lInd)};

    if (lWord == Cl && rWord == ret && lInd == rInd)
        return {std::make_pair(A, 0)};

    return {std::make_pair(fault, 0)};
}


Set<Label> StdVFA::unarySumm(Label lty)
{
    char lWord = lty.first;
    if (lWord == a)
        return {std::make_pair(A, 0)};

    return {std::make_pair(fault, 0)};
}


void StdVFA::initSolver()
{
    for (CFLEdge* edge: graph()->getIVFGEdges())
    {
        NodeID srcId = edge->getSrcID();
        NodeID dstId = edge->getDstID();

        if (edge->getEdgeKind() == IVFG::DirectVF)
        {
            addEdge(srcId, dstId, std::make_pair(a, 0));
            pushIntoWorklist(srcId, dstId, std::make_pair(a, 0));
        }
        if (edge->getEdgeKind() == IVFG::CallVF)
        {
            addEdge(srcId, dstId, std::make_pair(call, edge->getEdgeIdx()));
            pushIntoWorklist(srcId, dstId, std::make_pair(call, edge->getEdgeIdx()));
        }
        if (edge->getEdgeKind() == IVFG::RetVF)
        {
            addEdge(srcId, dstId, std::make_pair(ret, edge->getEdgeIdx()));
            pushIntoWorklist(srcId, dstId, std::make_pair(ret, edge->getEdgeIdx()));
        }
    }

    /// A ::= epsilon
    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter)
    {
        NodeID nodeId = nIter->first;
        addEdge(nodeId, nodeId, std::make_pair(A, 0));
        pushIntoWorklist(nodeId, nodeId, std::make_pair(A, 0));
    }
}


void StdVFA::processCFLItem(CFLItem item)
{
    /// Derive edges via unary production rules
    for (Label newTy: unarySumm(item.label()))
        if (addEdge(item.src(), item.dst(), newTy))
        {
            stat->checks++;
            pushIntoWorklist(item.src(), item.dst(), newTy);
        }

    /// Derive edges via binary production rules
    //@{
    for (auto& iter: cflData()->getSuccMap(item.dst()))
    {
        Label rty = iter.first;
        for (Label newTy : binarySumm(item.label(), rty))
        {
            NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
            stat->checks += iter.second.count();
            for (NodeID diffDst: diffDsts)
                pushIntoWorklist(item.src(), diffDst, newTy);
        }
    }

    for (auto& iter: cflData()->getPredMap(item.src()))
    {
        Label lty = iter.first;
        for (Label newTy : binarySumm(lty, item.label()))
        {
            NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
            stat->checks += iter.second.count();
            for (NodeID diffSrc: diffSrcs)
                pushIntoWorklist(diffSrc, item.dst(), newTy);
        }
    }
    //@}
}


void StdVFA::countSumEdges()
{
    stat->numOfSumEdges = 0;
    std::set<u32_t> s = {A, Cl};

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1)
    {
        for (auto& iter2: iter1->second)
        {
            if (s.find(iter2.first.first) != s.end())
                stat->numOfSumEdges += iter2.second.count();
        }
    }
}

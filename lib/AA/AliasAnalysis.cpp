/*
 * AliasAnalysis.cpp
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#include <sstream>
#include "Util/CppUtil.h"
#include "AA/AliasAnalysis.h"

using namespace SVF;
using namespace SVFUtil;


void AliasAnalysis::initialize()
{
    setGraph(new PEG());
    graph()->readGraph(graphName);  // read a uni-directed graph

    stat = new AAStat(this);
    stat->setMemUsageBefore();

    /// Graph simplification
    simplifyGraph();
    /// initialize online solver
    initSolver();
}


void AliasAnalysis::finalize()
{
    stat->setMemUsageAfter();

    dumpStat();
    if (!CFLOpt::outGraphFName().empty())
        graph()->writeGraph(CFLOpt::outGraphFName());
}


bool AliasAnalysis::pushIntoWorklist(NodeID src, NodeID dst, Label ty)
{
    if (ty.first == fault)
        return false;

    return CFLBase::pushIntoWorklist(src, dst, ty);
}


void AliasAnalysis::analyze()
{
    initialize();

    std::thread th(AliasAnalysis::timer);     // timer thread

    // Start solving
    double propStart = stat->getClk();

    do
    {
        stat->numOfIteration++;
        reanalyze = false;
        if (CFLOpt::solveCFL())
            solve();
    } while (reanalyze);

    double propEnd = stat->getClk();
    stat->timeOfSolving += (propEnd - propStart) / TIMEINTERVAL;


    // Finalize the analysis
    finalize();

    pthread_cancel(th.native_handle());     // kill timer
    th.join();
}


bool AliasAnalysis::checkAndAddEdge(NodeID src, NodeID dst, Label lbl)
{
    if (!lbl.first)
        return false;

    stat->checks++;
    return cflData()->checkAndAddEdge(src, dst, lbl);
}


NodeBS AliasAnalysis::checkAndAddEdges(NodeID src, const NodeBS& dstSet, Label lbl)
{
    if (!lbl.first)
        return emptyBS;

    stat->checks += dstSet.count();
    return cflData()->checkAndAddEdges(src, dstSet, lbl);
}


NodeBS AliasAnalysis::checkAndAddEdges(const NodeBS& srcSet, NodeID dst, Label lbl)
{
    if (!lbl.first)
        return emptyBS;

    stat->checks += srcSet.count();
    return cflData()->checkAndAddEdges(srcSet, dst, lbl);
}


void AliasAnalysis::countSumEdges()
{
    stat->numOfSumEdges = 0;

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1)
    {
        for (auto& iter2 : iter1->second)
        {
            stat->numOfSumEdges += iter2.second.count();
            if (iter2.first.first == V)
                stat->numOfSEdges += iter2.second.count();
        }
    }
}


/// ------------------- Std AA Methods ----------------------

Set<Label> StdAA::binarySumm(Label lty, Label rty)
{
    u32_t lWord = lty.first;
    u32_t rWord = rty.first;

    if (lWord == A && rWord == A)
        return {std::make_pair(A, 0)};
    if (lWord == Abar && rWord == Abar)
        return {std::make_pair(Abar, 0)};
    if (lWord == a && rWord == M)
        return {std::make_pair(A, 0)};
    if (lWord == M && rWord == abar)
        return {std::make_pair(Abar, 0)};
    if (lWord == Abar && rWord == V)
        return {std::make_pair(V, 0)};
    if (lWord == V && rWord == A)
        return {std::make_pair(V, 0)};
    if (lWord == dbar && rWord == V)
        return {std::make_pair(DV, 0)};
    if (lWord == DV && rWord == d)
        return {std::make_pair(M, 0)};
    if (lWord == fbar && rWord == V)
        return {std::make_pair(FV, lty.second)};
    if (lWord == FV && rWord == f && lty.second == rty.second)
        return {std::make_pair(V, 0)};

    return {std::make_pair(fault, 0)};
}


Set<Label> StdAA::unarySumm(Label lty)
{
    u32_t lWord = lty.first;
    if (lWord == M)
        return {std::make_pair(V, 0)};
    if (lWord == a)
        return {std::make_pair(A, 0)};
    if (lWord == abar)
        return {std::make_pair(Abar, 0)};

    return {std::make_pair(fault, 0)};
}


void StdAA::initSolver()
{
    for (CFLEdge* edge : graph()->getPEGEdges())
    {
        if (edge->getEdgeKind() == PEG::Asgn)
        {
            checkAndAddEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
            checkAndAddEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(abar, 0));
            pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
            pushIntoWorklist(edge->getDstID(), edge->getSrcID(), std::make_pair(abar, 0));
        }
        else if (edge->getEdgeKind() == PEG::Gep)
        {
            u32_t offset = edge->getEdgeIdx();
            checkAndAddEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(f, offset));
            checkAndAddEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(fbar, offset));
            pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(f, offset));
            pushIntoWorklist(edge->getDstID(), edge->getSrcID(), std::make_pair(fbar, offset));
        }
        else if (edge->getEdgeKind() == PEG::Deref)
        {
            checkAndAddEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(d, 0));
            checkAndAddEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(dbar, 0));
            pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(d, 0));
            pushIntoWorklist(edge->getDstID(), edge->getSrcID(), std::make_pair(dbar, 0));
        }
    }

    /// V ::= epsilon
    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter)
    {
        NodeID nodeId = nIter->first;
        checkAndAddEdge(nodeId, nodeId, std::make_pair(V, 0));
        pushIntoWorklist(nodeId, nodeId, std::make_pair(V, 0));
        checkAndAddEdge(nodeId, nodeId, std::make_pair(A, 0));
        checkAndAddEdge(nodeId, nodeId, std::make_pair(Abar, 0));
    }
}

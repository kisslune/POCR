/*
 * AliasAnalysis.cpp
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */


#include <sstream>
#include "Util/CppUtil.h"
#include "Util/SVFUtil.h"
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
}


void AliasAnalysis::destroy()
{
    delete stat;
    stat = NULL;
}


void AliasAnalysis::dumpStat()
{
    if (stat)
        stat->performStat();
}


void AliasAnalysis::finalize()
{
    stat->setMemUsageAfter();

    dumpStat();
    if (CFLOpt::writeGraph())
        graph()->writeGraph("peg");
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


Set<Label> StdAA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

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
    char lWord = lty.first;
    if (lWord == M)
        return {std::make_pair(V, 0)};
    if (lWord == a)
        return {std::make_pair(A, 0)};
    if (lWord == abar)
        return {std::make_pair(Abar, 0)};

    return {std::make_pair(fault, 0)};
}


void StdAA::initialize()
{
    AliasAnalysis::initialize();
    initSolver();
}


void StdAA::finalize()
{
    AliasAnalysis::finalize();
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

bool StdAA::pushIntoWorklist(NodeID src, NodeID dst, Label ty)
{
    if (ty.first == fault)
        return false;

    return CFLBase::pushIntoWorklist(src, dst, ty);
}


void StdAA::processCFLItem(CFLItem item)
{
    /// Derive edges via unary production rules
    for (Label newTy : unarySumm(item.label()))
        if (checkAndAddEdge(item.src(), item.dst(), newTy))
        {
            stat->checks++;
            pushIntoWorklist(item.src(), item.dst(), newTy);
        }

    /// Derive edges via binary production rules
    //@{
    for (auto& iter : cflData()->getSuccMap(item.dst()))
    {
        Label rty = iter.first;
        for (Label newTy : binarySumm(item.label(), rty))
        {
            NodeBS diffDsts = checkAndAddEdges(item.src(), iter.second, newTy);
            stat->checks += iter.second.count();
            for (NodeID diffDst : diffDsts)
                pushIntoWorklist(item.src(), diffDst, newTy);
        }
    }

    for (auto& iter : cflData()->getPredMap(item.src()))
    {
        Label lty = iter.first;
        for (Label newTy : binarySumm(lty, item.label()))
        {
            NodeBS diffSrcs = checkAndAddEdges(iter.second, item.dst(), newTy);
            stat->checks += iter.second.count();
            for (NodeID diffSrc : diffSrcs)
                pushIntoWorklist(diffSrc, item.dst(), newTy);
        }
    }
    //@}
};


void StdAA::dumpAlias()
{
    for (auto& it : valAlias)
    {
        NodeID nId1 = it.first;
        outs() << "\nNode " << nId1 << " ";
        outs() << "  mayAlias with: { ";
        for (NodeID nId2 : it.second)
            outs() << nId2 << " ";
        outs() << "}\n\n";
    }
}


void StdAA::countSumEdges()
{
    stat->numOfSumEdges = 0;
    std::set<int> s = {M, V, DV, FV, A, Abar};

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1)
    {
        for (auto& iter2 : iter1->second)
        {
            if (s.find(iter2.first.first) != s.end())
                stat->numOfSumEdges += iter2.second.count();
        }
    }

    std::set<int> s1 = {A};
    for (auto it = cflData()->begin(); it != cflData()->end(); ++it)
    {
        cflData()->checkAndAddEdge(it->first, it->first, std::make_pair(A, 0));
    }
}

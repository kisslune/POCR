/* -------------------- CFLSolver.cpp ------------------ */
//
// Created by kisslune on 7/5/22.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;

void StdCFL::initialize()
{
    _grammar = new CFG();
    _grammar->parseGrammar(grammarName);

    _graph = new CFLGraph(_grammar);
    _graph->readGraph(graphName);

    stat = new CFLStat(this);
    stat->setMemUsageBefore();

    initSolver();
}


void StdCFL::finalize()
{
    stat->setMemUsageAfter();

    dumpStat();
    if (CFLOpt::writeGraph())
        graph()->writeGraph("cflg");
}


void StdCFL::analyze()
{
    initialize();

    std::thread th(StdCFL::timer);      // timer thread

    /// start solving
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

    finalize();

    pthread_cancel(th.native_handle());     // kill timer
    th.join();
}


Set<Label> StdCFL::unarySumm(Label lty)
{
    Set<Label> retVal;
    auto& lhsSet = grammar()->getLhs(lty.first);

    for (auto lhs : lhsSet)
    {
        if (!lhs)
            continue;

        if (grammar()->isaVariantSymbol(lhs) && grammar()->isaVariantSymbol(lty.first))
            retVal.insert(Label(lhs, lty.second));
        else
            retVal.insert(Label(lhs, 0));
    }

    return retVal;
}


Set<Label> StdCFL::binarySumm(Label lty, Label rty)
{
    Set<Label> retVal;
    auto lhsSet = grammar()->getLhs(std::make_pair(lty.first, rty.first));

    for (auto lhs : lhsSet)
    {
        if (!lhs)       // a fault label
            continue;

        if (grammar()->isaVariantSymbol(lty.first))
        {
            if ((grammar()->isaVariantSymbol(rty.first) && lty.second == rty.second)
                || !grammar()->isaVariantSymbol(rty.first))
            {
                if (grammar()->isaVariantSymbol(lhs))
                    retVal.insert(Label(lhs, lty.second));
                else
                    retVal.insert(Label(lhs, 0));
            }
        }
        else if (grammar()->isaVariantSymbol(rty.first))
        {
            if (grammar()->isaVariantSymbol(lhs))
                retVal.insert(Label(lhs, rty.second));
            else
                retVal.insert(Label(lhs, 0));
        }
        else
            retVal.insert(Label(lhs, 0));
    }

    return retVal;
}


void StdCFL::initSolver()
{
    /// add all edges into adjacency list and worklist
    for (auto edge : graph()->getCFLEdges())
    {
        addEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(edge->getEdgeKind(), edge->getEdgeIdx()));
        pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(edge->getEdgeKind(), edge->getEdgeIdx()));
    }

    /// processing empty rules, i.e., X ::= epsilon
    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter)
    {
        NodeID nodeId = nIter->first;
        for (auto lhs : grammar()->getEmptyRules())
        {
            addEdge(nodeId, nodeId, std::make_pair(lhs, 0));
            pushIntoWorklist(nodeId, nodeId, std::make_pair(lhs, 0));
        }
    }
}


void StdCFL::processCFLItem(CFLItem item)
{
    /// Derive edges via unary production rules
    for (Label newTy : unarySumm(item.label()))
        if (addEdge(item.src(), item.dst(), newTy))
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
            NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
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
            NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
            stat->checks += iter.second.count();
            for (NodeID diffSrc : diffSrcs)
                pushIntoWorklist(diffSrc, item.dst(), newTy);
        }
    }
    //@}
}


bool StdCFL::pushIntoWorklist(NodeID src, NodeID dst, Label ty)
{
    if (ty.first == 0)
        return false;

    return CFLBase::pushIntoWorklist(src, dst, ty);
}


void StdCFL::dumpStat()
{
    if (CFLOpt::PStat() && stat)
        stat->performStat();
}


void StdCFL::countSumEdges()
{
    stat->numOfSumEdges = 0;
    stat->numOfCountEdges = 0;

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1)
    {
        for (auto& iter2 : iter1->second)
        {
            stat->numOfSumEdges += iter2.second.count();
            if (grammar()->isCountSymbol(iter2.first.first))
                stat->numOfCountEdges += iter2.second.count();
        }
    }
}


/* -------------------- CFLSolver.cpp ------------------ */
//
// Created by kisslune on 7/5/22.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;

void StdCFL::initialize()
{
    _grammar = new CFGrammar();
    _grammar->parseGrammar(grammarName);

    _graph = new CFLGraph(_grammar);
    _graph->readGraph(graphName);

    stat = new CFLStat(this);

    initSolver();
}


void StdCFL::finalize()
{
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

    do {
        numOfIteration++;
        reanalyze = false;
        if (CFLOpt::solveCFL())
            solve();
    } while (reanalyze);

    double propEnd = stat->getClk();
    timeOfSolving += (propEnd - propStart) / TIMEINTERVAL;

    finalize();

    pthread_cancel(th.native_handle());     // kill timer
    th.join();
}


Label StdCFL::binarySumm(Label lty, Label rty)
{
    auto lhs = grammar()->getLhs(std::make_pair(lty.first, rty.first));

    if (lhs == 0)       // a fault label
        return std::make_pair(0, 0);    // return a fault label

    if (grammar()->isaVariantLabel(lty.first) &&
        grammar()->isaVariantLabel(rty.first) &&
        lty.second == rty.second) {
        if (grammar()->isaVariantLabel(lhs))
            return std::make_pair(lhs, lty.second);
        else
            return std::make_pair(lhs, 0);
    }

    if (grammar()->isaVariantLabel(lty.first) &&
        !grammar()->isaVariantLabel(rty.first)) {
        if (grammar()->isaVariantLabel(lhs))
            return std::make_pair(lhs, lty.second);
        else
            return std::make_pair(lhs, 0);
    }

    if (!grammar()->isaVariantLabel(lty.first) &&
        grammar()->isaVariantLabel(rty.first)) {
        if (grammar()->isaVariantLabel(lhs))
            return std::make_pair(lhs, rty.second);
        else
            return std::make_pair(lhs, 0);
    }

    if (!grammar()->isaVariantLabel(lty.first) &&
        !grammar()->isaVariantLabel(rty.first)) {
        return std::make_pair(lhs, 0);
    }

    return std::make_pair(0, 0);
}


Label StdCFL::unarySumm(Label lty)
{
    auto lhs = grammar()->getLhs(lty.first);

    if (lhs == 0)
        return std::make_pair(0, 0);

    if (grammar()->isaVariantLabel(lhs) &&
        grammar()->isaVariantLabel(lty.first))
        return std::make_pair(lhs, lty.second);

    return std::make_pair(lhs, 0);
}


void StdCFL::initSolver()
{
    /// add all edges into adjacency list and worklist
    for (auto edge: graph()->getCFLEdges()) {
        addEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(edge->getEdgeKind(), edge->getEdgeIdx()));
        pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(edge->getEdgeKind(), edge->getEdgeIdx()));
    }

    /// processing empty rules, i.e., X ::= epsilon
    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter) {
        NodeID nodeId = nIter->first;
        for (auto lhs: grammar()->getEmptyRules()) {
            addEdge(nodeId, nodeId, std::make_pair(lhs, 0));
            pushIntoWorklist(nodeId, nodeId, std::make_pair(lhs, 0));
        }
    }
}


void StdCFL::processCFLItem(CFLItem item)
{
    Label newTy = unarySumm(item.type());
    if (addEdge(item.src(), item.dst(), newTy)) {
        checks++;
        pushIntoWorklist(item.src(), item.dst(), newTy);
    }

    for (auto& iter: cflData()->getSuccMap(item.dst())) {
        Label rty = iter.first;
        newTy = binarySumm(item.type(), rty);
        NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
        checks += iter.second.count();
        for (NodeID diffDst: diffDsts)
            pushIntoWorklist(item.src(), diffDst, newTy);
    }

    for (auto& iter: cflData()->getPredMap(item.src())) {
        Label lty = iter.first;
        newTy = binarySumm(lty, item.type());
        NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
        checks += iter.second.count();
        for (NodeID diffSrc: diffSrcs)
            pushIntoWorklist(diffSrc, item.dst(), newTy);
    }
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
    numOfSumEdges = 0;

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1) {
        for (auto& iter2: iter1->second) {
            numOfSumEdges += iter2.second.count();
        }
    }
}


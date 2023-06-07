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

    do
    {
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


Set<Label> StdCFL::cflBinarySumm(Label lty, Label rty)
{
    Set<Label> retVal;
    auto lhsSet = grammar()->getLhs(std::make_pair(lty.first, rty.first));

    for (auto lhs: lhsSet)
    {
        if (!lhs)       // a fault label
            continue;

        if (grammar()->isaVariantLabel(lty.first))
        {
            if ((grammar()->isaVariantLabel(rty.first) && lty.second == rty.second)
                || !grammar()->isaVariantLabel(rty.first))
            {
                if (grammar()->isaVariantLabel(lhs))
                    retVal.insert(Label(lhs, lty.second));
                else
                    retVal.insert(Label(lhs, 0));
            }
        }
        else if (grammar()->isaVariantLabel(rty.first))
        {
            if (grammar()->isaVariantLabel(lhs))
                retVal.insert(Label(lhs, rty.second));
            else
                retVal.insert(Label(lhs, 0));
        }
        else
            retVal.insert(Label(lhs, 0));

//        if (grammar()->isaVariantLabel(lty.first) &&
//            grammar()->isaVariantLabel(rty.first) &&
//            lty.second == rty.second)
//        {
//            if (grammar()->isaVariantLabel(lhs))
//                retVal.insert(Label(lhs, lty.second));
//            else
//                retVal.insert(Label(lhs, 0));
//        }
//
//        if (grammar()->isaVariantLabel(lty.first) &&
//            !grammar()->isaVariantLabel(rty.first))
//        {
//            if (grammar()->isaVariantLabel(lhs))
//                retVal.insert(Label(lhs, lty.second));
//            else
//                retVal.insert(Label(lhs, 0));
//        }
//
//        if (!grammar()->isaVariantLabel(lty.first) &&
//            grammar()->isaVariantLabel(rty.first))
//        {
//            if (grammar()->isaVariantLabel(lhs))
//                retVal.insert(Label(lhs, rty.second));
//            else
//                retVal.insert(Label(lhs, 0));
//        }
//
//        if (!grammar()->isaVariantLabel(lty.first) &&
//            !grammar()->isaVariantLabel(rty.first))
//        {
//            retVal.insert(Label(lhs, 0));
//        }
    }

    return retVal;
}


Set<Label> StdCFL::cflUnarySumm(Label lty)
{
    Set<Label> retVal;
    auto& lhsSet = grammar()->getLhs(lty.first);

    for (auto lhs: lhsSet)
    {
        if (!lhs)
            continue;

        if (grammar()->isaVariantLabel(lhs) && grammar()->isaVariantLabel(lty.first))
            retVal.insert(Label(lhs, lty.second));
        else
            retVal.insert(Label(lhs, 0));
    }

    return retVal;
}


void StdCFL::initSolver()
{
    /// add all edges into adjacency list and worklist
    for (auto edge: graph()->getCFLEdges())
    {
        addEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(edge->getEdgeKind(), edge->getEdgeIdx()));
        pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(edge->getEdgeKind(), edge->getEdgeIdx()));
    }

    /// processing empty rules, i.e., X ::= epsilon
    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter)
    {
        NodeID nodeId = nIter->first;
        for (auto lhs: grammar()->getEmptyRules())
        {
            addEdge(nodeId, nodeId, std::make_pair(lhs, 0));
            pushIntoWorklist(nodeId, nodeId, std::make_pair(lhs, 0));
        }
    }
}


void StdCFL::processCFLItem(CFLItem item)
{
    auto newTySet = cflUnarySumm(item.type());
    for (Label newTy: newTySet){
        if (addEdge(item.src(), item.dst(), newTy))
        {
            checks++;
            pushIntoWorklist(item.src(), item.dst(), newTy);
        }
    }

    for (auto& iter: cflData()->getSuccMap(item.dst()))
    {
        Label rty = iter.first;
        auto newTySet = cflBinarySumm(item.type(), rty);
        for (Label newTy: newTySet)
        {
            NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
            checks += iter.second.count();
            for (NodeID diffDst: diffDsts)
                pushIntoWorklist(item.src(), diffDst, newTy);
        }
    }

    for (auto& iter: cflData()->getPredMap(item.src()))
    {
        Label lty = iter.first;
        auto newTySet = cflBinarySumm(lty, item.type());
        for (Label newTy: newTySet)
        {
            NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
            checks += iter.second.count();
            for (NodeID diffSrc: diffSrcs)
                pushIntoWorklist(diffSrc, item.dst(), newTy);
        }
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

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1)
    {
        for (auto& iter2: iter1->second)
        {
            // std::cout << "num "<<(int)(iter2.first).first <<std::endl;
            // numOfSumEdges += iter2.second.count();
            if ((int)(iter2.first).first == 1){
                numOfSumEdges += iter2.second.count();
                // numOfSumEdges = 0;
                // std::cout << (int)(*iter1).first << ": ";
                // for (int dst : iter2.second){
                //     std::cout << dst << ", " ;
                // }
                // std::cout<<std::endl;
                // std::cout << (int) iter2.second.count() <<std::endl;
                // std::cout << (int)iter2.second <<std::endl;
            }
        }
    }
}


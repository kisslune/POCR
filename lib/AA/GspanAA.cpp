//
// Created by kisslune on 3/6/22.
//


#include "AA/AliasAnalysis.h"

using namespace SVF;


void GspanAA::initSolver()
{
    for (CFLEdge* edge: graph()->getPEGEdges())
    {
        if (edge->getEdgeKind() == PEG::Asgn)
        {
            checkAndAddEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
            checkAndAddEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(abar, 0));
        }
        else if (edge->getEdgeKind() == PEG::Gep)
        {
            u32_t offset = edge->getEdgeIdx();
            checkAndAddEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(f, offset));
            checkAndAddEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(fbar, offset));
        }
        else if (edge->getEdgeKind() == PEG::Deref)
        {
            checkAndAddEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(d, 0));
            checkAndAddEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(dbar, 0));
        }
    }

    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter)
    {
        NodeID nodeId = nIter->first;
        checkAndAddEdge(nodeId, nodeId, std::make_pair(V, 0));
        checkAndAddEdge(nodeId, nodeId, std::make_pair(A, 0));
        checkAndAddEdge(nodeId, nodeId, std::make_pair(Abar, 0));
    }
}


void GspanAA::solve()
{
    stat->numOfIteration++;
    reanalyze = false;

    for (auto& srcIter: cflData()->getSuccMap())
    {
        NodeID src = srcIter.first;
        CFLData resData;

        // old + new
        for (auto& tyIter: oldData()->getSuccs(src))
        {
            Label lty = tyIter.first;
            for (NodeID oldDst1: tyIter.second)
            {
                // new
                for (auto& tyIter2: cflData()->getSuccs(oldDst1))
                {
                    for (Label newTy: binarySumm(lty, tyIter2.first))
                        if (newTy.first && (resData.getSuccs(src, newTy) |= tyIter2.second))
                            stat->checks += tyIter2.second.count();       // stat
                }
            }
        }

        // new + old and new
        for (auto& tyIter: cflData()->getSuccs(src))
        {
            Label lty = tyIter.first;
            for (Label newTy: unarySumm(lty))
                for (NodeID newDst1: tyIter.second)
                {
                    if (newTy.first && (resData.getSuccs(src, newTy).test_and_set(newDst1)))
                        stat->checks++;       // stat
                    // old
                    for (auto& tyIter2: oldData()->getSuccs(newDst1))
                    {
                        for (Label newTy: binarySumm(lty, tyIter2.first))
                            if (newTy.first && (resData.getSuccs(src, newTy) |= tyIter2.second))
                                stat->checks += tyIter2.second.count();       // stat
                    }
                    // new
                    for (auto& tyIter2: cflData()->getSuccs(newDst1))
                    {
                        for (Label newTy: binarySumm(lty, tyIter2.first))
                            if (newTy.first && (resData.getSuccs(src, newTy) |= tyIter2.second))
                                stat->checks += tyIter2.second.count();       // stat
                    }
                }
        }

        // update old and new
        for (auto& tyIter: cflData()->getSuccs(src))
        {
            oldData()->getSuccs(src, tyIter.first) |= tyIter.second;
            tyIter.second.clear();
        }
        for (auto& tyIter: resData.getSuccs(src))
        {
            cflData()->getSuccs(src, tyIter.first) |= tyIter.second;
            cflData()->getSuccs(src, tyIter.first).intersectWithComplement(oldData()->getSuccs(src, tyIter.first));
            if (!cflData()->getSuccs(src, tyIter.first).empty())
                reanalyze = true;
        }
    }
}


/*!
 *
 */
void GspanAA::countSumEdges()
{
    stat->numOfSumEdges = 0;
    std::set<int> s = {M, V, DV, FV, A, Abar};

    for (auto iter1 = oldData()->begin(); iter1 != oldData()->end(); ++iter1)
    {
        for (auto& iter2: iter1->second)
        {
            if (s.find(iter2.first.first) != s.end())
                stat->numOfSumEdges += iter2.second.count();
        }
    }

    std::set<int> s1 = {A};
    for (auto it = oldData()->begin(); it != oldData()->end(); ++it)
    {
        oldData()->checkAndAddEdge(it->first, it->first, std::make_pair(A, 0));
    }
}
//
// Created by kisslune on 3/6/22.
//


#include "AA/AliasAnalysis.h"

using namespace SVF;


void GspanAA::initSolver()
{
    for (CFLEdge* edge: graph()->getPEGEdges()) {
        if (edge->getEdgeKind() == PEG::Asgn) {
            addEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
            addEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(abar, 0));
        }
        else if (edge->getEdgeKind() == PEG::Gep) {
            u32_t offset = edge->getEdgeIdx();
            addEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(f, offset));
            addEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(fbar, offset));
        }
        else if (edge->getEdgeKind() == PEG::Deref) {
            addEdge(edge->getSrcID(), edge->getDstID(), std::make_pair(d, 0));
            addEdge(edge->getDstID(), edge->getSrcID(), std::make_pair(dbar, 0));
        }
    }

    for (auto nIter = graph()->begin(); nIter != graph()->end(); ++nIter) {
        NodeID nodeId = nIter->first;
        addEdge(nodeId, nodeId, std::make_pair(V, 0));
        addEdge(nodeId, nodeId, std::make_pair(A, 0));
        addEdge(nodeId, nodeId, std::make_pair(Abar, 0));
    }
}


void GspanAA::solve()
{
    numOfIteration++;
    reanalyze = false;

    for (auto& srcIter: cflData()->getSuccMap()) {
        NodeID src = srcIter.first;
        CFLData resData;

        // old + new
        for (auto& tyIter: oldData()->getSuccMap(src)) {
            Label lty = tyIter.first;
            for (NodeID oldDst1: tyIter.second) {
                // new
                for (auto& tyIter2: cflData()->getSuccMap(oldDst1)) {
                    Label newTy = binarySumm(lty, tyIter2.first);
                    if (newTy.first) {
                        checks += tyIter2.second.count();       // stat
                        resData.getSuccs(src, newTy) |= tyIter2.second;
                    }
                }
            }
        }

        // new + old and new
        for (auto& tyIter: cflData()->getSuccMap(src)) {
            Label lty = tyIter.first;
            Label newTy = unarySumm(lty);
            for (NodeID newDst1: tyIter.second) {
                if (newTy.first) {
                    checks++;       // stat
                    resData.getSuccs(src, newTy).test_and_set(newDst1);
                }
                // old
                for (auto& tyIter2: oldData()->getSuccMap(newDst1)) {
                    Label newTy = binarySumm(lty, tyIter2.first);
                    if (newTy.first) {
                        checks += tyIter2.second.count();       // stat
                        resData.getSuccs(src, newTy) |= tyIter2.second;
                    }
                }
                // new
                for (auto& tyIter2: cflData()->getSuccMap(newDst1)) {
                    Label newTy = binarySumm(lty, tyIter2.first);
                    if (newTy.first) {
                        checks += tyIter2.second.count();       // stat
                        resData.getSuccs(src, newTy) |= tyIter2.second;
                    }
                }
            }
        }

        // update old and new
        for (auto& tyIter: cflData()->getSuccMap(src)) {
            oldData()->getSuccs(src, tyIter.first) |= tyIter.second;
            tyIter.second.clear();
        }
        for (auto& tyIter: resData.getSuccMap(src)) {
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
    numOfSumEdges = 0;
    std::set<int> s = {M, V, DV, FV,A,Abar};

    for (auto iter1 = oldData()->begin(); iter1 != oldData()->end(); ++iter1) {
        for (auto& iter2: iter1->second) {
            if (s.find(iter2.first.first) != s.end())
                numOfSumEdges += iter2.second.count();
        }
    }

    std::set<int> s1 = {A};
    for (auto it = oldData()->begin(); it != oldData()->end(); ++it) {
        oldData()->addEdge(it->first, it->first, std::make_pair(A, 0));
    }
    for (auto iter1 = oldData()->begin(); iter1 != oldData()->end(); ++iter1) {
        for (auto& iter2: iter1->second) {
            if (s1.find(iter2.first.first) != s1.end()) {
                numOfTEdges += iter2.second.count() * 2;
//                numOfSumEdges += iter2.second.count() * 2;
            }
        }
    }
}
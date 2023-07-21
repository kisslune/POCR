/* -------------------- TRVFA.cpp ------------------ */
//
// Created by kisslune on 5/14/23.
//

#include "VFA/VFAnalysis.h"

using namespace SVF;

void FocrVFA::initSolver()
{
    for (CFLEdge* edge : graph()->getIVFGEdges())
    {
        NodeID srcId = edge->getSrcID();
        NodeID dstId = edge->getDstID();

        if (edge->getEdgeKind() == IVFG::DirectVF)
        {
            cflData()->addEdge(srcId,dstId,Label(a,0));
            pushIntoWorklist(srcId,dstId,Label(a,0));
        }
        if (edge->getEdgeKind() == IVFG::CallVF)
            cflData()->addEdge(srcId, dstId, Label(call, edge->getEdgeIdx()));
        if (edge->getEdgeKind() == IVFG::RetVF)
            cflData()->addEdge(srcId, dstId, Label(ret, edge->getEdgeIdx()));
    }

    for (auto it = graph()->begin(); it != graph()->end(); ++it)
    {
        NodeID nId = it->first;
        ecg.addNode(nId);
        matchCallRet(nId, nId);
    }
}


void FocrVFA::solve()
{
    while (!isWorklistEmpty())
    {
        CFLItem item = popFromWorklist();
        addArc(item.src(), item.dst());
    }

    ecg.countECGEdges();
}


void FocrVFA::addArc(NodeID src, NodeID dst)
{
    if (ecg.isReachable(src, dst))
        return;

    std::unordered_map<NodeID, NodeBS>* newEdgeMapPtr;

    if (ecg.isReachable(dst, src))     // src --> dst is a back edge
        newEdgeMapPtr = &ecg.insertBackEdge(src, dst);
    else                                    // src --> dst is a forward edge
        newEdgeMapPtr = &ecg.insertForwardEdge(src, dst);

    for (auto& it1 : *newEdgeMapPtr)
        for (auto newDst : it1.second)
            matchCallRet(it1.first, newDst);    // it1.first == newSrc
}


void FocrVFA::matchCallRet(NodeID u, NodeID v)
{
    /// vertical handling of matched parentheses
    for (auto& srcIt : cflData()->getPreds(u))
    {
        if (srcIt.first.first == call)
            for (auto& dstIt : cflData()->getSuccs(v))
            {
                if (dstIt.first.first == ret && srcIt.first.second == dstIt.first.second)
                    for (NodeID srcCallParent : srcIt.second)
                        for (NodeID dstRetChild : dstIt.second)
                        {
                            stat->checks++;
                            pushIntoWorklist(srcCallParent, dstRetChild, Label(A, 0));
                        }
            }
    }
}


void FocrVFA::addCl(NodeID u, u32_t idx, ECGNode* vNode)
{
    NodeID v = vNode->id;
    if (!checkAndAddEdge(u, v, Label(Cl, idx)))
        return;

    for (auto succ : vNode->successors)
        addCl(u, idx, succ);
}


void FocrVFA::countSumEdges()
{
    /// calculate checks
    stat->checks += ecg.checks;

    /// calculate summary edges
    for (auto& it1 : cflData()->getSuccMap())
        for (auto& it2 : it1.second)
            if (it2.first.first == call)
            {
                for (NodeID dst : it2.second)
                    addCl(it1.first, it2.first.second, ecg.getNode(dst));
            }

    VFAnalysis::countSumEdges();
    stat->numOfSumEdges += ecg.countReachablePairs();
    stat->numOfSEdges += ecg.countReachablePairs();
}

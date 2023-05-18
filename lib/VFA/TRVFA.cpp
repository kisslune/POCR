/* -------------------- TRVFA.cpp ------------------ */
//
// Created by kisslune on 5/14/23.
//

#include "VFA/VFAnalysis.h"

using namespace SVF;

void TRVFA::initSolver()
{
    for (CFLEdge* edge: graph()->getIVFGEdges())
    {
        NodeID srcId = edge->getSrcID();
        NodeID dstId = edge->getDstID();

        if (edge->getEdgeKind() == IVFG::DirectVF)
            pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
        if (edge->getEdgeKind() == IVFG::CallVF)
            callParents[dstId][edge->getEdgeIdx()].insert(srcId);
        if (edge->getEdgeKind() == IVFG::RetVF)
            retChildren[srcId][edge->getEdgeIdx()].insert(dstId);
    }

    for (auto it = graph()->begin(); it != graph()->end(); ++it)
    {
        NodeID nId = it->first;
        ecg.addNode(nId);
//        matchCallRet(nId, nId);
    }
}


void TRVFA::solve()
{
    while (!isWorklistEmpty())
    {
        CFLItem item = popFromWorklist();
        addArc(item.src(), item.dst());
    }

    ecg.countECGEdges();
}


void TRVFA::addArc(NodeID src, NodeID dst)
{
    if (ecg.isReachable(src,dst))
        return;

//    if (ecg.isReachable(dst, src))   // src --> dst is a back edge
//        return ecg.insertBackEdge(src, dst);

    ecg.insertForthEdge(src, dst);      // src --> dst is a forth edge
}


// TODO: test

//void TRVFA::addArc(NodeID src, NodeID dst)
//{
//    if (isReachable(src, dst))
//        return;
//
//    if (isReachable(dst, src))   // src --> dst is a back edge
//        return insertBackEdge(src, dst);
//
//    insertForthEdge(src, dst);      // src --> dst is a forth edge
//}
//
//
//void TRVFA::insertForthEdge(NodeID i, NodeID j)
//{
//    ecg.addEdge(i, j, ECG::Forth);
//    ECGNode* vi = ecg.getNode(i);
//    ECGNode* vj = ecg.getNode(j);
//    searchBack(vi, vj);
//}
//
//
//void TRVFA::insertBackEdge(NodeID i, NodeID j)
//{
//    ecg.addEdge(i, j, ECG::Back);
//    ECGNode* vi = ecg.getNode(i);
//    ECGNode* vj = ecg.getNode(j);
//    ecg.resetBackSrc(vi);
//    ecg.resetBackDst(vj);
//
//    searchForthInCycle(vj);
//    searchBackInCycle(vi);
//}
//
//
//void TRVFA::searchBack(ECGNode* vi, ECGNode* vj)
//{
//    std::stack<ECGEdge> edgesToRemove;
//    for (auto succ: vi->successors)
//    {
//        ECGNode* vSucc = succ.first;
//        if (isReachable(vj->id, vSucc->id))
//            edgesToRemove.push(ECGEdge(vi, vSucc));
//    }
//    while (!edgesToRemove.empty())
//    {
//        auto edge = edgesToRemove.top();
//        edgesToRemove.pop();
//        ECG::removeEdge(edge);
//    }
//
//    searchForth(vi, vj);
//
//    for (auto pred: vi->predecessors)
//    {
//        ECGNode* vPred = pred.first;
//        if (!isReachable(vPred->id, vj->id))
//            searchBack(vPred, vj);
//    }
//}
//
//
//void TRVFA::searchForth(ECGNode* vi, ECGNode* vj)
//{
//    ecg.setReachable(vi->id, vj->id);
//    matchCallRet(vi->id, vj->id);       // vertical propagation
//
//    for (auto succ: vj->successors)
//    {
//        ECGNode* vSucc = succ.first;
//        if (!isReachable(vi->id, vSucc->id))
//            searchForth(vi, vSucc);
//    }
//}
//
//
//void TRVFA::searchForthInCycle(ECGNode* vj)
//{
//    ecg.setReachable(ecg.backSrc()->id, vj->id);
//    matchCallRet(ecg.backSrc()->id, vj->id);       // vertical propagation
//
//    std::unordered_map<ECGNode*, ECGEdgeTy> vjSuccs = vj->successors;   // copy successors to traverse
//    for (auto succ: vjSuccs)
//    {
//        ECGNode* vSucc = succ.first;
//        if (succ.second == ECG::Back && isReachable(vj->id, ecg.backSrc()->id))
//        {
//            ECG::removeEdge(vj, vSucc);
//            if (!isReachable(ecg.backSrc()->id, vSucc->id))
//            {
//                ecg.resetBackEdge(ecg.backSrc(), vSucc);
//                searchForthInCycle(vSucc);
//            }
//        }
//        else if (!isReachable(ecg.backSrc()->id, vSucc->id))
//            searchForthInCycle(vSucc);
//    }
//}
//
//
//void TRVFA::searchBackInCycle(ECGNode* vi)
//{
//    searchForth(vi, ecg.backDst());
//
//    std::unordered_map<ECGNode*, ECGEdgeTy> viPreds = vi->predecessors;   // copy predecessors to traverse
//    for (auto pred: viPreds)
//    {
//        ECGNode* vPred = pred.first;
//        if (pred.second == ECG::Back && isReachable(ecg.backDst()->id, vi->id))
//        {
//            ECG::removeEdge(vPred, vi);
//            if (!isReachable(vPred->id, ecg.backDst()->id))
//            {
//                ecg.resetBackEdge(vPred, ecg.backDst());
//                searchBackInCycle(vPred);
//            }
//        }
//        else if (!isReachable(vPred->id, ecg.backDst()->id))
//            searchBackInCycle(vPred);
//    }
//}
//
//
//void TRVFA::matchCallRet(NodeID u, NodeID v)
//{
//    // vertical handling of matched parentheses
//    auto callIt = callParents.find(u);
//    if (callIt == callParents.end())
//        return;
//
//    auto retIt = retChildren.find(v);
//    if (retIt == retChildren.end())
//        return;
//
//    for (auto& callParentIt: callIt->second)
//    {
//        auto retChildIt = retIt->second.find(callParentIt.first);
//        if (retChildIt != retIt->second.end())
//        {
//            for (NodeID callP: callParentIt.second)
//                for (NodeID retC: retChildIt->second)
//                {
//                    stat->checks++;
//                    pushIntoWorklist(callP, retC, std::make_pair(A, 0));
//                }
//        }
//    }
//}
//
//
//void TRVFA::addCl(NodeID u, u32_t idx, ECGNode* vNode)
//{
//    NodeID v = vNode->id;
//    if (!clChildren[u][idx].insert(v).second)
//        return;
//
//    for (auto succ: vNode->successors)
//        addCl(u, idx, succ.first);
//}


void TRVFA::countSumEdges()
{
//    for (auto& iter: callParents)
//    {
//        for (auto& iter2: iter.second)
//        {
//            for (auto src: iter2.second)
//                addCl(src, iter2.first, ecg.getNode(iter.first));
//        }
//    }

    stat->numOfSumEdges = 0;
    stat->numOfSumEdges += ecg.countReachablePairs();

//    for (auto& iter: clChildren)
//        for (auto& iter2: iter.second)
//            stat->numOfSumEdges += iter2.second.size();
}

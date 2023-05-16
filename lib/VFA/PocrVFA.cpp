//
// Created by kisslune on 3/13/22.
//

#include "VFA/VFAnalysis.h"

using namespace SVF;

void PocrVFA::initSolver()
{
    for (CFLEdge* edge: graph()->getIVFGEdges()) {
        NodeID srcId = edge->getSrcID();
        NodeID dstId = edge->getDstID();

        if (edge->getEdgeKind() == IVFG::DirectVF)
            pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
        if (edge->getEdgeKind() == IVFG::CallVF)
            callParents[dstId][edge->getEdgeIdx()].insert(srcId);
        if (edge->getEdgeKind() == IVFG::RetVF)
            retChildren[srcId][edge->getEdgeIdx()].insert(dstId);
    }

    for (auto it = graph()->begin(); it != graph()->end(); ++it) {
        NodeID nId = it->first;
        treeData.addInd(nId, nId);
        matchCallRet(nId, nId);
    }
}


void PocrVFA::solve()
{
    while (!isWorklistEmpty()) {
        CFLItem item = popFromWorklist();
        addArc(item.src(), item.dst());
    }
}


void PocrVFA::addArc(NodeID src, NodeID dst)
{
    if (!hasA(src, dst)) {
        for (auto& iter: treeData.indMap[src]) {
            meld(iter.first, treeData.getNode(iter.first, src), treeData.getNode(dst, dst));
        }
    }
}


void PocrVFA::meld(NodeID x, TreeNode* uNode, TreeNode* vNode)
{
    stat->checks++;

    TreeNode* newVNode = treeData.addInd(x, vNode->id);
    addEdge(x, vNode->id, std::make_pair(A, 0));
    if (!newVNode)
        return;

    matchCallRet(x, vNode->id);

    treeData.insertEdge(uNode, newVNode);
    for (TreeNode* vChild: vNode->children) {
        meld(x, newVNode, vChild);
    }
}


/*!
 * Matching call A ret
 */
void PocrVFA::matchCallRet(NodeID u, NodeID v)
{
    // vertical handling of matched parentheses
    auto callIt = callParents.find(u);
    if (callIt == callParents.end())
        return;

    auto retIt = retChildren.find(v);
    if (retIt == retChildren.end())
        return;

    for (auto& callParentIt: callIt->second) {
        auto retChildIt = retIt->second.find(callParentIt.first);
        if (retChildIt != retIt->second.end()) {
            for (NodeID callP: callParentIt.second)
                for (NodeID retC: retChildIt->second) {
                    stat->checks++;
                    pushIntoWorklist(callP, retC, std::make_pair(A, 0));
                }
        }
    }
}


bool PocrVFA::hasA(NodeID src, NodeID dst)
{
    stat->checks++;
    return treeData.hasInd(src, dst);
}


void PocrVFA::addCl(NodeID u, u32_t idx, TreeNode* vNode)
{
    NodeID v = vNode->id;
    if (!clChildren[u][idx].insert(v).second)
        return;

    for (auto child: vNode->children)
        addCl(u, idx, child);
}


void PocrVFA::countSumEdges()
{
//    for (auto& iter: callParents) {
//        for (auto& iter2: iter.second) {
//            for (auto src: iter2.second)
//                addCl(src, iter2.first, treeData.getNode(iter.first, iter.first));
//        }
//    }

    stat->numOfSumEdges = 0;

    for (auto& iter: treeData.indMap) {
        stat->numOfSumEdges += iter.second.size();
    }
//    for (auto& iter: clChildren) {
//        for (auto& iter2: iter.second) {
//            stat->numOfSumEdges += iter2.second.size();
//        }
//    }
}



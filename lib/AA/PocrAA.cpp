//
// Created by kisslune on 3/20/22.
//

#include "AA/AliasAnalysis.h"

using namespace SVF;

void PocrAA::initSolver()
{
    // init graph edges
    for (CFLEdge* edge: graph()->getPEGEdges())
    {
        NodeID srcId = edge->getSrcID();
        NodeID dstId = edge->getDstID();

        if (edge->getEdgeKind() == PEG::Asgn)
        {
            aParents[dstId].insert(srcId);
            pushIntoWorklist(edge->getSrcID(), edge->getDstID(), std::make_pair(a, 0));
        }
        if (edge->getEdgeKind() == PEG::Deref)
            dChildren[srcId].insert(dstId);
        if (edge->getEdgeKind() == PEG::Gep)
            fChildren[srcId][edge->getEdgeIdx()].insert(dstId);
    }

    // init dataset
    for (auto it = graph()->begin(); it != graph()->end(); ++it)
    {
        NodeID nId = it->first;
        hybridData.addInd(nId, nId);
        setV(nId, nId);
    }
}


void PocrAA::solve()
{
    while (!isWorklistEmpty())
    {
        CFLItem item = popFromWorklist();
        Label type = item.type();
        NodeID src = item.src();
        NodeID dst = item.dst();

        if (type.first == a)
            addArc(src, dst);
        else if (type.first == M)
        {
            setM(src, dst);
            addV(hybridData.getNode(src, src), hybridData.getNode(dst, dst));
        }
        else if (type.first == V)
            addV(hybridData.getNode(src, src), hybridData.getNode(dst, dst));
    }
}


void PocrAA::addArc(NodeID src, NodeID dst)
{
    if (hasA(src, dst))
        return;

    for (auto& iter: hybridData.indMap[src])
    {
        meld(iter.first, hybridData.getNode(iter.first, src), hybridData.getNode(dst, dst));
    }

    for (NodeID vSrc: vChildren[src])
        pushIntoWorklist(vSrc, dst, std::make_pair(V, 0));
}


void PocrAA::meld(NodeID x, TreeNode* uNode, TreeNode* vNode)
{
    stat->checks++;

    TreeNode* newVNode = hybridData.addInd(x, vNode->id);
    if (!newVNode)
        return;

    hybridData.insertEdge(uNode, newVNode);
    for (TreeNode* vChild: vNode->children)
    {
        meld(x, newVNode, vChild);
    }
}


bool PocrAA::hasA(NodeID u, NodeID v)
{
    stat->checks++;

    return hybridData.hasInd(u, v);
}


void PocrAA::addV(TreeNode* u, TreeNode* v)
{
    if (!setV(u->id, v->id))
        return;

    for (TreeNode* vChild: v->children)
    {
        addV(u, vChild);
    }

    for (TreeNode* uChild: u->children)
    {
        addV(uChild, v);
    }
}


bool PocrAA::setV(NodeID src, NodeID dst)
{
    stat->checks++;

    if (!vChildren[src].insert(dst).second)
        return false;
    vChildren[dst].insert(src);
    stat->checks++;

    // solve the parentheses of d and dealloc edges
    checkdEdges(src, dst);
    checkfEdges(src, dst);
    return true;
}


bool PocrAA::hasM(NodeID src, NodeID dst)
{
    stat->checks++;

    if (src == dst)
        return true;

    auto srcM = mChildren.find(src);

    if (srcM == mChildren.end())
        return false;

    return (srcM->second.find(dst) != srcM->second.end());
}


void PocrAA::setM(NodeID src, NodeID dst)
{
    mChildren[src].insert(dst);
    mChildren[dst].insert(src);
}


/*!
 * Matching parentheses dbar V d
 */
void PocrAA::checkdEdges(NodeID src, NodeID dst)
{
    auto srcD = dChildren.find(src);
    if (srcD == dChildren.end())
        return;

    auto dstD = dChildren.find(dst);
    if (dstD == dChildren.end())
        return;

    for (auto srcChild: srcD->second)
        for (auto dstChild: dstD->second)
            if (!hasM(srcChild, dstChild))
            {
                pushIntoWorklist(srcChild, dstChild, std::make_pair(M, 0));
                for (NodeID srcP: aParents[srcChild])
                {
                    pushIntoWorklist(srcP, dstChild, std::make_pair(a, 0));
                }
                for (NodeID dstP: aParents[dstChild])
                {
                    pushIntoWorklist(dstP, srcChild, std::make_pair(a, 0));
                }
            }
}


/*!
 * Matching parentheses fbar V f
 */
void PocrAA::checkfEdges(NodeID src, NodeID dst)
{
    auto srcD = fChildren.find(src);
    if (srcD == fChildren.end())
        return;

    auto dstD = fChildren.find(dst);
    if (dstD == fChildren.end())
        return;

    for (auto& srcChildIt: srcD->second)
    {
        auto dstChildIt = dstD->second.find(srcChildIt.first);
        if (dstChildIt != dstD->second.end())
        {
            for (NodeID srcChild: srcChildIt.second)
            {
                for (NodeID dstChild: dstChildIt->second)
                {
                    stat->checks++;
                    pushIntoWorklist(srcChild, dstChild, std::make_pair(V, 0));
                }
            }
        }
    }
}


void PocrAA::countSumEdges()
{
    for (auto& iter: dChildren)
        for (auto dTgt: iter.second)
        {
            setM(dTgt, dTgt);
            /// dv
            for (auto dst: vChildren[iter.first])
                dvChildren[dTgt].insert(dst);
        }

    /// fv
    for (auto& iter: fChildren)
    {
        for (auto& iter2: iter.second)
        {
            for (auto& src: iter2.second)
                for (auto& dst: vChildren[iter.first])
                    fvChildren[src][iter2.first].insert(dst);
        }
    }

    stat->numOfSumEdges = 0;

    for (auto& iter: hybridData.indMap)
    {
        stat->numOfSumEdges += iter.second.size() * 2;
    }
    for (auto& iter: vChildren)
    {
        stat->numOfSumEdges += iter.second.size();
    }
    for (auto& iter: mChildren)
    {
        stat->numOfSumEdges += iter.second.size();
    }
    for (auto& iter: dvChildren)
    {
        stat->numOfSumEdges += iter.second.size();
    }
    for (auto& iter: fvChildren)
    {
        for (auto& iter2: iter.second)
        {
            stat->numOfSumEdges += iter2.second.size();
        }
    }
}

//
// Created by kisslune on 6/6/22.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;


void PocrCFL::initSolver()
{
    StdCFL::initSolver();

    /// Init ptrees and strees for each node
    for (auto lbl: grammar()->transitiveLabels)
    {
        ptrees[lbl] = new HybridData();
        strees[lbl] = new HybridData();
    }
    for (auto it = graph()->begin(); it != graph()->end(); ++it)
    {
        for (auto lbl: grammar()->transitiveLabels)
        {
            NodeID nId = it->first;
            ptrees[lbl]->addInd(nId, nId);
            strees[lbl]->addInd(nId, nId);
        }
    }
    /// Remove transitive rules
    Set<CFG::LabelIDTy> transitiveSymbols;
    for (auto rule: grammar()->binaryRules)
    {
        for (auto lhs: rule.second)
            if (lhs == rule.first.first && lhs == rule.first.second)
                transitiveSymbols.insert(lhs);
    }
    for (auto lbl: transitiveSymbols)
        grammar()->binaryRules[std::make_pair(lbl, lbl)].erase(lbl);
}


void PocrCFL::solve()
{
    while (!isWorklistEmpty())
    {
        CFLItem item = popFromWorklist();

        if (grammar()->isTransitive(item.type().first) && isPrimary(item))
        {
            /// Process primary transitive items
            procPrimaryItem(item);
        }
        else
        {
            /// Process other items by ordinary routine
            processCFLItem(item);
        }
    }
}


void PocrCFL::procPrimaryItem(CFLItem item)
{
    tmpPrimaryItem = item;
    char lbl = item.type().first;
    NodeID px = item.dst();
    NodeID sx = item.src();
    /// py is the root of ptree(item.src())
    TreeNode* py = ptrees[lbl]->getNode(item.src(), item.src());
    /// sy is the root of stree(item.dst())
    TreeNode* sy = strees[lbl]->getNode(item.dst(), item.dst());

    traverseStree(lbl, px, py, sx, sy);
}


void PocrCFL::traverseStree(char lbl, NodeID px, TreeNode* py, NodeID sx, TreeNode* sy)
{
    traversePtree(lbl, px, py, sx, sy);

    for (auto sz: sy->children)
    {
        if (!strees[lbl]->hasInd(py->id, sz->id))
            /// Only handle sz when py -lbl-> sz does not exist
            traverseStree(lbl, px, py, sy->id, sz);
    }
}


void PocrCFL::traversePtree(char lbl, NodeID px, TreeNode* py, NodeID sx, TreeNode* sy)
{
    updateTrEdge(lbl, px, py, sx, sy);

    for (auto pz: py->children)
    {
        if (!ptrees[lbl]->hasInd(sy->id, pz->id))
            /// Only handle pz when pz -lbl->sy does not exist
            traversePtree(lbl, py->id, pz, sx, sy);
    }
}


/*!
 * Add a node py to ptree(sy) as a child of px; \n
 * add a node sy to stree(py) as a child of sx.
 */
bool PocrCFL::updateTrEdge(char lbl, NodeID px, TreeNode* py, NodeID sx, TreeNode* sy)
{
    stat->checks++;

    TreeNode* newPy = ptrees[lbl]->addInd(sy->id, py->id);
    if (!newPy)
        return false;   // the node py is already in ptree(sy)

    /// ptrees[lbl]->getNode(sy->id, px) refers to the node px in ptree(sy)
    ptrees[lbl]->insertEdge(ptrees[lbl]->getNode(sy->id, px), newPy);

    TreeNode* newSy = strees[lbl]->addInd(py->id, sy->id);
    /// strees[lbl]->getNode(py->id, sx) refers to the node sx in stree(py)
    strees[lbl]->insertEdge(strees[lbl]->getNode(py->id, sx), newSy);
    /// Update adjacency lists
    cflData()->addEdge(py->id, sy->id, Label(lbl, 0));
    /// Push the new edge into worklist as a secondary edge
//    if (py->id == tmpPrimaryItem.src() || sy->id == tmpPrimaryItem.dst())
    pushIntoWorklist(py->id, sy->id, Label(lbl, 0), false);

    return true;
}


/*!
 * All the transitive items generated during ordinary solving is regarded as primary
 */
bool PocrCFL::pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary)
{
    return CFLBase::pushIntoWorklist(src, dst, ty, isPrimary);
}


void PocrCFL::processCFLItem(CFLItem item)
{
    for (Label newTy: unarySumm(item.type()))
        if (addEdge(item.src(), item.dst(), newTy))
        {
            stat->checks++;
            pushIntoWorklist(item.src(), item.dst(), newTy);
        }

    for (auto& iter: cflData()->getSuccMap(item.dst()))
    {
        Label rty = iter.first;
        for (Label newTy: binarySumm(item.type(), rty))
            if (newTy == item.type() && grammar()->isTransitive(rty.first))
            {
                /// X ::= X A
                TreeNode* dst = strees[rty.first]->getNode(item.dst(), item.dst());
                checkStree(newTy, item.src(), dst);
            }
            else
            {
                NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
                stat->checks += iter.second.count();
                for (NodeID diffDst: diffDsts)
                    pushIntoWorklist(item.src(), diffDst, newTy);
            }
    }

    for (auto& iter: cflData()->getPredMap(item.src()))
    {
        Label lty = iter.first;
        for (Label newTy: binarySumm(lty, item.type()))
            if (newTy == item.type() && grammar()->isTransitive(lty.first))
            {
                /// X ::= A X
                TreeNode* src = ptrees[lty.first]->getNode(item.src(), item.src());
                checkPtree(newTy, src, item.dst());
            }
            else
            {
                NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
                stat->checks += iter.second.count();
                for (NodeID diffSrc: diffSrcs)
                    pushIntoWorklist(diffSrc, item.dst(), newTy);
            }
    }
}


void PocrCFL::checkPtree(Label newLbl, TreeNode* src, NodeID dst)
{
    stat->checks++;
    for (auto child: src->children)
        if (addEdge(child->id, dst, newLbl))
        {
            pushIntoWorklist(child->id, dst, newLbl);
            checkPtree(newLbl, child, dst);
        }
}


void PocrCFL::checkStree(Label newLbl, NodeID src, TreeNode* dst)
{
    stat->checks++;
    for (auto child: dst->children)
        if (addEdge(src, child->id, newLbl))
        {
            pushIntoWorklist(src, child->id, newLbl);
            checkStree(newLbl, src, child);
        }
}
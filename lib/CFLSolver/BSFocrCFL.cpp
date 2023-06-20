//
// Created by kisslune on 6/12/23.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;


void BSFocrCFL::initSolver()
{
    StdCFL::initSolver();
    /// Initialize ECG
    for (auto lbl : grammar()->transitiveSymbols)
        ecgs[lbl] = new BSECG();
    /// Create ECG nodes
    for (auto it = graph()->begin(); it != graph()->end(); ++it)
    {
        for (auto lbl : grammar()->transitiveSymbols)
        {
            NodeID nId = it->first;
            ecgs[lbl]->addNode(nId);
        }
    }
    /// Remove transitive rules from binary-summarization list
    for (auto lbl : grammar()->transitiveSymbols)
        grammar()->binaryRules[std::make_pair(lbl, lbl)].erase(lbl);
}


void BSFocrCFL::processCFLItem(CFLItem item)
{
    /// Process primary transitive items
    if (grammar()->isTransitive(item.label().first) && isPrimary(item))
    {
        procPrimaryItem(item);
        return;
    }

    /// Process other items
    for (Label newTy : unarySumm(item.label()))
        if (checkAndAddEdge(item.src(), item.dst(), newTy))
            pushIntoWorklist(item.src(), item.dst(), newTy);

    for (auto& iter : cflData()->getSuccMap(item.dst()))
    {
        Label rty = iter.first;
        for (Label newTy : binarySumm(item.label(), rty))
            if (newTy == item.label() && grammar()->isTransitive(rty.first))
            {
                /// X ::= X A
                checkSuccs(newTy, item.src(), item.dst(), ecgs[rty.first]);
            }
            else
            {
                NodeBS diffDsts = checkAndAddEdges(item.src(), iter.second, newTy);
                for (NodeID diffDst : diffDsts)
                    pushIntoWorklist(item.src(), diffDst, newTy);
            }
    }

    for (auto& iter : cflData()->getPredMap(item.src()))
    {
        Label lty = iter.first;
        for (Label newTy : binarySumm(lty, item.label()))
            if (newTy == item.label() && grammar()->isTransitive(lty.first))
            {
                /// X ::= A X
                checkPreds(newTy, item.src(), item.dst(), ecgs[lty.first]);
            }
            else
            {
                NodeBS diffSrcs = checkAndAddEdges(iter.second, item.dst(), newTy);
                for (NodeID diffSrc : diffSrcs)
                    pushIntoWorklist(diffSrc, item.dst(), newTy);
            }
    }
}


/*!
 * Transitive items generated in this methods are marked as secondary
 */
void BSFocrCFL::procPrimaryItem(CFLItem item)
{
    CFGSymbTy symb = item.label().first;
    NodeID src = item.src();
    NodeID dst = item.dst();

    if (ecgs[symb]->isReachable(src, dst))
        return;

    if (ecgs[symb]->isReachable(dst, src))   // // src --> dst is a back edge
        return insertBackEdge(src, dst, symb);

    insertForthEdge(src, dst, symb);
}


void BSFocrCFL::checkPreds(Label newLbl, NodeID src, NodeID dst, BSECG* g)
{
    for (auto pred : g->getPreds(src))
        if (checkAndAddEdge(pred, dst, newLbl))
        {
            pushIntoWorklist(pred, dst, newLbl);
            checkPreds(newLbl, pred, dst, g);
        }
}


void BSFocrCFL::checkSuccs(Label newLbl, NodeID src, NodeID dst, BSECG* g)
{
    for (auto succ : g->getSuccs(dst))
        if (checkAndAddEdge(src, succ, newLbl))
        {
            pushIntoWorklist(src, succ, newLbl);
            checkSuccs(newLbl, src, succ, g);
        }
}


/*!
 * All the transitive items generated during ordinary solving is regarded as primary
 */
bool BSFocrCFL::pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary)
{
    return CFLBase::pushIntoWorklist(src, dst, ty, isPrimary);
}


/// -------------------- Overridden ECG methods -------------------------------

void BSFocrCFL::insertForthEdge(NodeID i, NodeID j, CFGSymbTy symb)
{
    searchBack(i, j, symb);
    ecgs[symb]->addEdge(i, j);
}


void BSFocrCFL::searchBack(NodeID i, NodeID j, CFGSymbTy symb)
{
    std::stack<ECGEdge> edgesToRemove;
    for (auto succ : ecgs[symb]->getSuccs(i))
    {
        if (ecgs[symb]->isReachable(j, succ))
            edgesToRemove.push(ECGEdge(i, succ));
    }
    while (!edgesToRemove.empty())
    {
        auto edge = edgesToRemove.top();
        edgesToRemove.pop();
        ecgs[symb]->removeEdge(edge);
    }

    searchForth(i, j, symb);

    for (auto pred : ecgs[symb]->getPreds(i))
    {
        if (!ecgs[symb]->isReachable(pred, j))
            searchBack(pred, j, symb);
    }
}


void BSFocrCFL::searchForth(NodeID i, NodeID j, CFGSymbTy symb)
{
    updateTrEdge(i, j, symb);
    for (auto succ : ecgs[symb]->getSuccs(j))
    {
        if (!ecgs[symb]->isReachable(i, succ))
            searchForth(i, succ, symb);
    }
}


void BSFocrCFL::updateTrEdge(NodeID i, NodeID j, CFGSymbTy symb)
{
    stat->checks++;
    ecgs[symb]->setReachable(i, j);
    addEdge(i, j, Label(symb, 0));
    /// New ECG edges are added to worklist as secondary edges
    pushIntoWorklist(i, j, Label(symb, 0), false);
}


void BSFocrCFL::insertBackEdge(NodeID i, NodeID j, CFGSymbTy symb)
{
    searchBackInCycle(i, j, symb);
    ecgs[symb]->addEdge(i, j);
}


void BSFocrCFL::searchBackInCycle(NodeID i, NodeID j, CFGSymbTy symb)
{
    searchForth(i, j, symb);

    for (auto pred : ecgs[symb]->getPreds(i))
    {
        if (!ecgs[symb]->isReachable(pred, j))
            searchBackInCycle(pred, j, symb);
    }
}


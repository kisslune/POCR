//
// Created by kisslune on 30/5/23.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;


void UCFL::initSolver()
{
    StdCFL::initSolver();

    /// Initialize ECG
    for (auto lbl : grammar()->transitiveSymbols)
        ecgs[lbl] = new ECG();
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


void UCFL::processCFLItem(CFLItem item)
{
    /// Process primary transitive items
    if (grammar()->isTransitive(item.label().first) && isPrimary(item))
    {
        procPrimaryItem(item);
        return;
    }

    /// Process other items
    for (Label newTy : unarySumm(item.label()))
        if (addEdge(item.src(), item.dst(), newTy))
        {
            stat->checks++;
            pushIntoWorklist(item.src(), item.dst(), newTy);
        }

    for (auto& iter : cflData()->getSuccMap(item.dst()))
    {
        Label rty = iter.first;
        for (Label newTy : binarySumm(item.label(), rty))
            if (newTy == item.label() && grammar()->isTransitive(rty.first))
            {
                /// X ::= X A
                ECGNode* dst = ecgs[rty.first]->getNode(item.dst());
                checkSuccs(newTy, item.src(), dst);
            }
            else
            {
                NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
                stat->checks += iter.second.count();
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
                ECGNode* src = ecgs[lty.first]->getNode(item.src());
                checkPreds(newTy, src, item.dst());
            }
            else
            {
                NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
                stat->checks += iter.second.count();
                for (NodeID diffSrc : diffSrcs)
                    pushIntoWorklist(diffSrc, item.dst(), newTy);
            }
    }
}


void UCFL::procPrimaryItem(CFLItem item)
{
    // TODO: edges not added to CFLData
    CFGSymbTy symb = item.label().first;
    NodeID src = item.src();
    NodeID dst = item.dst();

    if (ecgs[symb]->isReachable(src, dst))
        return;

    if (ecgs[symb]->isReachable(dst, src))   // // src --> dst is a back edge
        return ecgs[symb]->insertBackEdge(src, dst);

    ecgs[symb]->insertForthEdge(src, dst);
}


void UCFL::checkPreds(Label newLbl, ECGNode* src, NodeID dst)
{
    stat->checks++;
    for (auto pred : src->predecessors)
        if (addEdge(pred.first->id, dst, newLbl))
        {
            pushIntoWorklist(pred.first->id, dst, newLbl);
            checkPreds(newLbl, pred.first, dst);
        }
}


void UCFL::checkSuccs(Label newLbl, NodeID src, ECGNode* dst)
{
    stat->checks++;
    for (auto succ : dst->successors)
        if (addEdge(src, succ.first->id, newLbl))
        {
            pushIntoWorklist(src, succ.first->id, newLbl);
            checkSuccs(newLbl, src, succ.first);
        }
}


/*!
 * All the transitive items generated during ordinary solving is regarded as primary
 */
bool UCFL::pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary)
{
    return CFLBase::pushIntoWorklist(src, dst, ty, isPrimary);
}



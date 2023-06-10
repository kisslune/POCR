//
// Created by kisslune on 30/5/23.
//

#include "CFLSolver/CFLSolver.h"

using namespace SVF;


void UCFL::initSolver()
{
    CFLOpt::ucflSumm.setValue(true);

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
    // grammar()->isInsertSymbol(ty.first)
    for (Label newTy : unarySumm(item.label()))
        if (!grammar()->isInsertSymbol(newTy.first))
        {
            // if(grammar()->isCountSymbol(newTy.first))
            // {
                // // figure out a more efficient way to store this, since it's causing 10s slower for i3
            //     followData.addEdge(item.src(), item.dst(), newTy);
            // }
            pushIntoWorklist(item.src(), item.dst(), newTy);
        }
        else if (updateEdge(item.src(), item.dst(), newTy))
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
            else if (!grammar()->isInsertSymbol(newTy.first))
            {
                NodeBS diffDsts = cflData()->getSuccs(item.dst(), rty);
                stat->checks += iter.second.count();
                for (NodeID diffDst : diffDsts)
                    pushIntoWorklist(item.src(), diffDst, newTy);
            }
            else 
            {
                NodeBS diffDsts = updateEdges(item.src(), iter.second, newTy);
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
            else if (!grammar()->isInsertSymbol(newTy.first))
            {
                NodeBS diffSrcs = cflData()->getPreds(item.src(), lty);
                // stat->checks += iter.second.count();
                for (NodeID diffSrc : diffSrcs)
                    pushIntoWorklist(diffSrc, item.dst(), newTy);
            }
            else
            {
                NodeBS diffSrcs = updateEdges(iter.second, item.dst(), newTy);
                stat->checks += iter.second.count();
                for (NodeID diffSrc : diffSrcs)
                    pushIntoWorklist(diffSrc, item.dst(), newTy);
            }
    }
}


/*!
 * Transitive items generated in this methods are marked as secondary
 */
void UCFL::procPrimaryItem(CFLItem item)
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


void UCFL::checkPreds(Label newLbl, ECGNode* src, NodeID dst)
{
    stat->checks++;
    for (auto pred : src->predecessors)
        if (updateEdge(pred.first->id, dst, newLbl))
        {
            pushIntoWorklist(pred.first->id, dst, newLbl);
            checkPreds(newLbl, pred.first, dst);
        }
}


void UCFL::checkSuccs(Label newLbl, NodeID src, ECGNode* dst)
{
    stat->checks++;
    for (auto succ : dst->successors)
        if (updateEdge(src, succ.first->id, newLbl))
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


/// -------------------- Methods for updating adjacency lists ---------------------------------

bool UCFL::updateEdge(NodeID srcId, NodeID dstId, Label ty)
{
    if (!CFLOpt::ucflSumm())
        return CFLBase::addEdge(srcId, dstId, ty);

    if (!ty.first)
        return false;
    if (grammar()->isInsertSymbol(ty.first))
        return cflData()->addEdge(srcId, dstId, ty);
    return followData.addEdge(srcId, dstId, ty);
}


NodeBS UCFL::updateEdges(NodeID srcId, const NodeBS& dstData, Label ty)
{
    if (!CFLOpt::ucflSumm())
        return CFLBase::addEdges(srcId, dstData, ty);

    if (!ty.first)
        return emptyBS;
    if (grammar()->isInsertSymbol(ty.first))
        return cflData()->addEdges(srcId, dstData, ty);
    return followData.addEdges(srcId, dstData, ty);
}


NodeBS UCFL::updateEdges(const NodeBS& srcData, NodeID dstId, Label ty)
{
    if (!CFLOpt::ucflSumm())
        return CFLBase::addEdges(srcData, dstId, ty);

    if (!ty.first)
        return emptyBS;
    if (grammar()->isInsertSymbol(ty.first))
        return cflData()->addEdges(srcData, dstId, ty);
    return followData.addEdges(srcData, dstId, ty);
}


/// -------------------- Overridden ECG methods -------------------------------

void UCFL::insertForthEdge(NodeID i, NodeID j, CFGSymbTy symb)
{
    ECGNode* vi = ecgs[symb]->getNode(i);
    ECGNode* vj = ecgs[symb]->getNode(j);
    searchBack(vi, vj, symb);

    ECG::addEdge(vi, vj, ECG::Forth);
}


void UCFL::searchBack(ECGNode* vi, ECGNode* vj, CFGSymbTy symb)
{
    std::stack<ECGEdge> edgesToRemove;
    for (auto succ : vi->successors)
    {
        ECGNode* vSucc = succ.first;
        if (ecgs[symb]->isReachable(vj->id, vSucc->id) && vj->id != vSucc->id)
            edgesToRemove.push(ECGEdge(vi, vSucc));
    }
    while (!edgesToRemove.empty())
    {
        auto edge = edgesToRemove.top();
        edgesToRemove.pop();
        ECG::removeEdge(edge);
    }

    searchForth(vi, vj, symb);

    for (auto pred : vi->predecessors)
    {
        ECGNode* vPred = pred.first;
        if (!ecgs[symb]->isReachable(vPred->id, vj->id))
            searchBack(vPred, vj, symb);
    }
}


void UCFL::searchForth(ECGNode* vi, ECGNode* vj, CFGSymbTy symb)
{
    updateTrEdge(vi->id, vj->id, symb);
    for (auto succ : vj->successors)
    {
        ECGNode* vSucc = succ.first;
        if (!ecgs[symb]->isReachable(vi->id, vSucc->id))
            searchForth(vi, vSucc, symb);
    }
}


void UCFL::updateTrEdge(NodeID i, NodeID j, CFGSymbTy symb)
{
    stat->checks++;
    ecgs[symb]->setReachable(i, j);
    updateEdge(i, j, Label(symb, 0));
    /// New ECG edges are added to worklist as secondary edges
    pushIntoWorklist(i, j, Label(symb, 0), false);
}


void UCFL::insertBackEdge(NodeID i, NodeID j, CFGSymbTy symb)
{
    ECGNode* vi = ecgs[symb]->getNode(i);
    ECGNode* vj = ecgs[symb]->getNode(j);

    searchBackInCycle(vi, vj, symb);

    ECG::addEdge(vi, vj, ECG::Back);
}


void UCFL::searchBackInCycle(ECGNode* vi, ECGNode* vj, CFGSymbTy symb)
{
    searchForth(vi, vj, symb);

    for (auto pred : vi->predecessors)
    {
        ECGNode* vPred = pred.first;
        if (!ecgs[symb]->isReachable(vPred->id, vj->id))
            searchBackInCycle(vPred, vj, symb);
    }
}


/// ---------------------- stats --------------------------------

void UCFL::countSumEdges()
{
    stat->numOfSumEdges = 0;
    stat->numOfCountEdges = 0;

    for (auto iter1 = cflData()->begin(); iter1 != cflData()->end(); ++iter1)
    {
        for (auto& iter2 : iter1->second)
        {
            stat->numOfSumEdges += iter2.second.count();
            if (grammar()->isCountSymbol(iter2.first.first))
                stat->numOfCountEdges += iter2.second.count();
        }
    }

    for (auto iter1 = followData.begin(); iter1 != followData.end(); ++iter1)
    {
        for (auto& iter2 : iter1->second)
        {
            stat->numOfSumEdges += iter2.second.count();
            if (grammar()->isCountSymbol(iter2.first.first))
                stat->numOfCountEdges += iter2.second.count();
        }
    }
}

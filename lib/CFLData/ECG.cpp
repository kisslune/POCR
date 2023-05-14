/* -------------------- ECG.cpp ------------------ */
//
// Created by kisslune on 4/27/23.
//

#include "CFLData/ECG.h"

using namespace SVF;


void ECG::insertForthEdge(NodeID i, NodeID j)
{
    addEdge(i, j, Forth);
    ECGNode* vi = getNode(i);
    ECGNode* vj = getNode(j);
    searchBack(vi, vj);
}


void ECG::searchBack(ECGNode* vi, ECGNode* vj)
{
    std::stack<ECGEdge> edgesToRemove;
    for (auto succ: vi->successors)
    {
        ECGNode* vSucc = succ.first;
        if (isReachable(vj->id, vSucc->id))
            edgesToRemove.push(ECGEdge(vi, vSucc));
    }
    while (!edgesToRemove.empty())
    {
        auto edge = edgesToRemove.top();
        edgesToRemove.pop();
        removeEdge(edge);
    }

    searchForth(vi, vj);

    for (auto pred: vi->predecessors)
    {
        ECGNode* vPred = pred.first;
        if (!isReachable(vPred->id, vj->id))
            searchBack(vPred, vj);
    }
}


void ECG::searchForth(ECGNode* vi, ECGNode* vj)
{
    setReachable(vi->id, vj->id);
    for (auto succ: vj->successors)
    {
        ECGNode* vSucc = succ.first;
        if (!isReachable(vi->id, vSucc->id))
            searchForth(vi, vSucc);
    }
}


void ECG::insertBackEdge(NodeID i, NodeID j)
{
    /// Set a back edge, backSrc and backDst will change during the following two-way search
    addEdge(i, j, Back);
    ECGNode* vi = getNode(i);
    ECGNode* vj = getNode(j);
    backSrc = vi;
    backDst = vj;

    searchForthInCycle(vj);
    searchBackInCycle(vi);
}


void ECG::resetBackEdge(ECGNode* vi, ECGNode* vj)
{
    removeEdge(backSrc, backDst);
    addEdge(vi, vj, Back);
    backSrc = vi;
    backDst = vj;
}


void ECG::searchForthInCycle(ECGNode* vj)
{
    setReachable(backSrc->id, vj->id);

    std::unordered_map<ECGNode*, ECGEdgeTy> vjSuccs = vj->successors;   // copy successors to traverse
    for (auto succ: vjSuccs)
    {
        ECGNode* vSucc = succ.first;
        if (succ.second == Back && isReachable(vj->id, backSrc->id))
        {
            removeEdge(vj, vSucc);
            if (!isReachable(backSrc->id, vSucc->id))
            {
                resetBackEdge(backSrc, vSucc);
                searchForthInCycle(vSucc);
            }
        }
        else if (!isReachable(backSrc->id, vSucc->id))
            searchForthInCycle(vSucc);
    }
}


void ECG::searchBackInCycle(ECGNode* vi)
{
    searchForth(vi, backDst);

    std::unordered_map<ECGNode*, ECGEdgeTy> viPreds = vi->predecessors;   // copy predecessors to traverse
    for (auto pred: viPreds)
    {
        ECGNode* vPred = pred.first;
        if (pred.second == Back && isReachable(backDst->id, vi->id))
        {
            removeEdge(vPred, vi);
            if (!isReachable(vPred->id, backDst->id))
            {
                resetBackEdge(vPred, backDst);
                searchBackInCycle(vPred);
            }
        }
        else if (!isReachable(vPred->id, backDst->id))
            searchBackInCycle(vPred);
    }
}



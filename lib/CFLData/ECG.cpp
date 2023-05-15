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
    _backSrc = vi;
    _backDst = vj;

    searchForthInCycle(vj);
    searchBackInCycle(vi);
}


void ECG::resetBackEdge(ECGNode* vi, ECGNode* vj)
{
    removeEdge(_backSrc, _backDst);
    addEdge(vi, vj, Back);
    _backSrc = vi;
    _backDst = vj;
}


void ECG::searchForthInCycle(ECGNode* vj)
{
    setReachable(_backSrc->id, vj->id);

    std::unordered_map<ECGNode*, ECGEdgeTy> vjSuccs = vj->successors;   // copy successors to traverse
    for (auto succ: vjSuccs)
    {
        ECGNode* vSucc = succ.first;
        if (succ.second == Back && isReachable(vj->id, _backSrc->id))
        {
            removeEdge(vj, vSucc);
            if (!isReachable(_backSrc->id, vSucc->id))
            {
                resetBackEdge(_backSrc, vSucc);
                searchForthInCycle(vSucc);
            }
        }
        else if (!isReachable(_backSrc->id, vSucc->id))
            searchForthInCycle(vSucc);
    }
}


void ECG::searchBackInCycle(ECGNode* vi)
{
    searchForth(vi, _backDst);

    std::unordered_map<ECGNode*, ECGEdgeTy> viPreds = vi->predecessors;   // copy predecessors to traverse
    for (auto pred: viPreds)
    {
        ECGNode* vPred = pred.first;
        if (pred.second == Back && isReachable(_backDst->id, vi->id))
        {
            removeEdge(vPred, vi);
            if (!isReachable(vPred->id, _backDst->id))
            {
                resetBackEdge(vPred, _backDst);
                searchBackInCycle(vPred);
            }
        }
        else if (!isReachable(vPred->id, _backDst->id))
            searchBackInCycle(vPred);
    }
}


u32_t ECG::countReachablePairs()
{
    u32_t retVal = 0;
    for (auto& iter: succMap) {
        retVal += iter.second.count();
    }
    return retVal;
}

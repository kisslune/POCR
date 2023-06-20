/* -------------------- ECG.cpp ------------------ */
//
// Created by kisslune on 4/27/23.
//

#include "CFLData/ECG.h"

using namespace SVF;


std::unordered_map<NodeID, NodeBS>& ECG::insertForthEdge(NodeID i, NodeID j)
{
    newEdgeMap.clear();

    ECGNode* vi = getNode(i);
    ECGNode* vj = getNode(j);
    searchBackward(vi, vj);

    addEdge(vi, vj, Forth);

    return newEdgeMap;
}


std::unordered_map<NodeID, NodeBS>& ECG::insertBackEdge(NodeID i, NodeID j)
{
    newEdgeMap.clear();

    /// Set a back edge, backSrc and backDst will change during the following two-way search
    ECGNode* vi = getNode(i);
    ECGNode* vj = getNode(j);
    searchBackwardInCycle(vi, vj);

    addEdge(vi, vj, Back);

    return newEdgeMap;
}


void ECG::searchBackward(ECGNode* vi, ECGNode* vj)
{
    /// When adding a new forth edge vi --> vj, we should remove all the edges vi' --> vj' such that
    /// vi' in pred(vi) and vj' in succ(vj).
    //@{
    std::stack<ECGEdge> edgesToRemove;
    for (auto succ : vi->successors)
    {
        ECGNode* vSucc = succ;
        if (isReachable(vj->id, vSucc->id) && vj->id != vSucc->id)
            edgesToRemove.push(ECGEdge(vi, vSucc));
    }
    while (!edgesToRemove.empty())
    {
        auto edge = edgesToRemove.top();
        edgesToRemove.pop();
        removeEdge(edge);
    }
    //@}

    searchForward(vi, vj);

    for (auto pred : vi->predecessors)
    {
        ECGNode* vPred = pred;
        if (!isReachable(vPred->id, vj->id))
            searchBackward(vPred, vj);
    }
}


void ECG::searchForward(ECGNode* vi, ECGNode* vj)
{
    setReachable(vi->id, vj->id);
    recordNewEdge(vi->id, vj->id);

    for (auto succ : vj->successors)
    {
        ECGNode* vSucc = succ;
        if (!isReachable(vi->id, vSucc->id))
            searchForward(vi, vSucc);
    }
}


void ECG::searchBackwardInCycle(ECGNode* vi, ECGNode* vj)
{
    searchForward(vi, vj);

    for (auto pred : vi->predecessors)
    {
        ECGNode* vPred = pred;
        if (!isReachable(vPred->id, vj->id))
            searchBackwardInCycle(vPred, vj);
    }
}


u32_t ECG::countReachablePairs()
{
    u32_t retVal = 0;
    for (auto& iter : reachableMap)
    {
        retVal += iter.second.count();
    }
    return retVal;
}


void ECG::countECGEdges()
{
    u32_t numOfEdges = 0;
    for (auto it : idToNodeMap)
    {
        numOfEdges += it.second->successors.size();
    }
    std::cout << "#ECGEdge" << "\t" << numOfEdges << std::endl;
}

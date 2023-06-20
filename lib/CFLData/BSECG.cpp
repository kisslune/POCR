//
// Created by kisslune on 6/11/23.
//

#include "CFLData/ECG.h"

using namespace SVF;


void BSECG::insertForthEdge(NodeID i, NodeID j)
{
    searchBack(i, j);
    addEdge(i, j);
}


void BSECG::searchBack(SVF::NodeID i, SVF::NodeID j)
{
    /// When adding a new forth edge vi --> vj, we should remove all the edges vi' --> vj' such that
    /// vi' in pred(vi) and vj' in succ(vj).
    //@{
    std::stack<ECGEdge> edgesToRemove;
    for (auto succ : succMap[i])
    {
        if (isReachable(j, succ))
            edgesToRemove.push(ECGEdge(i, succ));
    }
    while (!edgesToRemove.empty())
    {
        auto edge = edgesToRemove.top();
        edgesToRemove.pop();
        removeEdge(edge);
    }
    //@}

    searchForth(i, j);

    for (auto pred : predMap[i])
    {
        if (!isReachable(pred, j))
            searchBack(pred, j);
    }
}


void BSECG::searchForth(SVF::NodeID i, SVF::NodeID j)
{
    setReachable(i, j);
    for (auto succ : succMap[j])
    {
        if (!isReachable(i, succ))
            searchForth(i, succ);
    }
}


void BSECG::insertBackEdge(SVF::NodeID i, SVF::NodeID j)
{
    searchBackInCycle(i, j);
    addEdge(i, j);
}


void BSECG::searchBackInCycle(SVF::NodeID i, SVF::NodeID j)
{
    searchForth(i, j);

    for (auto pred : predMap[i])
    {
        if (!isReachable(pred, j))
            searchBackInCycle(pred, j);
    }
}
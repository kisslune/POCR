/* -------------------- ECG.h -- Edge Critical Graph ------------------ */
//
// Created by kisslune on 4/27/23.
//

#ifndef POCR_SVF_ECG_H
#define POCR_SVF_ECG_H

#include "CFLData.h"

namespace SVF
{
class ECG
{
public:
    enum ECGEdgeTy
    {
        Forth = 0,
        Back        // backward edges for tracking cycles
    };

    struct ECGNode
    {
        NodeID id;
        std::unordered_set<ECGNode*> successors;
        std::unordered_set<ECGNode*> predecessors;

        ECGNode(NodeID i) : id(i)
        {}

        inline bool operator==(const ECGNode& rhs) const
        { return id == rhs.id; }

        inline bool operator<(const ECGNode& rhs) const
        { return id < rhs.id; }
    };

    typedef std::pair<ECGNode*, ECGNode*> ECGEdge;   // first: src, second: dst

    struct VisitedStack
    {
        std::stack<ECGNode*> _stack;
        std::unordered_set<ECGNode*> _set;

        VisitedStack() = default;

        inline ECGNode* top()
        { return _stack.top(); }

        inline void push(ECGNode* n)
        {
            _set.insert(n);
            _stack.push(n);
        }

        inline void pop()
        {
            _set.erase(_stack.top());
            _stack.pop();
        }

        inline bool hasElement(ECGNode* n)
        { return _set.find(n) != _set.end(); }
    };

    /// calculators
    u32_t checks;

protected:
    std::unordered_map<NodeID, NodeID> nodeToRepMap;
    std::unordered_map<NodeID, ECGNode*> idToNodeMap;
    std::unordered_map<NodeID, NodeBS> reachableMap;
    std::unordered_map<NodeID, NodeBS> newEdgeMap;

    VisitedStack* visited;

public:
    /// constructor
    ECG() : checks(0), visited(nullptr)
    {};

    /// node methods
    //@{
    inline void addNode(NodeID id)
    {
        idToNodeMap[id] = new ECGNode(id);
        setReachable(id, id);
    }

    inline NodeID repNodeID(NodeID id) const
    {
        auto it = nodeToRepMap.find(id);
        if (it == nodeToRepMap.end())
            return id;
        return it->second;
    }
    //@}

    /// edge methods
    //@{
    inline ECGNode* getNode(NodeID id)
    {
        auto it = idToNodeMap.find(id);
        assert(it != idToNodeMap.end() && "Node not found!");
        return it->second;
    }

    inline bool hasEdge(NodeID src, NodeID dst)
    {
        ECGNode * srcNode = getNode(src);
        return srcNode->successors.find(getNode(dst)) != srcNode->successors.end();
    }

    inline void addEdge(NodeID src, NodeID dst)
    {
        ECGNode * srcNode = getNode(src);
        ECGNode * dstNode = getNode(dst);
        addEdge(srcNode, dstNode);
    }

    inline static void addEdge(ECGNode* src, ECGNode* dst)
    {
        src->successors.insert(dst);
        dst->predecessors.insert(src);
    }

    inline void removeEdge(NodeID src, NodeID dst)
    {
        ECGNode * vSrc = getNode(src);
        ECGNode * vDst = getNode(dst);
        removeEdge(vSrc, vDst);
    }

    inline static void removeEdge(ECGEdge edge)
    {
        ECGNode * vSrc = edge.first;
        ECGNode * vDst = edge.second;
        removeEdge(vSrc, vDst);
    }

    inline static void removeEdge(ECGNode* src, ECGNode* dst)
    {
        src->successors.erase(dst);
        dst->predecessors.erase(src);
    }
    //@}

    /// adjacency list methods
    //@{
    inline bool isReachable(NodeID n, NodeID tgt)
    {
        checks++;
        return reachableMap[n].test(tgt);
    }

    inline void setReachable(NodeID n, NodeID tgt)
    { reachableMap[n].set(tgt); }

    inline void recordNewEdge(NodeID n, NodeID tgt)
    { newEdgeMap[n].set(tgt); }
    //@}

    /// graph methods
    std::unordered_map<NodeID, NodeBS>& insertForwardEdge(NodeID i, NodeID j);
    std::unordered_map<NodeID, NodeBS>& insertBackEdge(NodeID i, NodeID j);
    void searchForward(ECGNode* vi, ECGNode* vj);
    void searchBackward(ECGNode* vi, ECGNode* vj);
    void searchBackwardInCycle(ECGNode* vi, ECGNode* vj);   // no use vj
    void simplifyCycle(ECGNode* vi);
    void stepInto(ECGNode* vi);

    /// calculator
    u32_t countReachablePairs();
    void countECGEdges();
};


/*!
 * Bit set-based ECG
 */
class BSECG
{
public:
    typedef std::pair<NodeID, NodeID> ECGEdge;

protected:
    std::unordered_map<NodeID, NodeBS> predMap;
    std::unordered_map<NodeID, NodeBS> succMap;
    std::unordered_map<NodeID, NodeBS> reachableMap;
    const NodeBS emptyBS;

public:
    BSECG() = default;

    /// node methods
    //@{
    inline void addNode(NodeID id)
    {
        setReachable(id, id);
    }

    inline const NodeBS& getSuccs(NodeID id) const
    {
        auto it = succMap.find(id);
        if (it == succMap.end())
            return emptyBS;
        return it->second;
    }

    inline const NodeBS& getPreds(NodeID id) const
    {
        auto it = predMap.find(id);
        if (it == predMap.end())
            return emptyBS;
        return it->second;
    }
    //@}

    /// edge methods
    //@{
    inline bool hasEdge(NodeID src, NodeID dst)
    {
        if (succMap[src].test(dst))
            return true;
        if (succMap[src].test(dst))
            return true;
        return false;
    }

    inline void addEdge(NodeID src, NodeID dst)
    {
        succMap[src].set(dst);
        predMap[dst].set(src);
    }

    inline void removeEdge(ECGEdge edge)
    { removeEdge(edge.first, edge.second); }

    inline void removeEdge(NodeID src, NodeID dst)
    {
        succMap[src].reset(dst);
        succMap[src].reset(dst);
        predMap[dst].reset(src);
        predMap[dst].reset(src);
    }
    //@}

    /// Reachability info methods
    //@{
    inline bool isReachable(NodeID n, NodeID tgt)
    { return reachableMap[n].test(tgt); }

    inline void setReachable(NodeID n, NodeID tgt)
    { reachableMap[n].set(tgt); }
    //@}

    /// graph methods
    void insertForthEdge(NodeID i, NodeID j);
    void insertBackEdge(NodeID i, NodeID j);
    void searchForth(NodeID i, NodeID j);
    void searchBack(NodeID i, NodeID j);

    void searchBackInCycle(NodeID i, NodeID j);   // no use vj
};


}

#endif //POCR_SVF_ECG_H

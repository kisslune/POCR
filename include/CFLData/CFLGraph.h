//
// Created by kisslune on 7/4/22.
//

#ifndef POCR_SVF_CFLGRAPH_H
#define POCR_SVF_CFLGRAPH_H

#include "CFLEdge.h"
#include "CFLNode.h"
#include "CFG.h"

namespace SVF
{
/*!
 * CFL-reachability graph
 */
class CFLGraph : public GenericGraph<CFLNode, CFLEdge>
{
public:
    typedef llvm::DenseMap<NodeID, NodeID> NodeToRepMap;
    typedef llvm::DenseMap<NodeID, NodeBS> NodeToSubsMap;
    typedef FIFOWorkList<NodeID> WorkList;

protected:
    NodeToRepMap nodeToRepMap;
    NodeToSubsMap nodeToSubsMap;
    CFLEdge::CFLEdgeSetTy pegEdgeSet;
    u32_t maxNodeID;

    CFG* grammar;

public:
    /// Constructor
    CFLGraph(CFG* _grammar) : maxNodeID(0), grammar(_grammar)
    {
    }

    /// copy constructor
    CFLGraph(const CFLGraph& rhs)
    {
        copyBuild(rhs);
    }

    void readGraph(std::string fname);   /// build from graph file
    void copyBuild(const CFLGraph& rhs);           /// copy builder

    void destroy()
    {};

    /// Destructor
    virtual ~CFLGraph()
    {
        destroy();
    }

    /// Get/add/remove constraint node
    //@{
    inline const CFLEdge::CFLEdgeSetTy& getCFLEdges() const
    {
        return pegEdgeSet;
    }

    inline CFLNode* getNode(NodeID id) const
    {
        id = repNodeID(id);
        return getGNode(id);
    }

    inline void addNode(NodeID id)
    {
        if (hasCFLGNode(id))
            return;

        addGNode(id, new CFLNode(id));
        if (id > maxNodeID)
            maxNodeID = id;
    }

    inline bool hasCFLGNode(NodeID id) const
    {
        return hasGNode(id);
    }
    //@}

    // Find and get edges
    //@{
    inline bool hasEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind kind, u32_t idx)
    {
        CFLEdge edge(src, dst, kind, idx);
        return hasEdge(&edge);
    }

    inline CFLEdge* getEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind kind)
    {
        CFLEdge edge(src, dst, kind);
        auto eit = pegEdgeSet.find(&edge);
        assert(eit != pegEdgeSet.end() && "no such edge!!");
        return *eit;
    }

    inline bool hasEdge(CFLEdge* edge)
    {
        return pegEdgeSet.find(edge) != pegEdgeSet.end();
    }
    //@}

    ///Add a PAG edge into Edge map
    //@{
    /// Add terminal edge with specified type
    bool addEdge(NodeID srcId, NodeID dstId, CFLEdge::GEdgeKind k, u32_t idx = 0);
    bool addEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind k, u32_t idx = 0);
    //@}

    /// Remove direct edge from their src and dst edge sets
    void removeEdge(CFLEdge* edge);

    inline NodeID repNodeID(NodeID id) const
    {
        NodeToRepMap::const_iterator it = nodeToRepMap.find(id);
        if (it == nodeToRepMap.end())
            return id;
        else
            return it->second;
    }

    void writeGraph(std::string name);
};
}


#endif //POCR_SVF_CFLGRAPH_H

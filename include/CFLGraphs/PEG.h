/*
 * PEG.h
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#ifndef CFLGRAPH_H_
#define CFLGRAPH_H_

#include "CFLEdge.h"
#include "CFLNode.h"

namespace SVF
{
/*!
 * Program expression graph
 */
class PEG : public GenericGraph<CFLNode, CFLEdge>
{
public:
    enum PEGEdgeTy
    {
        Asgn,
        Deref,
        Gep
    };

    typedef llvm::DenseMap<NodeID, NodeID> NodeToRepMap;
    typedef llvm::DenseMap<NodeID, NodeBS> NodeToSubsMap;
    typedef FIFOWorkList<NodeID> WorkList;

protected:
    NodeToRepMap nodeToRepMap;
    NodeToSubsMap nodeToSubsMap;
    CFLEdge::CFLEdgeSetTy pegEdgeSet;
    u32_t maxNodeID;

public:
    /// Constructor
    PEG() : maxNodeID(0)
    {
    }

    /// copy constructor
    PEG(const PEG& rhs)
    {
        copyBuild(rhs);
    }

    void readGraph(std::string fname);   /// build from graph file
    void copyBuild(const PEG& rhs);           /// copy builder

    void destroy()
    {};

    /// Destructor
    virtual ~PEG()
    {
        destroy();
    }

    /// Get/add/remove constraint node
    //@{
    inline u32_t getMaxNodeID()
    {
        return maxNodeID;
    }

    inline const CFLEdge::CFLEdgeSetTy& getPEGEdges() const
    {
        return pegEdgeSet;
    }

    inline CFLNode* getPEGNode(NodeID id) const
    {
        id = repNodeID(id);
        return getGNode(id);
    }

    inline void addPEGNode(NodeID id)
    {
        if (hasPEGNode(id))
            return;

        addGNode(id, new CFLNode(id));
        if (id > maxNodeID)
            maxNodeID = id;
    }

    inline bool hasPEGNode(NodeID id) const
    {
        return hasGNode(id);
    }

    inline void removePEGNode(CFLNode* node)
    {
        removeGNode(node);
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
    void removePEGEdge(CFLEdge* edge);

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

#endif /* CFLGRAPH_H_ */

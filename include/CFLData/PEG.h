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
    PEG();

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

    /// Rep/sub methods
    //@{
    inline NodeID repNodeID(NodeID id) const
    {
        NodeToRepMap::const_iterator it = nodeToRepMap.find(id);
        if (it == nodeToRepMap.end())
            return id;
        else
            return it->second;
    }

    inline NodeBS& subNodeIds(NodeID id)
    {
        nodeToSubsMap[id].set(id);
        return nodeToSubsMap[id];
    }

    inline void setRep(NodeID node, NodeID rep)
    {
        nodeToRepMap[node] = rep;
    }

    inline void setSubs(NodeID node, NodeBS& subs)
    {
        nodeToSubsMap[node] |= subs;
    }

    inline void resetSubs(NodeID node)
    {
        nodeToSubsMap.erase(node);
    }
    //@}

    /// merge nodes
    void mergeNodeToRep(NodeID nodeId, NodeID newRepId);
    void reTargetDstOfEdge(CFLEdge* edge, CFLNode* newDstNode);
    void reTargetSrcOfEdge(CFLEdge* edge, CFLNode* newSrcNode);
    bool moveInEdgesToRepNode(CFLNode* node, CFLNode* rep);
    bool moveOutEdgesToRepNode(CFLNode* node, CFLNode* rep);
    void updateNodeRepAndSubs(NodeID nodeId, NodeID newRepId);

    inline bool moveEdgesToRepNode(CFLNode* node, CFLNode* rep)
    {
        bool gepIn = moveInEdgesToRepNode(node, rep);
        bool gepOut = moveOutEdgesToRepNode(node, rep);
        return (gepIn || gepOut);
    }

    /// Write graph to file
    void writeGraph(std::string name);
};

/* !
 * GenericGraphTraits specializations for the generic graph algorithms.
 * Provide graph traits for traversing from a constraint node using standard graph traversals.
 */
template<>
struct GenericGraphTraits<SVF::CFLNode*>
        : public GenericGraphTraits<SVF::GenericNode<CFLNode, CFLEdge>*>
{
};

/// Inverse GenericGraphTraits specializations for Value flow node, it is used for inverse traversal.
template<>
struct GenericGraphTraits<Inverse<SVF::CFLNode*> >
        : public GenericGraphTraits<Inverse<SVF::GenericNode<CFLNode, CFLEdge>*> >
{
};

template<>
struct GenericGraphTraits<SVF::PEG*>
        : public GenericGraphTraits<SVF::GenericGraph<CFLNode, CFLEdge>*>
{
    typedef CFLNode* NodeRef;
};

}

#endif /* CFLGRAPH_H_ */

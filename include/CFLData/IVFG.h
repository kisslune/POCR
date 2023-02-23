//===- IVFG.h -- Interprocedural valueflow graph-----------------------------//

/*
 * IVFG.h
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#ifndef IVFG_H_
#define IVFG_H_

#include "CFLEdge.h"
#include "CFLNode.h"

namespace SVF
{
/*!
 * Interprocedural valueflow graph
 */
class IVFG : public GenericGraph<CFLNode, CFLEdge>
{
public:
    enum IVFGEdgeTy
    {
        DirectVF,
        CallVF,
        RetVF
    };

    typedef llvm::DenseMap<NodeID, NodeID> NodeToRepMap;
    typedef llvm::DenseMap<NodeID, NodeBS> NodeToSubsMap;
    typedef FIFOWorkList<NodeID> WorkList;
    typedef u32_t CallsiteID;

protected:
    NodeToRepMap nodeToRepMap;
    NodeToSubsMap nodeToSubsMap;
    CFLEdge::CFLEdgeSetTy ivfgEdgeSet;

    bool addEdge(CFLEdge* edge)
    {
        return ivfgEdgeSet.insert(edge).second;
    }

public:
    /// Constructor
    IVFG();

    void readGraph(std::string fname);   /// build from graph file
    void copyBuild(const IVFG& rhs);  /// copy builder

    /// copy constructor
    IVFG(const IVFG& rhs)
    {
        copyBuild(rhs);
    }

    /// Get/add/remove constraint node
    //@{
    inline CFLNode* getIVFGNode(NodeID id) const
    {
        id = repNodeID(id);
        return getGNode(id);
    }

    inline void addIVFGNode(NodeID id)
    {
        if (hasIVFGNode(id))
            return;

        addGNode(id, new CFLNode(id));
    }

    inline bool hasIVFGNode(NodeID id) const
    {
        return hasGNode(id);
    }

    inline void removeIVFGNode(CFLNode* node)
    {
        removeGNode(node);
    }
    //@}

    // Find and get edges
    //@{
    inline const CFLEdge::CFLEdgeSetTy& getIVFGEdges() const
    {
        return ivfgEdgeSet;
    }

    inline bool hasEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind kind, u32_t idx)
    {
        CFLEdge edge(src, dst, kind, idx);
        return hasEdge(&edge);
    }

    inline CFLEdge* getEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind kind)
    {
        CFLEdge edge(src, dst, kind);
        auto eit = ivfgEdgeSet.find(&edge);
        assert(eit != ivfgEdgeSet.end() && "no such edge!!");
        return *eit;
    }

    inline bool hasEdge(CFLEdge* edge)
    {
        return ivfgEdgeSet.find(edge) != ivfgEdgeSet.end();
    }
    //@}

    ///Add a PAG edge into Edge map
    //@{
    bool addEdge(NodeID srcId, NodeID dstId, CFLEdge::GEdgeKind k, CallsiteID idx = 0);
    bool addEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind k, CallsiteID idx = 0);
    //@}

    /// Remove direct edge from their src and dst edge sets
    void removeIVFGEdge(CFLEdge* edge);

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

    /// Dump cflData into dot file
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
struct GenericGraphTraits<SVF::IVFG*>
        : public GenericGraphTraits<SVF::GenericGraph<CFLNode, CFLEdge>*>
{
    typedef CFLNode* NodeRef;
};

}

#endif

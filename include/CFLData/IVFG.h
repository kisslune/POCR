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
    IVFG()
    {}

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

    inline NodeID repNodeID(NodeID id) const
    {
        NodeToRepMap::const_iterator it = nodeToRepMap.find(id);
        if (it == nodeToRepMap.end())
            return id;
        else
            return it->second;
    }

    /// Dump cflData into dot file
    void writeGraph(std::string name);
};

}

#endif

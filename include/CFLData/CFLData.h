/*
 * CFLSolver.h
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#ifndef CFGDATA_H_
#define CFGDATA_H_

#include "Util/WorkList.h"
#include "BasicUtils.h"

namespace SVF
{
/*!
 * Adjacency-list graph representation
 */
class CFLData
{
public:
    typedef std::map<const Label, NodeBS> TypeMap;
    typedef std::unordered_map<NodeID, TypeMap> DataMap;
    typedef typename DataMap::iterator iterator;
    typedef typename DataMap::const_iterator const_iterator;

protected:
    DataMap succMap;
    DataMap predMap;
    const NodeBS emptyData;
    NodeBS diff;

public:
    // Constructor
    CFLData()
    {}

    // Destructor
    virtual ~CFLData() = default;

    virtual void clear()
    {
        succMap.clear();
        predMap.clear();
    }

    inline const_iterator begin() const
    { return succMap.begin(); }

    inline const_iterator end() const
    { return succMap.end(); }

    inline iterator begin()
    { return succMap.begin(); }

    inline iterator end()
    { return succMap.end(); }

    inline DataMap& getSuccMap()
    { return succMap; }

    inline DataMap& getPredMap()
    { return predMap; }

    inline TypeMap& getSuccMap(const NodeID key)
    { return succMap[key]; }

    inline TypeMap& getPredMap(const NodeID key)
    { return predMap[key]; }

    inline NodeBS& getSuccs(const NodeID key, const Label lbl)
    { return succMap[key][lbl]; }

    inline NodeBS& getPreds(const NodeID key, const Label lbl)
    { return predMap[key][lbl]; }

    // Alias data operations
    //@{
    inline void addEdge(const NodeID src, const NodeID dst, const Label lbl)
    {
        succMap[src][lbl].set(dst);
        predMap[dst][lbl].set(src);
    }

    inline bool checkAndAddEdge(const NodeID src, const NodeID dst, const Label lbl)
    {
        succMap[src][lbl].test_and_set(dst);
        return predMap[dst][lbl].test_and_set(src);
    }

    inline NodeBS checkAndAddEdges(const NodeID src, const NodeBS& dstSet, const Label lbl)
    {
        NodeBS newDsts;
        if (succMap[src][lbl] |= dstSet)
        {
            for (const NodeID dst : dstSet)
                if (predMap[dst][lbl].test_and_set(src))
                    newDsts.set(dst);
        }
        return newDsts;
    }

    inline NodeBS checkAndAddEdges(const NodeBS& srcSet, const NodeID dst, const Label lbl)
    {
        NodeBS newSrcs;
        if (predMap[dst][lbl] |= srcSet)
        {
            for (const NodeID src : srcSet)
                if (succMap[src][lbl].test_and_set(dst))
                    newSrcs.set(src);
        }
        return newSrcs;
    }

    inline bool hasEdge(const NodeID src, const NodeID dst, const Label lbl)
    {
        auto it1 = succMap.find(src);
        if (it1 == succMap.end())
            return false;

        auto it2 = it1->second.find(lbl);
        if (it2 == it1->second.end())
            return false;

        return it2->second.test(dst);
    }

    /* This is a dataset version, to be modified to a cflData version */
    inline void clearEdges(const NodeID key)
    {
        succMap[key].clear();
        predMap[key].clear();
    }
    //@}
};


/*!
 * Hybrid graph representation for transitive relations
 */
class HybridData
{
public:
    struct TreeNode
    {
        NodeID id;
        std::unordered_set<TreeNode*> children;

        TreeNode(NodeID nId) : id(nId)
        {}

        inline bool operator==(const TreeNode& rhs) const
        {
            return id == rhs.id;
        }

        inline bool operator<(const TreeNode& rhs) const
        {
            return id < rhs.id;
        }
    };


public:
    Map<NodeID, std::unordered_map<NodeID, TreeNode*>> indMap;   // indMap[v][u] points to node v in tree(u)

    HybridData()
    {}

    ~HybridData()
    {
        for (auto iter1 : indMap)
        {
            for (auto iter2 : iter1.second)
            {
                delete iter2.second;
                iter2.second = NULL;
            }
        }
    }

    bool hasInd(NodeID src, NodeID dst)
    {
        auto it = indMap.find(dst);
        if (it == indMap.end())
            return false;
        return (it->second.find(src) != it->second.end());
    }

    /// Add a node dst to tree(src)
    TreeNode* addInd(NodeID src, NodeID dst)
    {
        auto resIns = indMap[dst].insert(std::make_pair(src, new TreeNode(dst)));
        if (resIns.second)
            return resIns.first->second;
        return nullptr;
    }

    /// Get the node dst in tree(src)
    TreeNode* getNode(NodeID src, NodeID dst)
    {
        return indMap[dst][src];
    }

    /// add v into desc(x) as a child of u
    void insertEdge(TreeNode* u, TreeNode* v)
    {
        u->children.insert(v);
    }

    void addArc(NodeID src, NodeID dst)
    {
        if (!hasInd(src, dst))
        {
            for (auto iter : indMap[src])
            {
                meld(iter.first, getNode(iter.first, src), getNode(dst, dst));
            }
        }
    }

    void meld(NodeID x, TreeNode* uNode, TreeNode* vNode)
    {
        TreeNode* newVNode = addInd(x, vNode->id);
        if (!newVNode)
            return;

        insertEdge(uNode, newVNode);
        for (TreeNode* vChild : vNode->children)
        {
            meld(x, newVNode, vChild);
        }
    }
};


}   // end namespace SVF

#endif

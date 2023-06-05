//===- CFLSolver.h -- CFL reachability solver---------------------------------//

/*
 * CFLSolver.h
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#ifndef CFLRSOLVER_H_
#define CFLRSOLVER_H_

#include <SVFIR/SVFIR.h>
#include <Util/SVFUtil.h>
#include <Util/SCC.h>
#include "CFLData/CFLData.h"
#include "CFLOpt.h"
#include <fstream>
#include <thread>
#include <pthread.h>

namespace SVF
{
/*!
 *
 */
class CFLItem
{
public:
    NodeID _src;
    NodeID _dst;
    Label _type;
    bool _isPrimary;

    //Constructor
    CFLItem(NodeID e1, NodeID e2, Label e3, bool e4 = true) :
            _src(e1), _dst(e2), _type(e3), _isPrimary(e4)
    {}

    /// Methods for binary tree comparison
    //@{
    inline bool operator<(const CFLItem& rhs) const
    {
        if (_src != rhs._src)
            return _src < rhs._src;
        else if (_dst != rhs._dst)
            return _dst < rhs._dst;
        else
            return _type < rhs._type;
    }

    inline bool operator==(const CFLItem& rhs) const
    {
        return (_src == rhs._src) && (_dst == rhs._dst) && (_type == rhs._type) && (_isPrimary == rhs._isPrimary);
    }
    //@}

    NodeID src() const
    { return _src; }

    NodeID dst() const
    { return _dst; }

    Label type() const
    { return _type; }

    void setPrimary(bool v)
    { _isPrimary = v; }

    bool isPrimary() const
    { return _isPrimary; }
};


/*
 * Generic CFL solver for all-pair analysis based on different graphs (e.g. PAG, VFG, ThreadVFG)
 * Extend this class for sophisticated CFL-reachability resolution (e.g. field, flow, path)
 */
class CFLBase
{
public:
    typedef FIFOWorkList<CFLItem> WorkList;

protected:
    /// Worklist for resolution
    WorkList worklist;
    /// Alias dataset
    CFLData* _cflData;
    const NodeBS emptyBS;

public:
    /// Constructor
    CFLBase() : _cflData(NULL)
    {
        if (!_cflData)
            _cflData = new CFLData();
    }

    /// Destructor
    virtual ~CFLBase()
    { delete _cflData; }

    CFLData* cflData()
    { return _cflData; }

    // worklist operations
    //@{
    virtual inline CFLItem popFromWorklist()
    { return worklist.pop(); }

    virtual inline bool pushIntoWorklist(CFLItem item)
    { return worklist.push(item); }

    virtual inline bool pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary = true)
    { return pushIntoWorklist(CFLItem(src, dst, ty, isPrimary)); }

    virtual inline bool isInWorklist(CFLItem item)
    { return worklist.find(item); }

    virtual inline bool isInWorklist(NodeID src, NodeID dst, Label ty)
    { return isInWorklist(CFLItem(src, dst, ty)); }

    virtual inline bool isWorklistEmpty()
    { return worklist.empty(); }
    //@}

    //CFL data operations
    //@{
    virtual Set<Label> unarySumm(Label lty) = 0;
    virtual Set<Label> binarySumm(Label lty, Label rty) = 0;

    virtual bool addEdge(const NodeID srcId, const NodeID dstId, const Label ty)
    {
        if (!ty.first)
            return false;
        return cflData()->addEdge(srcId, dstId, ty);
    }

    virtual NodeBS addEdges(const NodeID srcId, const NodeBS& dstData, const Label ty)
    {
        if (!ty.first)
            return emptyBS;
        return cflData()->addEdges(srcId, dstData, ty);
    }

    virtual NodeBS addEdges(const NodeBS& srcData, const NodeID dstId, const Label ty)
    {
        if (!ty.first)
            return emptyBS;
        return cflData()->addEdges(srcData, dstId, ty);
    }
    //@}

    virtual void solve()
    {
        while (!isWorklistEmpty())
        {
            CFLItem item = popFromWorklist();
            processCFLItem(item);
        }
    }

    virtual void processCFLItem(CFLItem item)
    {
        /// Derive edges via unary production rules
        for (Label newTy : unarySumm(item.type()))
            if (addEdge(item.src(), item.dst(), newTy))
                pushIntoWorklist(item.src(), item.dst(), newTy);

        /// Derive edges via binary production rules
        //@{
        for (auto& iter : cflData()->getSuccMap(item.dst()))
        {
            Label rty = iter.first;
            for (Label newTy : binarySumm(item.type(), rty))
            {
                NodeBS diffDsts = addEdges(item.src(), iter.second, newTy);
                for (NodeID diffDst : diffDsts)
                    pushIntoWorklist(item.src(), diffDst, newTy);
            }
        }

        for (auto& iter : cflData()->getPredMap(item.src()))
        {
            Label lty = iter.first;
            for (Label newTy : binarySumm(lty, item.type()))
            {
                NodeBS diffSrcs = addEdges(iter.second, item.dst(), newTy);
                for (NodeID diffSrc : diffSrcs)
                    pushIntoWorklist(diffSrc, item.dst(), newTy);
            }
        }
        //@}
    };
};

}   // end namespace SVF


/*!
 * hash function
 */
template<>
struct std::hash<SVF::CFLItem>
{
    size_t operator()(const SVF::CFLItem& ls) const
    {
        SVF::Hash<std::pair<SVF::u32_t, SVF::u32_t >> h;
        return h(std::make_pair(ls.src(), ls.dst()));
    }
};


#endif /* CFLRSOLVER_H_ */

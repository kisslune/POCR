/*
 * CFLEdge.h
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#ifndef PEGEDGE_H_
#define PEGEDGE_H_

#include "SVFIR/SVFIR.h"
#include "SVF-LLVM/LLVMUtil.h"

namespace SVF
{

class CFLNode;

/*!
 * Self-defined edge for program expression resolution
 * 1) 6-7 bits of edgeFlag encode two main kinds of a PEG edge
 * 2) 0-5 bits of edgeFlag encodes sub-kinds defined in specific CFL grammar
 * 3) 8-63 bits of edgeFlag encodes other information
 */
typedef GenericEdge<CFLNode> GenericPEGEdgeTy;

class CFLEdge : public GenericPEGEdgeTy
{
public:
    /// edge type
    typedef GenericNode<CFLNode, CFLEdge>::GEdgeSetTy CFLEdgeSetTy;

protected:
    u32_t idx;
    bool _isDyckContributing;

public:
    /// Constructor
    CFLEdge(CFLNode* s, CFLNode* d, GEdgeFlag k, u32_t _idx = 0)
            : GenericPEGEdgeTy(s, d, k), idx(_idx)
    {
    }

    /// Destructor
    ~CFLEdge()
    {
    }

    inline const u32_t getEdgeIdx() const
    {
        return idx;
    }

    /// Methods for binary tree comparison
    //@{
    typedef struct equalGEdge
    {
        bool operator()(const CFLEdge* lhs, const CFLEdge* rhs) const
        {
            if (lhs->getEdgeKind() != rhs->getEdgeKind())
                return lhs->getEdgeKind() < rhs->getEdgeKind();
            else if (lhs->getSrcID() != rhs->getSrcID())
                return lhs->getSrcID() < rhs->getSrcID();
            else if (lhs->getDstID() != rhs->getDstID())
                return lhs->getDstID() < rhs->getDstID();
            else
                return lhs->getEdgeIdx() < rhs->getEdgeIdx();
        }
    } equalGEdge;

    inline bool operator==(const CFLEdge* rhs) const
    {
        return (rhs->getEdgeKind() == this->getEdgeKind() && rhs->getSrcID() == this->getSrcID()
                && rhs->getDstID() == this->getDstID() && rhs->getEdgeIdx() == this->getEdgeIdx());
    }
    //@}

    inline bool isDyckContributing() const
    {
        return _isDyckContributing;
    }

    inline void setDyckContributing()
    {
        _isDyckContributing = true;
    }
};

}

#endif

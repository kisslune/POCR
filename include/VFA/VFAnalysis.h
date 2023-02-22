/*
 * VFAnalysis.h
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#ifndef IVFANALYSIS_H_
#define IVFANALYSIS_H_

#include "CFLSolver/CFLBase.h"
#include "CFLData/IVFG.h"
#include "CFLSolver/CFLOpt.h"
#include "VFAStat.h"

namespace SVF
{
/*!
 * Basic valueflow analysis solver
 */
class VFAnalysis : public CFLBase
{
public:
    // Statistics
    //@{
    VFAStat* stat;

    u32_t numOfIteration;
    u32_t checks;
    u32_t numOfTEdges;
    u32_t numOfSumEdges;
    u32_t numOfAdd;
    double timeOfSolving;
    //@}

protected:
    /// Reanalyze flag
    bool reanalyze;
    /// Graph
    IVFG* _graph;
    std::string graphName;

public:

    /// Constructor
    VFAnalysis(std::string& gName) :
            numOfIteration(0),
            checks(0),
            numOfTEdges(0),
            numOfSumEdges(0),
            numOfAdd(0),
            timeOfSolving(0),
            reanalyze(false),
            _graph(nullptr),
            graphName(gName)
    {}

    /// Destructor
    virtual ~VFAnalysis()
    {
        delete _graph;
        _graph = NULL;
    }

    /// Graph operations
    //@{
    const inline IVFG* graph() const
    {
        return _graph;
    }

    virtual inline void setGraph(IVFG* g)
    {
        _graph = g;
    }

    virtual IVFG* graph()
    {
        return _graph;
    }
    //@}

    virtual void analyze();
    virtual void initialize();
    virtual void finalize();

    /// Print statistics results
    inline void dumpStat()
    {
        if (stat)
            stat->performStat();
    }

    // stat
    //{
    virtual void countSumEdges()
    {}
    //}

    static void timer()
    {
        sleep(CFLOpt::timeOut);
        assert(false && "Time out!!");
    }
};


/*!
 * Standard valueflow analysis solver
 */
class StdVFA : public VFAnalysis
{
public:
    enum Words
    {
        fault,
        a,
        call,
        ret,

        A,      // valueflow
        B,
        Cl,     // call a
    };

public:
    StdVFA(std::string gName) : VFAnalysis(gName)
    {}

    virtual void initialize();
    virtual void initSolver();

    // CFLItem operations
    //@{
    // Get a new edge kind from CFL grammar
    virtual Label binarySumm(Label lty, Label rty);
    Label unarySumm(Label lty);
    virtual void processCFLItem(CFLItem item);
    //@}

    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty);

    void countSumEdges();
};


/*!
 * POCR
 */
class PocrVFA : public StdVFA
{
public:
    typedef HybridData::TreeNode TreeNode;
    typedef std::unordered_map<NodeID, std::unordered_map<u32_t, std::unordered_set<NodeID>>> CallRetMap;
    typedef std::unordered_map<NodeID, std::unordered_set<NodeID>> ChildrenMap;

protected:
    HybridData treeData;
    CallRetMap callParents;
    CallRetMap retChildren;
    ChildrenMap sChildren;
    CallRetMap clChildren;

public:
    PocrVFA(std::string& gName) : StdVFA(gName)
    {}

    void initSolver();
    virtual void solve();

    virtual void addArc(NodeID src, NodeID dst);
    void meld(NodeID x, TreeNode* uNode, TreeNode* vNode);
    bool hasA(NodeID src, NodeID dst);
    virtual void matchCallRet(NodeID u, NodeID v);
    void addCl(NodeID u, u32_t idx, TreeNode* vNode);

    void countSumEdges();
};


/*!
 * Graspan
 */
class GspanVFA : public StdVFA
{
protected:
    CFLData* _oldData;

public:
    GspanVFA(std::string& gName) : StdVFA(gName), _oldData(nullptr)
    {
        if (!_oldData)
            _oldData = new CFLData();
    }

    CFLData* oldData()
    {
        return _oldData;
    }

    void initSolver();
    virtual void solve();
    void countSumEdges();
};


/*!
 * Standard CFL solver with rewritten grammar
 */
class GRVFA : public StdVFA
{
public:
    GRVFA(std::string& gName) : StdVFA(gName)
    {}

    Label binarySumm(Label lty, Label rty);
    Label unarySumm(Label lty);
};


/*!
 * Graspan solver with rewritten grammar
 */
class GRGspanVFA : public GspanVFA
{
public:
    GRGspanVFA(std::string& gName) : GspanVFA(gName)
    {}

    Label binarySumm(Label lty, Label rty);
    Label unarySumm(Label lty);
};

}

#endif

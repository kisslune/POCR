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
#include "VFAStat.h"
#include "IVFGFold.h"
#include "IVFGInterDyck.h"
#include "CFLData/ECG.h"

namespace SVF
{
/*!
 * Basic valueflow analysis solver
 */
class VFAnalysis : public CFLBase
{
public:
    typedef SCCDetection<IVFG*> SCC;

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

    // Statistics
    VFAStat* stat;

protected:
    /// Reanalyze flag
    bool reanalyze;
    /// Graph
    IVFG* _graph;
    std::string graphName;
    /// Graph simplifiation
    SCC* scc;
    IVFGFold* ivfgFold;
    IVFGInterDyck* interDyck;

public:
    /// Constructor
    VFAnalysis(std::string& gName) : reanalyze(false),
                                     _graph(nullptr),
                                     graphName(gName),
                                     scc(nullptr),
                                     ivfgFold(nullptr),
                                     interDyck(nullptr)
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
    { return _graph; }

    virtual inline void setGraph(IVFG* g)
    { _graph = g; }

    virtual IVFG* graph()
    { return _graph; }
    //@}

    virtual void analyze();
    virtual void initialize();
    virtual void initSolver() = 0;
    virtual void finalize();
    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty);

    Set<Label> unarySumm(Label lty) override
    { return {}; }

    Set<Label> binarySumm(Label lty, Label rty) override
    { return {}; }

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

    /// Graph simplification
    //@{
    void simplifyGraph();
    void graphFolding();
    void interDyckGS();
    void SCCElimination();
    void SCCDetect();
    void mergeSCCCycle();
    void mergeSCCNodes(NodeID repNodeId, const NodeBS& subNodes);
    //@}
};


/*!
 * Standard valueflow analysis solver
 */
class StdVFA : public VFAnalysis
{
public:
    StdVFA(std::string gName) : VFAnalysis(gName)
    {}

    void initSolver() override;

    // CFLItem operations
    //@{
    // Get a new edge kind from CFL grammar
    Set<Label> binarySumm(Label lty, Label rty) override;
    Set<Label> unarySumm(Label lty) override;
    void processCFLItem(CFLItem item) override;
    //@}

    void countSumEdges() override;
};


/*!
 * POCR
 */
class PocrVFA : public VFAnalysis
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
    PocrVFA(std::string gName) : VFAnalysis(gName)
    {}

    void initSolver() override;
    void solve() override;

    virtual void addArc(NodeID src, NodeID dst);
    void meld(NodeID x, TreeNode* uNode, TreeNode* vNode);
    bool hasA(NodeID src, NodeID dst);
    virtual void matchCallRet(NodeID u, NodeID v);
    void addCl(NodeID u, u32_t idx, TreeNode* vNode);

    void countSumEdges() override;
};


/*!
 * VFA with transitive reduction
 */
class TRVFA : public VFAnalysis
{
public:
    typedef ECG::ECGNode ECGNode;
    typedef ECG::ECGEdge ECGEdge;
    typedef ECG::ECGEdgeTy ECGEdgeTy;
    typedef std::unordered_map<NodeID, std::unordered_map<u32_t, std::unordered_set<NodeID>>> CallRetMap;
    typedef std::unordered_map<NodeID, std::unordered_set<NodeID>> ChildrenMap;

protected:
    ECG ecg;
    CallRetMap callParents;
    CallRetMap retChildren;
    ChildrenMap sChildren;
    CallRetMap clChildren;

public:
    TRVFA(std::string gName) : VFAnalysis(gName)
    {}

    void initSolver() override;
    void solve() override;

    virtual void addArc(NodeID src, NodeID dst);
    virtual void matchCallRet(NodeID u, NodeID v);
    void addCl(NodeID u, u32_t idx, ECGNode* vNode);

    void insertForthEdge(NodeID i, NodeID j);
    void insertBackEdge(NodeID i, NodeID j);
    void searchForth(ECGNode* vi, ECGNode* vj);
    void searchBack(ECGNode* vi, ECGNode* vj);
//
//    void searchForthInCycle(ECGNode* vj);  // no use vi
    void searchBackInCycle(ECGNode* vi, ECGNode* vj);   // no use vj

    inline bool isReachable(NodeID src, NodeID dst)
    {
        stat->checks++;
        return ecg.isReachable(src, dst);
    }

    void countSumEdges() override;
};


/*!
 * Graspan
 */
class GspanVFA : public StdVFA
{
protected:
    CFLData* _oldData;

public:
    GspanVFA(std::string gName) : StdVFA(gName), _oldData(nullptr)
    {
        if (!_oldData)
            _oldData = new CFLData();
    }

    CFLData* oldData()
    {
        return _oldData;
    }

    void initSolver() override;
    void solve() override;
    void countSumEdges() override;
};


/*!
 * Standard CFL solver with rewritten grammar
 */
class GRVFA : public StdVFA
{
public:
    GRVFA(std::string gName) : StdVFA(gName)
    {}

    Set<Label> binarySumm(Label lty, Label rty) override;
    Set<Label> unarySumm(Label lty) override;
};


/*!
 * Graspan solver with rewritten grammar
 */
class GRGspanVFA : public GspanVFA
{
public:
    GRGspanVFA(std::string gName) : GspanVFA(gName)
    {}

    Set<Label> binarySumm(Label lty, Label rty) override;
    Set<Label> unarySumm(Label lty) override;
};

}

#endif

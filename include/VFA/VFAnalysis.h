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
                                     stat(nullptr),
                                     _graph(nullptr),
                                     graphName(gName),
                                     scc(nullptr),
                                     ivfgFold(nullptr),
                                     interDyck(nullptr)
    {}

    /// Destructor
    virtual ~VFAnalysis()
    {
        delete stat;
        delete _graph;
        delete scc;
        delete ivfgFold;
        delete interDyck;
    }

    /// Graph operations
    //@{
    const inline IVFG* graph() const
    { return _graph; }

    inline void setGraph(IVFG* g)
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

    bool checkAndAddEdge(NodeID src, NodeID dst, Label lbl) override;
    NodeBS checkAndAddEdges(NodeID src, const NodeBS& dstSet, Label lbl) override;
    NodeBS checkAndAddEdges(const NodeBS& srcSet, NodeID dst, Label lbl) override;

    /// stat
    //@{
    inline void dumpStat()
    {
        if (stat)
            stat->performStat();
    }

    virtual void countSumEdges();
    //@}

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

    /// CFLItem operations
    //@{
    Set<Label> binarySumm(Label lty, Label rty) override;
    Set<Label> unarySumm(Label lty) override;
    //@}
};


/*!
 * POCR
 */
class PocrVFA : public VFAnalysis
{
public:
    typedef HybridData::TreeNode TreeNode;

protected:
    HybridData hybridData;

public:
    PocrVFA(std::string gName) : VFAnalysis(gName)
    {}

    void initSolver() override;
    void solve() override;

    void matchCallRet(NodeID u, NodeID v);
    void addCl(NodeID u, u32_t idx, TreeNode* vNode);

    void countSumEdges() override;
};


/*!
 * VFA with transitive reduction
 */
class FocrVFA : public VFAnalysis
{
public:
    typedef ECG::ECGNode ECGNode;

protected:
    ECG ecg;

public:
    FocrVFA(std::string gName) : VFAnalysis(gName)
    {}

    void initSolver() override;
    void solve() override;

    void addArc(NodeID src, NodeID dst);
    void matchCallRet(NodeID u, NodeID v);
    void addCl(NodeID u, u32_t idx, ECGNode* vNode);

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

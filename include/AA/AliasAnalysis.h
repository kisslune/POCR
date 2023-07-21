//===- CFLAlias.h -- CFL Alias Check---------------------------------//

/*
 * CFLAlias.h
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#ifndef ALIASANALYSIS_H_
#define ALIASANALYSIS_H_

#include "CFLSolver/CFLBase.h"
#include "CFLData/PEG.h"
#include "AAStat.h"
#include "PEGFold.h"
#include "PEGInterDyck.h"
#include "CFLData/ECG.h"

namespace SVF
{
/*!
 * Basic alias analysis solver
 */
class AliasAnalysis : public CFLBase
{
public:
    typedef SCCDetection<PEG*> SCC;

    enum Words
    {
        fault,
        a,
        abar,
        d,
        dbar,
        f,
        fbar,

        M,
        V,
        DV,
        A,
        Abar,
        FV
    };
    /// Statistics
    AAStat* stat;

protected:
    /// Reanalyze flag
    bool reanalyze;
    /// Graph
    PEG* _graph;
    std::string graphName;

    /// Graph simplification
    SCC* scc;
    PEGFold* pegFold;
    PEGInterDyck* interDyck;

public:
    AliasAnalysis(std::string& gName) : stat(nullptr),
                                        reanalyze(false),
                                        _graph(nullptr),
                                        graphName(gName),
                                        scc(nullptr),
                                        pegFold(nullptr),
                                        interDyck(nullptr)
    {};

    virtual ~AliasAnalysis()
    {
        delete stat;
        delete _graph;
        delete scc;
        delete pegFold;
        delete interDyck;
    }

    /// Graph operations
    //@{
    const inline PEG* graph() const
    { return _graph; }

    inline void setGraph(PEG* g)
    { _graph = g; }

    inline virtual PEG* graph()
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

    /// Graph simplifcation
    //@{
    void simplifyGraph();
    void graphFolding();
    void interDyckGS();
    void SCCElimination();
    void SCCDetect();
    void mergeSCCCycle();
    void mergeSCCNodes(NodeID repNodeId, const NodeBS& subNodes);
    // @}
};


/*!
 * Standard CFL solver
 */
class StdAA : public AliasAnalysis
{
public:
    StdAA(std::string graphName) : AliasAnalysis(graphName)
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
class PocrAA : public AliasAnalysis
{
public:
    typedef HybridData::TreeNode TreeNode;

protected:
    HybridData hybridData;

public:
    PocrAA(std::string gName) : AliasAnalysis(gName)
    {}

    void initSolver() override;
    void solve() override;

    void checkdEdges(NodeID src, NodeID dst);
    void checkfEdges(NodeID src, NodeID dst);
    void addV(TreeNode* u, TreeNode* v);
    bool setV(NodeID src, NodeID dst);
    bool hasM(NodeID src, NodeID dst);
    void setM(NodeID src, NodeID dst);

    void countSumEdges() override;
};


/*!
 * Focr AA
 */
class FocrAA : public AliasAnalysis
{
public:
    typedef ECG::ECGNode ECGNode;
    typedef ECG::ECGEdge ECGEdge;
    typedef ECG::ECGEdgeTy ECGEdgeTy;
    typedef std::unordered_map<NodeID, std::unordered_map<u32_t, std::unordered_set<NodeID>>> CallRetMap;
    typedef std::unordered_map<NodeID, std::unordered_set<NodeID>> ChildrenMap;

protected:
    ECG ecg;

public:
    FocrAA(std::string gName) : AliasAnalysis(gName)
    {}

    void initSolver() override;
    void solve() override;

    void addArc(NodeID src, NodeID dst);
    void checkdEdges(NodeID src, NodeID dst);
    void checkfEdges(NodeID src, NodeID dst);
    void addV(ECGNode* u, ECGNode* v);
    bool setV(NodeID src, NodeID dst);
    bool hasM(NodeID src, NodeID dst);
    void setM(NodeID src, NodeID dst);

    void countSumEdges() override;
};

/*!
 * Graspan (single thread for collecting derivation info)
 */
class GspanAA : public StdAA
{
protected:
    CFLData* _oldData;

public:
    GspanAA(std::string gName) : StdAA(gName), _oldData(nullptr)
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
class GRAA : public StdAA
{
public:
    GRAA(std::string gName) : StdAA(gName)
    {};

    Set<Label> binarySumm(Label lty, Label rty);
};


/*!
 * Graspan solver with rewritten grammar
 */
class GRGspanAA : public GspanAA
{
public:
    GRGspanAA(std::string gName) : GspanAA(gName)
    {}

    Set<Label> binarySumm(Label lty, Label rty);
};

}

#endif
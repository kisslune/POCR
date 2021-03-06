//===- CFLAlias.h -- CFL Alias Check---------------------------------//

/*
 * CFLAlias.h
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#ifndef ALIASANALYSIS_H_
#define ALIASANALYSIS_H_

#include "CFLBase.h"
#include "CFLGraphs/PEG.h"
#include "AAStat.h"
#include "Util/Options.h"

namespace SVF
{
/*!
 * Basic alias analysis solver
 */
class AliasAnalysis : public CFLBase
{
public:
    /// Statistics
    //@{
    AAStat* stat;

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
    PEG* _graph;
    std::string graphName;

public:
    AliasAnalysis(std::string& gName);

    virtual ~AliasAnalysis()
    {
        destroy();
    }

    /// Graph operations
    //@{
    const inline PEG* graph() const
    {
        return _graph;
    }

    virtual inline void setGraph(PEG* g)
    {
        _graph = g;
    }

    virtual PEG* graph()
    {
        return _graph;
    }
    //@}

    virtual void initialize();
    virtual void destroy();
    virtual void finalize();
    virtual void analyze();
    void dumpStat();

    virtual void dumpAlias()
    {};

    // stat
    //{
    virtual void countSumEdges()
    {
    }
    //}

    static void timer()
    {
        sleep(Options::timeOut);
        assert(false && "Time out!!");
    }
};


/*!
 * Standard CFL solver
 */
class StdAA : public AliasAnalysis
{
public:
    enum Words
    {
        fault,
        epsilon,
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

protected:
    std::map<NodeID, NodeSet> valAlias;

public:
    StdAA(std::string graphName) :
            AliasAnalysis(graphName)
    {}

    virtual ~StdAA()
    {
        delete _graph;
        _graph = NULL;
    }

    virtual void initialize();
    virtual void initWorklist();
    virtual void finalize();

    // Alias data operations
    //@{
    virtual Label binarySumm(Label lty, Label rty);
    virtual Label unarySumm(Label lty);
    //@}

    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty);
    virtual void processCFLItem(CFLItem item);
    virtual void dumpAlias();
    virtual void countSumEdges();
};


/*!
 * POCR
 */
class PocrAA : public StdAA
{
public:
    typedef HybridData::TreeNode TreeNode;
    typedef std::unordered_map<NodeID, std::unordered_map<short, std::unordered_set<NodeID>>> CallRetMap;
    typedef std::unordered_map<NodeID, std::unordered_set<NodeID>> ChildrenMap;

protected:
    HybridData hybridData;
    std::unordered_map<NodeID, NodeID> dChildren;
    ChildrenMap aParents;
    CallRetMap fChildren;
    ChildrenMap vChildren;
    ChildrenMap mChildren;
    ChildrenMap dvChildren;
    CallRetMap fvChildren;

public:
    PocrAA(std::string gName) : StdAA(gName)
    {}

    void initWorklist();

    void solveWorklist();

    bool pushIntoWorklist(NodeID src, NodeID dst, Label ty)
    {
        return AliasAnalysis::pushIntoWorklist(src, dst, ty);
    }

    void addArc(NodeID src, NodeID dst);

    void meld(NodeID x, TreeNode* uNode, TreeNode* vNode);

    bool hasA(NodeID u, NodeID v);

    void checkdEdges(NodeID src, NodeID dst);

    void checkfEdges(NodeID src, NodeID dst);

    void addV(TreeNode* u, TreeNode* v);

    bool setV(NodeID src, NodeID dst);

    bool hasM(NodeID src, NodeID dst);

    void setM(NodeID src, NodeID dst);

    u32_t getNumOfAdd()
    {
        return numOfAdd;
    };

    double getAnalysisTime()
    {
        return timeOfSolving;
    };

    void countSumEdges();
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

    void initWorklist();

    virtual void solveWorklist();

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

    Label binarySumm(Label lty, Label rty);
};


/*!
 * Graspan solver with rewritten grammar
 */
class GRGspanAA : public GspanAA
{
public:
    GRGspanAA(std::string gName) : GspanAA(gName)
    {}

    Label binarySumm(Label lty, Label rty);
};

}

#endif
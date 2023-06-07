/* -------------------- CFLSolver.h ------------------ */
//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFLSOLVER_H
#define POCR_SVF_CFLSOLVER_H

#include "CFLBase.h"
#include "CFLData/CFLGraph.h"
#include "CFLStat.h"
#include "CFLData/ECG.h"

namespace SVF
{
/*!
 * Standard solver for CFL-reachability
 */
class StdCFL : public CFLBase
{
public:
    /// Statistics
    CFLStat* stat;

protected:
    bool reanalyze;
    std::string grammarName;
    std::string graphName;
    CFG* _grammar;
    CFLGraph* _graph;

public:
    StdCFL(std::string& _grammarName, std::string& _graphName) : stat(nullptr),
                                                                 reanalyze(false),
                                                                 grammarName(_grammarName),
                                                                 graphName(_graphName),
                                                                 _grammar(nullptr),
                                                                 _graph(nullptr)
    {}

    virtual ~StdCFL()
    {
        delete _grammar;
        _grammar = nullptr;
        delete _graph;
        _graph = nullptr;
    }

    /// Grammar
    //@{
    virtual CFG* grammar()
    { return _grammar; }

    /// Graph
    //@{
    const inline CFLGraph* graph() const
    { return _graph; }

    virtual CFLGraph* graph()
    { return _graph; }
    //@}

    virtual void initialize();
    virtual void initSolver();
    virtual void finalize();
    virtual void analyze();

    /// stat
    void dumpStat();
    virtual void countSumEdges();

    static void timer()
    {
        sleep(CFLOpt::timeOut);
        assert(false && "Time out!!");
    }

    /// rules
    //@{
    Set<Label> unarySumm(Label lty) override;
    Set<Label> binarySumm(Label lty, Label rty) override;
    //@}
    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty);
    virtual void processCFLItem(CFLItem item);
};


/*!
 *  POCR solver
 */
class PocrCFL : public StdCFL
{
public:
    typedef HybridData::TreeNode TreeNode;
    typedef Map<char, HybridData*> TransitiveLblMap;

protected:
    TransitiveLblMap ptrees;
    TransitiveLblMap strees;
    CFLItem tmpPrimaryItem;

public:
    PocrCFL(std::string& _grammarName, std::string& _graphName) : StdCFL(_grammarName, _graphName),
                                                                  tmpPrimaryItem(0, 0, Label(0, 0))
    {}

    void initSolver();
    virtual void solve();
    void procPrimaryItem(CFLItem item);
    void traversePtree(char lbl, NodeID px, TreeNode* py, NodeID sx, TreeNode* sy);
    void traverseStree(char lbl, NodeID px, TreeNode* py, NodeID sx, TreeNode* sy);
    bool updateTrEdge(char lbl, NodeID px, TreeNode* py, NodeID sx, TreeNode* sy);
    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary = true);
    virtual void processCFLItem(CFLItem item);
    void checkPtree(Label newLbl, TreeNode* src, NodeID dst);
    void checkStree(Label newLbl, NodeID src, TreeNode* dst);

    static bool isPrimary(CFLItem& item)
    { return item.isPrimary(); }
};


/*!
 * Hierarchical POCR
 */
class HPocrCFL : public PocrCFL
{
private:
    WorkList primaryList;

public:
    HPocrCFL(std::string& _grammarName, std::string& _graphName) : PocrCFL(_grammarName, _graphName)
    {}

    virtual void solve();
    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary = true);
};


/*!
 * Unidirectional CFL-reachability
 */
class UCFL : public StdCFL
{
public:
    typedef ECG::ECGNode ECGNode;
    typedef ECG::ECGEdge ECGEdge;
    typedef Map<CFGSymbTy, ECG*> ECGMap;

protected:
    ECGMap ecgs;
    CFLData followData;

public:
    UCFL(std::string& _grammarName, std::string& _graphName) : StdCFL(_grammarName, _graphName)
    {}

    /// UCFL methods
    void initSolver() override;
    void procPrimaryItem(CFLItem item);
    bool pushIntoWorklist(NodeID src, NodeID dst, Label ty, bool isPrimary = true) override;
    void processCFLItem(CFLItem item) override;
    void checkPreds(Label newLbl, ECGNode* src, NodeID dst);
    void checkSuccs(Label newLbl, NodeID src, ECGNode* dst);

    static bool isPrimary(CFLItem& item)
    { return item.isPrimary(); }

    bool updateEdge(NodeID srcId, NodeID dstId, Label ty);
    NodeBS updateEdges(NodeID srcId, const NodeBS& dstData, Label ty);
    NodeBS updateEdges(const NodeBS& srcData, NodeID dstId, Label ty);

    /// Overridden ECG methods
    void insertForthEdge(NodeID i, NodeID j, CFGSymbTy symb);
    void insertBackEdge(NodeID i, NodeID j, CFGSymbTy symb);
    void searchForth(ECGNode* vi, ECGNode* vj, CFGSymbTy symb);
    void searchBack(ECGNode* vi, ECGNode* vj, CFGSymbTy symb);
    void updateTrEdge(NodeID i, NodeID j, CFGSymbTy symb);
    void searchBackInCycle(ECGNode* vi, ECGNode* vj, CFGSymbTy symb);

    /// stat
    void countSumEdges() override;
};

}

#endif //POCR_SVF_CFLSOLVER_H

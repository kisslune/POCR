/* -------------------- CFLSolver.h ------------------ */
//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFLSOLVER_H
#define POCR_SVF_CFLSOLVER_H

#include "CFLBase.h"
#include "CFLData/CFLGraph.h"
#include "CFLStat.h"

namespace SVF
{
/*!
 * Standard solver for CFL-reachability
 */
class StdCFL : public CFLBase
{
public:
    /// Statistics
    //@{
    CFLStat* stat;

    u32_t numOfIteration;
    u32_t checks;
    u32_t numOfSumEdges;
    double timeOfSolving;
    //@}

protected:
    bool reanalyze;
    std::string grammarName;
    std::string graphName;
    CFG* _grammar;
    CFLGraph* _graph;

public:
    StdCFL(std::string& _grammarName, std::string& _graphName) :
            stat(nullptr),
            numOfIteration(0),
            checks(0),
            numOfSumEdges(0),
            timeOfSolving(0),
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
    {
        return _grammar;
    }

    /// Graph
    //@{
    const inline CFLGraph* graph() const
    {
        return _graph;
    }

    virtual CFLGraph* graph()
    {
        return _graph;
    }
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
//    Set<CFLItem> primaryItems;

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

    bool isPrimary(CFLItem& item)
    {
        return item.isPrimary();
    }
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
//class UCFL : public CFLBase
//{
//
//};

}


#endif //POCR_SVF_CFLSOLVER_H

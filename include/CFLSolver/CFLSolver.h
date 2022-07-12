/* -------------------- CFLSolver.h ------------------ */
//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFLSOLVER_H
#define POCR_SVF_CFLSOLVER_H

#include "CFLBase.h"
#include "CFLGraphs/CFLGraph.h"
#include "Util/Options.h"
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
    u32_t numOfTEdges;
    u32_t numOfSumEdges;
    u32_t numOfAdd;
    double timeOfSolving;
    //@}

protected:
    bool reanalyze;
    std::string grammarName;
    std::string graphName;
    CFGrammar* _grammar;
    CFLGraph* _graph;

public:
    StdCFL(std::string& _grammarName, std::string& _graphName) :
            stat(nullptr),
            numOfIteration(0),
            checks(0),
            numOfTEdges(0),
            numOfSumEdges(0),
            numOfAdd(0),
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
    const inline CFGrammar* grammar() const
    {
        return _grammar;
    }

    virtual CFGrammar* grammar()
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
    virtual void initWorklist();
    virtual void finalize();
    virtual void analyze();

    /// stat
    void dumpStat();
    virtual void countSumEdges();

    static void timer()
    {
        sleep(Options::timeOut);
        assert(false && "Time out!!");
    }

    /// rules
    //@{
    virtual Label binarySumm(Label lty, Label rty);
    virtual Label unarySumm(Label lty);
    //@}
    virtual bool pushIntoWorklist(NodeID src, NodeID dst, Label ty);
    virtual void processCFLItem(CFLItem item);
};


///*!
// *
// */
//class

}


#endif //POCR_SVF_CFLSOLVER_H

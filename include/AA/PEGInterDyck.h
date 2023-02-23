/* -------------------- PEGInterDyck.h ------------------ */
//
// Created by kisslune on 2/22/23.
//

#ifndef POCR_SVF_PEGINTERDYCK_H
#define POCR_SVF_PEGINTERDYCK_H

#include "CFLData/PEG.h"

namespace SVF
{

class PEGInterDyck
{
public:
    typedef std::pair<u32_t, u32_t> Lbl;

private:
    PEG* peg;
    PEG* subGraph;
    std::unordered_map<NodeID, NodeBS> fastDyckRepSubMap;
    std::unordered_map<NodeID, Set<Lbl>> anchorMap;
    FIFOWorkList<NodeID> workList;

public:
    PEGInterDyck(PEG* _g) : peg(_g), subGraph(nullptr)
    {
    }

    ~PEGInterDyck()
    {
        delete subGraph;
        subGraph = nullptr;
    }

    void buildSubGraph();
    void fastDyck();    /// perform FastDyck on subgraph
    void pruneEdges();

    void toWorklist(NodeID nId);

    void printSubGraph(std::string fname);
};

}

#endif //POCR_SVF_PEGINTERDYCK_H

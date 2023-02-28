/* -------------------- IVFGInterDyck.h ------------------ */
//
// Created by kisslune on 2/23/23.
//

#ifndef POCR_SVF_IVFGINTERDYCK_H
#define POCR_SVF_IVFGINTERDYCK_H

#include "CFLData/IVFG.h"

namespace SVF
{
/*!
 *
 */
class IVFGInterDyck
{
public:
    typedef u32_t Lbl;

private:
    IVFG* ivfg;
    IVFG* subGraph;
    std::unordered_map<NodeID, NodeBS> fastDyckRepSubMap;
    std::unordered_map<NodeID, Set<Lbl>> anchorMap;
    FIFOWorkList<NodeID> workList;

public:
    IVFGInterDyck(IVFG* _g) : ivfg(_g), subGraph(nullptr)
    {
    }

    ~IVFGInterDyck()
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

#endif //POCR_SVF_IVFGINTERDYCK_H

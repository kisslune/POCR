//===- LG.cpp -- Program Expression Graph-----------------------------//

/*
 * LG.cpp
 *
 *  Created on: Aug 1, 2020
 *      Author: Yuxiang Lei
 */

#include "CFLSolver/CFLBase.h"
#include "Util/SVFUtil.h"
#include "CFLData/IVFG.h"
#include <iostream>

using namespace SVF;
using namespace SVFUtil;


IVFG::IVFG()
{
    /// Specify direct edge kinds for construction
    CFLNode::directEdgeKinds.clear();
    CFLNode::directEdgeKinds.insert(DirectVF);
}


/*!
 * Read a VFG from file
 */
void IVFG::readGraph(std::string fname)
{
    std::ifstream gFile;
    gFile.open(fname, std::ios::in);
    if (!gFile.is_open()) {
        std::cout << "error opening " << fname << std::endl;
        exit(0);
    }

    NodeSet nodeSet;
    std::string line;
    while (getline(gFile, line)) {
        std::vector<std::string> vec = CFLBase::split(line, '\t');
        if (vec.empty())
            continue;

        NodeID src = stoi(vec[0]);
        NodeID dst = stoi(vec[1]);
        std::string lbl = vec[2];
        addIVFGNode(src);
        addIVFGNode(dst);

        if (vec[2] == "a") {
            addEdge(src, dst, DirectVF);
        }
        if (vec[2] == "call_i") {
            addEdge(src, dst, CallVF, std::stoi(vec[3]));
        }
        if (vec[2] == "ret_i") {
            addEdge(src, dst, RetVF, std::stoi(vec[3]));
        }
    }

    gFile.close();
}


void IVFG::copyBuild(const IVFG& rhs)
{
    /// initialize nodes
    for (auto it = rhs.begin(), eit = rhs.end(); it != eit; ++it) {
        if (!it->second->hasIncomingEdge() && !it->second->hasOutgoingEdge())
            continue;
        addIVFGNode(it->first);
    }

    /// initialize edges
    for (auto edge: rhs.getIVFGEdges()) {
        addEdge(edge->getSrcID(), edge->getDstID(), edge->getEdgeKind(), edge->getEdgeIdx());
    }
}


//@{
bool IVFG::addEdge(NodeID srcId, NodeID dstId, CFLEdge::GEdgeKind k, CallsiteID idx)
{
    CFLNode* src = getIVFGNode(srcId);
    CFLNode* dst = getIVFGNode(dstId);
    return addEdge(src, dst, k, idx);
}

bool IVFG::addEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind k, CallsiteID idx)
{
    if (hasEdge(src, dst, k, idx))
        return false;

    if (k == DirectVF && src == dst)
        return false;

    CFLEdge* edge = new CFLEdge(src, dst, k, idx);
    bool added = addEdge(edge);
    src->addOutEdgeWithKind(edge, k);
    dst->addInEdgeWithKind(edge, k);
    return added;
}
//@}


/*!
 * Remove edge from their src and dst edge sets
 */
void IVFG::removeIVFGEdge(CFLEdge* edge)
{
    getIVFGNode(edge->getSrcID())->removeCFLOutEdge(edge);
    getIVFGNode(edge->getDstID())->removeCFLInEdge(edge);
    u32_t num1 = ivfgEdgeSet.erase(edge);
    delete edge;

    assert(num1 && "edge not in the set, can not remove!!!");
}


void IVFG::writeGraph(std::string name)
{
    std::ofstream outFile(name + ".g", std::ios::out);
    if (!outFile) {
        std::cout << "error opening file!";
        return;
    }

    std::set<NodeID> srcs;

    for (auto nIt = begin(); nIt != end(); ++nIt) {
        srcs.insert(nIt->first);
    }

    for (auto src: srcs) {
        auto node = getIVFGNode(src);
        for (auto edge: node->getOutEdgeWithTy(DirectVF)) {
            NodeID dst = edge->getDstID();
            outFile << src << "\t" << dst << "\ta" << std::endl;
        }
        for (auto edge: node->getOutEdgeWithTy(CallVF)) {
            NodeID dst = edge->getDstID();
            outFile << src << "\t" << dst << "\tcall_i" << "\t" << edge->getEdgeIdx() << std::endl;
        }
        for (auto edge: node->getOutEdgeWithTy(RetVF)) {
            NodeID dst = edge->getDstID();
            outFile << src << "\t" << dst << "\tret_i" << "\t" << edge->getEdgeIdx() << std::endl;
        }
    }

    outFile.close();
}

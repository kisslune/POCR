//===- PEG.cpp -- Program Expression Graph-----------------------------//

/*
 * PEG.cpp
 *
 *  Created on: Nov 22, 2019
 *      Author: Yuxiang Lei
 */

#include "CFLSolver/CFLBase.h"
#include "Util/SVFUtil.h"
#include "CFLGraphs/PEG.h"
#include <iostream>

using namespace SVF;
using namespace SVFUtil;


void PEG::readGraph(std::string fname)
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
        addPEGNode(src);
        addPEGNode(dst);

        if (vec[2] == "a") {
            addEdge(src, dst, Asgn);
        }
        if (vec[2] == "d") {
            addEdge(src, dst, Deref);
        }
        if (vec[2] == "f_i") {
            addEdge(src, dst, Gep, std::stoi(vec[3]));
        }
    }

    gFile.close();
}


void PEG::copyBuild(const PEG& rhs)
{
    /// initialize nodes
    for (auto it = rhs.begin(), eit = rhs.end(); it != eit; ++it) {
        if (!it->second->hasIncomingEdge() && !it->second->hasOutgoingEdge())
            continue;
        addPEGNode(it->first);
    }

    /// initialize edges
    for (auto edge: rhs.getPEGEdges()) {
        addEdge(edge->getSrcID(), edge->getDstID(), edge->getEdgeKind(), edge->getEdgeIdx());
    }
}


//@{
bool PEG::addEdge(NodeID srcId, NodeID dstId, CFLEdge::GEdgeKind k, u32_t idx)
{
    CFLNode* src = getPEGNode(srcId);
    CFLNode* dst = getPEGNode(dstId);
    return addEdge(src, dst, k, idx);
}

bool PEG::addEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind k, u32_t idx)
{
    if (hasEdge(src, dst, k, idx))
        return false;

    if (k == Asgn && src == dst)
        return false;

    CFLEdge* edge = new CFLEdge(src, dst, k, idx);
    bool added = pegEdgeSet.insert(edge).second;
    src->addOutEdgeWithKind(edge, k);
    dst->addInEdgeWithKind(edge, k);
    return added;
}
//@}


/*!
 * Remove edge from their src and dst edge sets
 */
void PEG::removePEGEdge(CFLEdge* edge)
{
    getPEGNode(edge->getSrcID())->removeCFLOutEdge(edge);
    getPEGNode(edge->getDstID())->removeCFLInEdge(edge);
    bool num1 = pegEdgeSet.erase(edge);
    delete edge;

    assert(num1 && "edge not in the set, can not remove!!!");
}


void PEG::writeGraph(std::string name)
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
        auto node = getPEGNode(src);
        for (auto edge: node->getOutEdgeWithTy(Asgn)) {
            NodeID dst = edge->getDstID();
            outFile << src << "\t" << dst << "\ta" << std::endl;
            outFile << dst << "\t" << src << "\tabar" << std::endl;
        }
        for (auto edge: node->getOutEdgeWithTy(Deref)) {
            NodeID dst = edge->getDstID();
            outFile << src << "\t" << dst << "\td" << std::endl;
            outFile << dst << "\t" << src << "\tdbar" << std::endl;
        }
        for (auto edge: node->getOutEdgeWithTy(Gep)) {
            NodeID dst = edge->getDstID();
            outFile << src << "\t" << dst << "\tf_i" << "\t" << edge->getEdgeIdx() << std::endl;
            outFile << dst << "\t" << src << "\tfbar_i" << "\t" << edge->getEdgeIdx() << std::endl;
        }
    }

    outFile.close();
}

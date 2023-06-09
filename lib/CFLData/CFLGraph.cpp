//
// Created by kisslune on 7/5/22.
//

#include "CFLSolver/CFLBase.h"
#include "Util/SVFUtil.h"
#include "CFLData/CFLGraph.h"
#include <iostream>

using namespace SVF;
using namespace SVFUtil;

std::set<CFLEdge::GEdgeKind> CFLNode::directEdgeKinds;

void CFLGraph::readGraph(std::string fname)
{
    std::ifstream gFile;
    gFile.open(fname, std::ios::in);
    if (!gFile.is_open()) {
        std::cout << "error opening " << fname << std::endl;
        exit(0);
    }

    std::string line;
    while (getline(gFile, line)) {
        std::vector<std::string> vec = split(line, '\t');
        if (vec.empty())
            continue;

        NodeID src = stoi(vec[0]);
        NodeID dst = stoi(vec[1]);

        std::string lblString = vec[2];
        if (!grammar->hasSymbol(lblString))
            continue;

        char lbl = grammar->getSymbolId(vec[2]);
        addNode(src);
        addNode(dst);

        if (vec.size() == 4 && grammar->isaVariantSymbol(lbl))
            addEdge(src, dst, lbl, std::stoi(vec[3]));
        else
            addEdge(src, dst, lbl);
    }

    gFile.close();
}


void CFLGraph::copyBuild(const CFLGraph& rhs)
{
    /// initialize nodes
    for (auto it = rhs.begin(), eit = rhs.end(); it != eit; ++it) {
        if (!it->second->hasIncomingEdge() && !it->second->hasOutgoingEdge())
            continue;
        addNode(it->first);
    }

    /// initialize edges
    for (auto edge: rhs.getCFLEdges()) {
        addEdge(edge->getSrcID(), edge->getDstID(), edge->getEdgeKind(), edge->getEdgeIdx());
    }
}


//@{
bool CFLGraph::addEdge(NodeID srcId, NodeID dstId, CFLEdge::GEdgeKind k, u32_t idx)
{
    CFLNode* src = getNode(srcId);
    CFLNode* dst = getNode(dstId);
    return addEdge(src, dst, k, idx);
}

bool CFLGraph::addEdge(CFLNode* src, CFLNode* dst, CFLEdge::GEdgeKind k, u32_t idx)
{
    if (hasEdge(src, dst, k, idx))
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
void CFLGraph::removeEdge(CFLEdge* edge)
{
    getNode(edge->getSrcID())->removeCFLOutEdge(edge);
    getNode(edge->getDstID())->removeCFLInEdge(edge);
    bool num1 = pegEdgeSet.erase(edge);
    delete edge;

    assert(num1 && "edge not in the set, can not remove!!!");
}


void CFLGraph::writeGraph(std::string name)
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
        auto node = getNode(src);
        for (auto edge: node->getOutEdges()) {
            NodeID dst = edge->getDstID();
            char edgeK = edge->getEdgeKind();
            if (grammar->isaVariantSymbol(edgeK))
                outFile << src << "\t" << dst << "\t" << grammar->getSymbolString(edgeK) << '\t' << edge->getEdgeIdx()
                        << std::endl;
            else
                outFile << src << "\t" << dst << "\t" << grammar->getSymbolString(edgeK) << std::endl;
        }
    }

    outFile.close();
}

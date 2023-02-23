/* -------------------- PEGFold.h ------------------ */
//
// Created by kisslune on 2/22/23.
//

#ifndef POCR_SVF_PEGFOLD_H
#define POCR_SVF_PEGFOLD_H

#include "CFLData/PEG.h"

namespace SVF
{
/*!
 *
 */
class PEGCompact
{
private:
    PEG* peg;
    std::stack<NodePair> compactPairs;

public:
    PEGCompact(PEG* g) : peg(g)
    {}

    void compactGraph();
};
}


#endif //POCR_SVF_PEGFOLD_H

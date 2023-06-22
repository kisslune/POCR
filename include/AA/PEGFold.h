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
 * Graph folding instance for PEGs
 */
class PEGFold
{
private:
    PEG* peg;
    std::stack<NodePair> foldablePairs;

public:
    PEGFold(PEG* g) : peg(g)
    {}

    void foldGraph();
    void mergeDeref();
};
}


#endif //POCR_SVF_PEGFOLD_H

/* -------------------- IVFGFold.h ------------------ */
//
// Created by kisslune on 2/23/23.
//

#ifndef POCR_SVF_IVFGFOLD_H
#define POCR_SVF_IVFGFOLD_H

#include "CFLData/IVFG.h"

namespace SVF
{
/*!
 * Graph folding instance for VFGs
 */
class IVFGCompact
{
private:
    IVFG* lg;
    std::stack<NodePair> compactPairs;

public:
    IVFGCompact(IVFG* g) : lg(g)
    {}

    void compactGraph();
};

}


#endif //POCR_SVF_IVFGFOLD_H

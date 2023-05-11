/* -------------------- ECG.h -- Edge Critical Graph ------------------ */
//
// Created by kisslune on 4/27/23.
//

#ifndef POCR_SVF_ECG_H
#define POCR_SVF_ECG_H

#include "CFLEdge.h"
#include "CFLNode.h"

namespace SVF
{
class ECG : public GenericGraph<CFLNode, CFLEdge>
{
public:
    enum ECGEdgeTy
    {
        Forward = 0,
        Backward        // backward edges for tracking cycles
    };

protected:
    Map<NodeID,NodeID> nodeToRepMap;

public:
    /// constructor
    ECG();      // should be initialized with no edges


};

}

#endif //POCR_SVF_ECG_H

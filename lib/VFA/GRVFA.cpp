//
// Created by kisslune on 4/14/22.
//

#include "VFA/VFAnalysis.h"

using namespace SVF;

/*!
 *
 */
Label GRVFA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

    u32_t lInd = lty.second;
    u32_t rInd = rty.second;

    if (lWord == A && (rWord == a))
        return std::make_pair(A, 0);

    if (lWord == A && (rWord == B))
        return std::make_pair(A, 0);

    if (lWord == call && rWord == A)
        return std::make_pair(Cl, lInd);

    if (lWord == Cl && rWord == ret && lInd == rInd)
        return std::make_pair(B, 0);

    return std::make_pair(fault, 0);
}


/*!
 *
 */
Label GRVFA::unarySumm(Label lty)
{
    char lWord = lty.first;
    if (lWord == a)
        return std::make_pair(A, 0);
    return std::make_pair(fault, 0);
}


/*!
 *
 */
Label GRGspanVFA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

    u32_t lInd = lty.second;
    u32_t rInd = rty.second;

    if (lWord == A && (rWord == a))
        return std::make_pair(A, 0);

    if (lWord == A && (rWord == B))
        return std::make_pair(A, 0);

    if (lWord == call && rWord == A)
        return std::make_pair(Cl, lInd);

    if (lWord == Cl && rWord == ret && lInd == rInd)
        return std::make_pair(B, 0);

    return std::make_pair(fault, 0);
}


/*!
 *
 */
Label GRGspanVFA::unarySumm(Label lty)
{
    char lWord = lty.first;
    if (lWord == a)
        return std::make_pair(A, 0);
    return std::make_pair(fault, 0);
}

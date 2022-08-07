//
// Created by kisslune on 3/14/22.
//

#include "AA/AliasAnalysis.h"

using namespace SVF;

Label GRAA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

    if (lWord == a && rWord == M)
        return std::make_pair(A, 0);
    if (lWord == M && rWord == abar)
        return std::make_pair(Abar, 0);
    if (lWord == Abar && rWord == V)
        return std::make_pair(V, 0);
    if (lWord == V && rWord == A)
        return std::make_pair(V, 0);
    if (lWord == dbar && rWord == V)
        return std::make_pair(DV, 0);
    if (lWord == DV && rWord == d)
        return std::make_pair(M, 0);
    if (lWord == fbar && rWord == V)
        return std::make_pair(FV, lty.second);
    if (lWord == FV && rWord == f && lty.second == rty.second)
        return std::make_pair(V, 0);

    return std::make_pair(fault, 0);
}


Label GRGspanAA::binarySumm(Label lty, Label rty)
{
    char lWord = lty.first;
    char rWord = rty.first;

    if (lWord == a && rWord == M)
        return std::make_pair(A, 0);
    if (lWord == M && rWord == abar)
        return std::make_pair(Abar, 0);
    if (lWord == Abar && rWord == V)
        return std::make_pair(V, 0);
    if (lWord == V && rWord == A)
        return std::make_pair(V, 0);
    if (lWord == dbar && rWord == V)
        return std::make_pair(DV, 0);
    if (lWord == DV && rWord == d)
        return std::make_pair(M, 0);
    if (lWord == fbar && rWord == V)
        return std::make_pair(FV, lty.second);
    if (lWord == FV && rWord == f && lty.second == rty.second)
        return std::make_pair(V, 0);

    return std::make_pair(fault, 0);
}
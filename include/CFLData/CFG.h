//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFG_H
#define POCR_SVF_CFG_H

#include "SVF-LLVM/BasicTypes.h"
#include "BasicTypes.h"

namespace SVF
{
/*!
 * Context-free grammar container
 */
class CFG
{
public:
    LabelSymbTy numOfLabels;       // maximum 128 labels are allowed

    /// mapping string label to int
    Map<std::string, LabelSymbTy> labelToIntMap;
    Map<LabelSymbTy, std::string> intToLabelMap;
    /// the IDs of labels with variant subscript
    Set<LabelSymbTy> variantLabels;

    /// Sets of rules
    Set<LabelSymbTy> emptyRules;                           // X ::= epsilon
    Map<LabelSymbTy, Set<LabelSymbTy>> unaryRules;                     // X ::= Y
    Map<std::pair<LabelSymbTy, LabelSymbTy>, Set<LabelSymbTy>> binaryRules;   // X ::= Y Z
    Set<LabelSymbTy> transitiveLabels;                     // X ::= X X

    const Set<LabelSymbTy> emptySet;

public:
    CFG() : numOfLabels(0)
    {}

    bool hasLabel(std::string& s)
    {
        return labelToIntMap.find(s) != labelToIntMap.end();
    }

    void addLabel(std::string& s);

    LabelSymbTy getLabelId(std::string& s)
    {
        assert(hasLabel(s) && "Attempting to access a non-existing label!!");
        return labelToIntMap[s];
    }

    std::string getLabelString(LabelSymbTy c)
    {
        return intToLabelMap[c];
    }

    bool isaVariantLabel(LabelSymbTy c)
    {
        return variantLabels.find(c) != variantLabels.end();
    }

    const Set<LabelSymbTy>& getLhs(LabelSymbTy rhs) const
    {
        auto it = unaryRules.find(rhs);
        if (it == unaryRules.end())
            return emptySet;
        return it->second;
    }

    const Set<LabelSymbTy>& getLhs(std::pair<LabelSymbTy, LabelSymbTy> rhs) const
    {
        auto it = binaryRules.find(rhs);
        if (it == binaryRules.end())
            return emptySet;
        return it->second;
    }

    Set<LabelSymbTy>& getEmptyRules()
    {
        return emptyRules;
    }

    bool isTransitive(LabelSymbTy c)
    {
        return transitiveLabels.find(c) != transitiveLabels.end();
    }

    void parseGrammar(std::string fname);
    void readGrammarFile(std::string fname);
    void detectTransitiveLabel();
    void printCFGStat();
};

}

#endif //POCR_SVF_CFG_H

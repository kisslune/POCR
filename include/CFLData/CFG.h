//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFG_H
#define POCR_SVF_CFG_H

#include "SVF-LLVM/BasicTypes.h"

namespace SVF
{
/*!
 * Context-free grammar container
 */
class CFG
{
public:
    typedef u32_t LabelIDTy;

public:
    LabelIDTy numOfLabels;       // maximum 128 labels are allowed

    /// mapping string label to int
    Map<std::string, LabelIDTy> labelToIntMap;
    Map<LabelIDTy, std::string> intToLabelMap;
    /// the IDs of labels with variant subscript
    Set<LabelIDTy> variantLabels;

    /// Sets of rules
    Set<LabelIDTy> emptyRules;                           // X ::= epsilon
    Map<LabelIDTy, Set<LabelIDTy>> unaryRules;                     // X ::= Y
    Map<std::pair<LabelIDTy, LabelIDTy>, Set<LabelIDTy>> binaryRules;   // X ::= Y Z
    Set<LabelIDTy> transitiveLabels;                     // X ::= X X

    const Set<LabelIDTy> emptySet;

public:
    CFG() : numOfLabels(0)
    {}

    bool hasLabel(std::string& s)
    {
        return labelToIntMap.find(s) != labelToIntMap.end();
    }

    void addLabel(std::string& s);

    LabelIDTy getLabelId(std::string& s)
    {
        assert(hasLabel(s) && "Attempting to access a non-existing label!!");
        return labelToIntMap[s];
    }

    std::string getLabelString(LabelIDTy c)
    {
        return intToLabelMap[c];
    }

    bool isaVariantLabel(LabelIDTy c)
    {
        return variantLabels.find(c) != variantLabels.end();
    }

    const Set<LabelIDTy>& getLhs(LabelIDTy rhs) const
    {
        auto it = unaryRules.find(rhs);
        if (it == unaryRules.end())
            return emptySet;
        return it->second;
    }

    const Set<LabelIDTy>& getLhs(std::pair<LabelIDTy, LabelIDTy> rhs) const
    {
        auto it = binaryRules.find(rhs);
        if (it == binaryRules.end())
            return emptySet;
        return it->second;
    }

    Set<LabelIDTy>& getEmptyRules()
    {
        return emptyRules;
    }

    bool isTransitive(LabelIDTy c)
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

//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFGRAMMAR_H
#define POCR_SVF_CFGRAMMAR_H

#include "SVF-LLVM/BasicTypes.h"

namespace SVF
{
/*!
 * Context-free grammar container
 */
class CFGrammar
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
    Map<LabelIDTy, LabelIDTy> unaryRules;                     // X ::= Y
    Map<std::pair<LabelIDTy, LabelIDTy>, LabelIDTy> binaryRules;   // X ::= Y Z
    Set<LabelIDTy> transitiveLabels;                     // X ::= X X

public:
    CFGrammar() : numOfLabels(0)
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

    LabelIDTy getLhs(LabelIDTy rhs)
    {
        auto it = unaryRules.find(rhs);
        if (it == unaryRules.end())
            return 0;
        return it->second;
    }

    LabelIDTy getLhs(std::pair<LabelIDTy, LabelIDTy> rhs)
    {
        auto it = binaryRules.find(rhs);
        if (it == binaryRules.end())
            return 0;
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
};

}

#endif //POCR_SVF_CFGRAMMAR_H

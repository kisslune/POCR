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
    char numOfLabels;       // maximum 128 labels are allowed

    /// mapping string label to int
    Map<std::string, char> labelToIntMap;
    Map<char, std::string> intToLabelMap;
    /// the IDs of labels with variant subscript
    Set<char> variantLabels;

    /// Sets of rules
    Set<char> emptyRules;                           // X ::= epsilon
    Map<char, char> unaryRules;                     // X ::= Y
    Map<std::pair<char, char>, char> binaryRules;   // X ::= Y Z
    Set<char> transitiveLabels;                     // X ::= X X

public:
    CFGrammar() : numOfLabels(0)
    {}

    bool hasLabel(std::string& s)
    {
        return labelToIntMap.find(s) != labelToIntMap.end();
    }

    void addLabel(std::string& s);

    char getLabelId(std::string& s)
    {
        assert(hasLabel(s) && "Attempting to access a non-existing label!!");
        return labelToIntMap[s];
    }

    std::string getLabelString(char c)
    {
        return intToLabelMap[c];
    }

    bool isaVariantLabel(char c)
    {
        return variantLabels.find(c) != variantLabels.end();
    }

    char getLhs(char rhs)
    {
        auto it = unaryRules.find(rhs);
        if (it == unaryRules.end())
            return 0;
        return it->second;
    }

    char getLhs(std::pair<char, char> rhs)
    {
        auto it = binaryRules.find(rhs);
        if (it == binaryRules.end())
            return 0;
        return it->second;
    }

    Set<char>& getEmptyRules()
    {
        return emptyRules;
    }

    bool isTransitive(char c)
    {
        return transitiveLabels.find(c) != transitiveLabels.end();
    }

    void parseGrammar(std::string fname);
    void readGrammarFile(std::string fname);
    void detectTransitiveLabel();
};

}

#endif //POCR_SVF_CFGRAMMAR_H

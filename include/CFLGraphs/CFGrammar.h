//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFGRAMMAR_H
#define POCR_SVF_CFGRAMMAR_H

#include "Util/BasicTypes.h"

namespace SVF
{
/*!
 * Context-free grammar container
 */
class CFGrammar
{
public:
    char numOfLabels;       /// maximum 128 labels are allowed

    /// mapping string label to int
    std::map<std::string, char> labelToIntMap;
    std::map<char, std::string> intToLabelMap;
    /// the IDs of labels with variant subscript
    std::set<char> variantLabels;

    /// rules
    std::set<char> emptyRules;                          // X ::= epsilon
    std::map<char, char> unaryRules;                    // X ::= Y
    std::map<std::pair<char, char>, char> binaryRules;  // X ::= Y Z

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

    std::string getLabel(char c)
    {
        return intToLabelMap[c];
    }

    bool isVariantLabel(char c)
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

    std::set<char>& getEmptyRules()
    {
        return emptyRules;
    }

    void readGrammarFile(std::string fname);
};

}

#endif //POCR_SVF_CFGRAMMAR_H

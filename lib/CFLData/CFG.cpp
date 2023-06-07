//
// Created by kisslune on 7/5/22.
//

#include "CFLSolver/CFLBase.h"
#include "CFLData/CFG.h"
#include <iostream>

using namespace SVF;
using namespace SVFUtil;


void CFG::parseGrammar(std::string fname)
{
    readGrammarFile(fname);
    detectTransitiveLabel();
//    printCFGStat();
}


void CFG::readGrammarFile(std::string fname)
{
    std::ifstream gFile;
    gFile.open(fname, std::ios::in);
    if (!gFile.is_open())
    {
        std::cout << "error opening " << fname << std::endl;
        exit(0);
    }

    std::string line;
    while (getline(gFile, line))
    {
        std::vector<std::string> vec = CFLBase::split(line, '\t');

        if (vec.empty())
            continue;

        if (vec.size() == 1)
        {
            addLabel(vec[0]);
            emptyRules.insert(getLabelId(vec[0]));
        }
        else if (vec.size() == 2)
        {
            addLabel(vec[0]);
            addLabel(vec[1]);
            unaryRules[getLabelId(vec[1])].insert(getLabelId(vec[0]));
        }
        else if (vec.size() == 3)
        {
            addLabel(vec[0]);
            addLabel(vec[1]);
            addLabel(vec[2]);
            binaryRules[std::make_pair(getLabelId(vec[1]), getLabelId(vec[2]))].insert(getLabelId(vec[0]));
        }
    }

    gFile.close();
}


void CFG::detectTransitiveLabel()
{
    for (auto& rule: binaryRules)
    {
        for (auto lhs : rule.second)
        if (lhs == rule.first.first && lhs == rule.first.second)
            transitiveLabels.insert(lhs);
    }
}


void CFG::addLabel(std::string& s)
{
    if (hasLabel(s))
        return;

    numOfLabels++;
    labelToIntMap[s] = numOfLabels;
    intToLabelMap[numOfLabels] = s;
    std::cout << "label: " << s  << " num: " << numOfLabels << " entry: " << labelToIntMap[s] << std::endl;


    /// check whether the label has an index
    if (s.find("_i") == s.size() - 2 && s.find("_i") != -1)
        variantLabels.insert(numOfLabels);
}


void CFG::printCFGStat()
{
    u32_t numOfSymbols = 0;
    u32_t numOfRules = 0;
    u32_t numOfVariantSymbols = 0;

    numOfSymbols = intToLabelMap.size();
    numOfVariantSymbols = variantLabels.size();

    numOfRules += emptyRules.size();
    for (auto rule : unaryRules)
        numOfRules += rule.second.size();

    for (auto rule : binaryRules)
        numOfRules += rule.second.size();

    std::cout << "#Symbol = " << numOfSymbols << std::endl;
    std::cout << "#VariantSymbol = " << numOfVariantSymbols << std::endl;
    std::cout << "#Rule = " << numOfRules << std::endl;
}
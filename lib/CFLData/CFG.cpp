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
    detectTransitiveSymbol();
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
        /// Switch line types
        //@{
        if (line == "Production:")
        {
            lineTy = Production;
            continue;
        }
        else if (line == "Insert:")
        {
            lineTy = Insert;
            continue;
        }
        else if (line == "Follow:")
        {
            lineTy = Follow;
            continue;
        }
        else if (line == "Count:")
        {
            lineTy = Count;
            continue;
        }
        //@}

        if (lineTy == Production)
            readProduction(line);
        // TODO


    }

        // TODO: add all to insert, follow, and count if empty ...
    gFile.close();
}


void CFG::readProduction(std::string& line)
{
    std::vector<std::string> vec = split(line, '\t');

    if (vec.size() == 1)
    {
        addSymbol(vec[0]);
        emptyRules.insert(getSymbolId(vec[0]));
    }
    else if (vec.size() == 2)
    {
        addSymbol(vec[0]);
        addSymbol(vec[1]);
        unaryRules[getSymbolId(vec[1])].insert(getSymbolId(vec[0]));
    }
    else if (vec.size() == 3)
    {
        addSymbol(vec[0]);
        addSymbol(vec[1]);
        addSymbol(vec[2]);
        binaryRules[std::make_pair(getSymbolId(vec[1]), getSymbolId(vec[2]))].insert(getSymbolId(vec[0]));
    }
}


void CFG::detectTransitiveSymbol()
{
    for (auto& rule : binaryRules)
    {
        for (auto lhs : rule.second)
            if (lhs == rule.first.first && lhs == rule.first.second)
                transitiveSymbols.insert(lhs);
    }
}


void CFG::addSymbol(std::string& s)
{
    if (hasSymbol(s))
        return;

    numOfSymbols++;
    symbToIntMap[s] = numOfSymbols;
    intToSymbMap[numOfSymbols] = s;

    /// check whether the label has an index
    if (s.find("_i") == s.size() - 2 && s.find("_i") != -1)
        variableSymbols.insert(numOfSymbols);
}


void CFG::printCFGStat()
{
    u32_t numOfSymbols = 0;
    u32_t numOfRules = 0;
    u32_t numOfVariantSymbols = 0;

    numOfSymbols = intToSymbMap.size();
    numOfVariantSymbols = variableSymbols.size();

    numOfRules += emptyRules.size();
    for (auto rule : unaryRules)
        numOfRules += rule.second.size();

    for (auto rule : binaryRules)
        numOfRules += rule.second.size();

    std::cout << "#Symbol = " << numOfSymbols << std::endl;
    for (auto it : intToSymbMap)
        std::cout << it.second << ", ";
    std::cout << std::endl << std::endl;

    std::cout << "#VariantSymbol = " << numOfVariantSymbols << std::endl;
    std::cout << "#Rule = " << numOfRules << std::endl;
}
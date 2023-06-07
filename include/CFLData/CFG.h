//
// Created by kisslune on 7/5/22.
//

#ifndef POCR_SVF_CFG_H
#define POCR_SVF_CFG_H

#include "SVF-LLVM/BasicTypes.h"
#include "BasicUtils.h"

namespace SVF
{
/*!
 * Context-free grammar container
 */
class CFG
{
public:
    enum LineTy
    {
        Production,     // production rule
        Insert,         // insert non-terminals
        Follow,         // follow non-terminals
        Count           // count non-terminals
    };

    LineTy lineTy;      // used to track the type of the current line

    /// number of symbols
    CFGSymbTy numOfSymbols;

    /// mapping string symbol to int
    Map<std::string, CFGSymbTy> symbToIntMap;
    Map<CFGSymbTy, std::string> intToSymbMap;
    /// the IDs of symbols with variant subscript
    Set<CFGSymbTy> variableSymbols;
    Set<CFGSymbTy> transitiveSymbols;                     // X ::= X X
    Set<CFGSymbTy> insertSymbols;
    Set<CFGSymbTy> followSymbols;
    Set<CFGSymbTy> countSymbols;

    /// Sets of rules
    Set<CFGSymbTy> emptyRules;                                          // X ::= epsilon
    Map<CFGSymbTy, Set<CFGSymbTy>> unaryRules;                          // X ::= Y
    Map<std::pair<CFGSymbTy, CFGSymbTy>, Set<CFGSymbTy>> binaryRules;   // X ::= Y Z

    const Set<CFGSymbTy> emptySet;

public:
    CFG() : numOfSymbols(0),
            lineTy(Production)
    {}

    bool hasSymbol(std::string& s)
    { return symbToIntMap.find(s) != symbToIntMap.end(); }

    void addSymbol(std::string& s);

    CFGSymbTy getSymbolId(std::string& s)
    {
        assert(hasSymbol(s) && "Attempting to access a non-existing symbol!!");
        return symbToIntMap[s];
    }

    std::string getSymbolString(CFGSymbTy c)
    { return intToSymbMap[c]; }

    bool isaVariantSymbol(CFGSymbTy c)
    { return variableSymbols.find(c) != variableSymbols.end(); }

    const Set<CFGSymbTy>& getLhs(CFGSymbTy rhs) const
    {
        auto it = unaryRules.find(rhs);
        if (it == unaryRules.end())
            return emptySet;
        return it->second;
    }

    const Set<CFGSymbTy>& getLhs(std::pair<CFGSymbTy, CFGSymbTy> rhs) const
    {
        auto it = binaryRules.find(rhs);
        if (it == binaryRules.end())
            return emptySet;
        return it->second;
    }

    Set<CFGSymbTy>& getEmptyRules()
    { return emptyRules; }

    bool isTransitive(CFGSymbTy c)
    { return transitiveSymbols.find(c) != transitiveSymbols.end(); }

    void parseGrammar(std::string fname);
    void readGrammarFile(std::string fname);
    void readProduction(std::string& line);
    void readUCFLSymbol(std::string& line, LineTy ty);
    void detectTransitiveSymbol();
    void printCFGStat();
};

}

#endif //POCR_SVF_CFG_H

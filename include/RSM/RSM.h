/* -------------------- RSM.h ------------------ */
//
// Created by kisslune on 3/1/23.
//

#ifndef POCR_SVF_RSM_H
#define POCR_SVF_RSM_H

#include "CFLData/CFLData.h"

namespace SVF
{
/*!
 * Data structure and parser of deterministic recursive state machines
 */
class RSM
{
public:
    typedef std::pair<u32_t, u32_t> IdxBoxTy;                   // box with index: <boxId, idx>
    typedef std::pair<std::deque<IdxBoxTy>, u32_t> GStateTy;    // global state: <<boxId, idx>, localStateId>
    typedef std::pair<std::deque<u32_t>, u32_t> BoxedStateTy;   // local states with boxes

    static char symbDelimiter;

protected:
    char numOfLabels;
    u32_t numOfBoxes;
    u32_t numOfStates;
    /// labels
    std::map<std::string, char> labelToIntMap;
    std::map<char, std::string> intToLabelMap;
    /// boxes
    std::map<std::string, u32_t> boxToIntMap;
    std::map<u32_t, std::string> intToBoxMap;
    /// local states, we use 0 to denote a false local state
    std::map<std::string, u32_t> stateToIntMap;
    std::map<u32_t, std::string> intToStateMap;
    /// transition rules
    std::map<BoxedStateTy, std::map<char, BoxedStateTy>> transRules;
    /// initial and accepting states
    GStateTy initState;
    std::set<GStateTy> acptStates;

public:
    /// Constructing an empty RSM
    RSM() : numOfLabels(0),
            numOfBoxes(0),
            numOfStates(0)
    {}

    bool hasState(const std::string& s)
    { return stateToIntMap.find(s) != stateToIntMap.end(); }

    bool hasLabel(const std::string& s)
    { return labelToIntMap.find(s) != labelToIntMap.end(); }

    bool hasBox(const std::string& s)
    { return boxToIntMap.find(s) != boxToIntMap.end(); }

    u32_t getStateID(const std::string& s)
    {
        if (hasState(s))
            return stateToIntMap[s];
        return 0;
    }

    char getLabelID(const std::string& s)
    {
        if (hasLabel(s))
            return labelToIntMap[s];
        return 0;
    }

    u32_t getBoxID(const std::string& s)
    {
        if (hasBox(s))
            return boxToIntMap[s];
        return 0;
    }

    std::string getStateStr(u32_t id)
    {
        auto it = intToStateMap.find(id);
        if (it == intToStateMap.end())
            return "false state!";
        return it->second;
    }

    std::string getLabelStr(char id)
    {
        auto it = intToLabelMap.find(id);
        if (it == intToLabelMap.end())
            return "false label!";
        return it->second;
    }

    std::string getBoxStr(u32_t id)
    {
        auto it = intToBoxMap.find(id);
        if (it == intToBoxMap.end())
            return "false box!";
        return it->second;
    }

    /// Whether a label/box has an index
//    static bool hasIdx(const std::string& s)
//    { return (s.find("_i") == s.size() - 2 && s.find("_i") != -1); }

    void addState(const std::string& s);
    void addLabel(const std::string& s);
    void addBox(const std::string& s);

    GStateTy strToGState(const std::string& s);
    Label strToLabel(const std::string& s);

    void addTransRule(char lbl, BoxedStateTy& src, BoxedStateTy& dst)
    { transRules[src][lbl] = dst; }

    void parseRSM(const std::string& fname);
    void readInitState(const std::string& inStr);
    void readAcptState(const std::string& inStr);

    GStateTy transition(GStateTy& src, Label lbl);
    std::string transition(std::string& src, std::string& lbl);

    void printRSM();

};

void trim(std::string& s, char c = ' ');

}

#endif //POCR_SVF_RSM_H

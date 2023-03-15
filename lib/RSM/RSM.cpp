/* -------------------- RSM.cpp ------------------ */
//
// Created by kisslune on 3/2/23.
//

#include "RSM/RSM.h"
#include <Util/SVFUtil.h>

using namespace SVF;
using namespace std;
using namespace SVFUtil;

char RSM::symbDelimiter = '\t';

void SVF::trim(std::string& s, char c)
{
    int pos = 0;
    if (!s.empty())
        while ((pos = s.find(c, pos)) != std::string::npos)
            s.erase(pos, 1);
}


void RSM::parseRSM(const std::string& fname)
{
    /// Open file
    std::ifstream inFile;
    inFile.open(fname, std::ios::in);
    if (!inFile.is_open())
    {
        std::cout << "error opening " << fname << std::endl;
        exit(0);
    }

    /// Read file
    string line;
    while (getline(inFile, line))
    {
        trim(line);
        vector<string> lineVec = split(line, symbDelimiter);

        if (lineVec.size() >= 3)    // read transition rules
        {
            BoxedStateTy srcState, dstState;

            /// parseRSM src
            vector<string> srcVec = split(lineVec[0], ',');
            if (!srcVec.empty())
            {
                string& localState = srcVec.back();
                addState(localState);    // add local state
                srcState.second = getStateID(localState);
                if (srcVec.size() > 1)
                {
                    string& box = srcVec[srcVec.size() - 2];
                    addBox(box);  // add box
                    srcState.first.push_back(getBoxID(box));
                }
            }

            /// parseRSM label
            addLabel(lineVec[1]);

            /// parseRSM dst
            vector<string> dstVec = split(lineVec[2], ',');
            if (!dstVec.empty())
            {
                string& localState = dstVec.back();
                addState(localState);    // add local state
                dstState.second = getStateID(localState);
                if (dstVec.size() > 1)
                {
                    string& box = dstVec[dstVec.size() - 2];
                    addBox(box);  // add box
                    dstState.first.push_back(getBoxID(box));
                }
            }

            /// add transition rule
            addTransRule(getLabelID(lineVec[1]), srcState, dstState);
        }
        else if (lineVec.size() > 1)   // read initial and accepting states, if specified
        {
            if (lineVec.front() == "init:")     // read initial states
                readInitState(lineVec[1]);
            if (lineVec.front() == "acpt:")     // read accepting states
                readAcptState(lineVec[1]);
        }
    }
}


void RSM::readInitState(const std::string& inStr)
{
    vector<string> inVec = split(inStr, ';');
    if (!inVec.empty())
    {
        GStateTy gState;
        vector<string> sVec = split(inVec.back(), ',');
        gState.second = getStateID(sVec.back());
        sVec.pop_back();
        for (auto& boxStr: sVec)
        {
            vector<string> boxVec = split(boxStr, '_');
            u32_t idx = 0;
            if (boxVec.size() > 1)
                idx = stoi(boxVec.back());
            gState.first.emplace_back(getBoxID(boxVec[0]), idx);
        }
        initState = gState;
    }
}


void RSM::readAcptState(const std::string& inStr)
{
    vector<string> inVec = split(inStr, ';');
    for (auto& sStr: inVec)
    {
        GStateTy gState;
        vector<string> sVec = split(sStr, ',');
        gState.second = getStateID(sVec.back());
        sVec.pop_back();
        for (auto& boxStr: sVec)
        {
            vector<string> boxVec = split(boxStr, '_');
            u32_t idx = 0;
            if (boxVec.size() > 1)
                idx = stoi(boxVec.back());
            gState.first.emplace_back(getBoxID(boxVec[0]), idx);
        }
        acptStates.insert(gState);
    }
}


void RSM::addState(const std::string& s)
{
    if (hasState(s))
        return;

    numOfStates++;
    stateToIntMap[s] = numOfStates;
    intToStateMap[numOfStates] = s;
}


void RSM::addBox(const std::string& s)
{
    if (hasBox(s))
        return;

    numOfBoxes++;
    boxToIntMap[s] = numOfBoxes;
    intToBoxMap[numOfBoxes] = s;
}


void RSM::addLabel(const std::string& s)
{
    if (hasLabel(s))
        return;

    numOfLabels++;
    labelToIntMap[s] = numOfLabels;
    intToLabelMap[numOfLabels] = s;
}


RSM::GStateTy RSM::strToGState(const std::string& s)
{
    GStateTy ret;
    ret.second = 0;
    if (s.empty())
        return ret;

    vector<string> sVec = split(s, ',');
    ret.second = getStateID(sVec.back());
    sVec.pop_back();

    for (auto& boxStr: sVec)
    {
        vector<string> boxVec = split(boxStr, '_');
        IdxBoxTy box;
        box.first = getBoxID(boxVec.front());
        if (boxVec.size() > 1)
            box.second = stoi(boxVec.back());

        ret.first.push_back(box);
    }

    return ret;
}


Label RSM::strToLabel(const std::string& s)
{
    Label ret = Label(0, 0);
    if (s.empty())
        return ret;

    vector<string> sVec = split(s, '_');
    ret.first = getLabelID(sVec.front());
    if (sVec.size() > 1)
        ret.second = stoi(sVec.back());

    return ret;
}


RSM::GStateTy RSM::transition(GStateTy& src, Label lbl)
{
    GStateTy retGS;
    retGS.second = 0;
    if (!src.second || !lbl.first)
        return retGS;

    retGS.first = src.first;
    BoxedStateTy srcLS;

    /// Check innermost local state
    srcLS.second = src.second;
    auto iter = transRules.find(srcLS);
    if (iter != transRules.end() && (iter->second.find(lbl.first)) != iter->second.end())
    {
        auto dstLs = iter->second[lbl.first];
        if (dstLs.first.empty())    // local
            retGS.second = dstLs.second;
        else    // enter, idx depends only on lbl
        {
            retGS.first.emplace_back(dstLs.first.back(), lbl.second);
            retGS.second = dstLs.second;
        }
        return retGS;
    }

    if (src.first.empty())
        return retGS;

    /// Check boxed local state
    srcLS.first.push_back(src.first.back().first);
    retGS.first.pop_back();
    iter = transRules.find(srcLS);
    if (iter != transRules.end() && (iter->second.find(lbl.first)) != iter->second.end())
    {
        auto dstLs = iter->second[lbl.first];
        if (dstLs.first.empty() && src.first.back().second == lbl.second)    // exit, idx depends on both src and lbl
        {
            retGS.second = dstLs.second;
        }
        else if (src.first.back().second == lbl.second)   // exit--enter, idx depends on both src and lbl
        {
            retGS.first.emplace_back(dstLs.first.back(), lbl.second);
            retGS.second = dstLs.second;
        }
    }

    return retGS;
}


string RSM::transition(std::string& src, std::string& lbl)
{
    stringstream ret;

    GStateTy srcGS;
    vector<string> srcVec = split(src, ',');
    if (!srcVec.empty())
    {
        srcGS.second = getStateID(srcVec.back());
        for (u32_t i = 0; i < srcVec.size() - 1; ++i)
        {
            vector<string> boxVec = split(srcVec[i], '_');
            u32_t idx = 0;
            if (boxVec.size() > 1)
                idx = stoi(boxVec[1]);
            srcGS.first.emplace_back(getBoxID(boxVec[0]), idx);
        }
    }

    vector<string> lblVec = split(lbl, '_');
    u32_t idx = 0;
    if (lblVec.size() > 1)
        idx = stoi(lblVec[1]);
    Label tgtlbl = std::make_pair(getLabelID(lblVec[0]), idx);

    GStateTy retGS = transition(srcGS, tgtlbl);

    for (auto box: retGS.first)
    {
        ret << getBoxStr(box.first) << '_' << box.second << ',';
    }
    ret << getStateStr(retGS.second) << endl;

    return ret.str();
}


void RSM::printRSM()
{
    cout << "Labels:" << endl;
    for (auto& it: labelToIntMap)
        cout << it.first << ":" << (u32_t) it.second << "\t\t";
    cout << endl << endl;

    cout << "States:" << endl;
    for (auto& it: stateToIntMap)
        cout << it.first << ":" << it.second << "\t\t";
    cout << endl << endl;

    cout << "Boxes:" << endl;
    for (auto& it: boxToIntMap)
        cout << it.first << ":" << it.second << "\t\t";
    cout << endl << endl;

    cout << "Transition rules:" << endl;
    for (auto& it: transRules)
    {
        string srcBox, dstBox;
        if (!it.first.first.empty())
            srcBox = getBoxStr(it.first.first.back()) + ",";
        for (auto& it1: it.second)
        {
            if (!it1.second.first.empty())
                dstBox = getBoxStr(it1.second.first.back()) + ",";
            else
                dstBox = "";

            cout << srcBox << getStateStr(it.first.second) << '\t'
                 << getLabelStr(it1.first) << "\t"
                 << dstBox << getStateStr(it1.second.second)
                 << endl;
        }
    }
    cout << endl;

    cout << "Initial state:" << endl;
    for (auto box: initState.first)
        cout << getBoxStr(box.first) << '_' << box.second << endl;
    cout << getStateStr(initState.second) << endl;
    cout << endl;

    cout << "Accepting states:" << endl;
    for (auto& state: acptStates)
    {
        for (auto box: state.first)
            cout << getBoxStr(box.first) << '_' << box.second << ',';
        cout << getStateStr(state.second) << endl;
    }
    cout << endl;
}

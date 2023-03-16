/* -------------------- GFPattern.cpp ------------------ */
//
// Created by kisslune on 3/5/23.
//

#include "RSM/GFPattern.h"
#include <Util/SVFUtil.h>

using namespace SVF;
using namespace std;
using namespace SVFUtil;

/*!
 * @param s: input string, should be in an order that \n
 * isxSrc, isySrc, Noty-x; Notx-y; x-y; y-x; x-Noty; y-Notx
 */
void NPPattern::parse(std::string s, RSM* rsm)
{
    trim(s);
    trim(s, '\t');
    vector<string> sVec = split(s, ';');

    _isxSrc = stoi(sVec[0]);
    _isySrc = stoi(sVec[1]);

    u32_t i = XYLABEL_START;
    while (i < PATTERN_WIDTH && i < sVec.size())
    {
        if (sVec[i].empty())
        {
            xyLabels[i - XYLABEL_START].insert(Label(0, 0));  // a false label denoting no such kinds of edges
            i++;
            continue;
        }
        /// parseRSM edge label
        vector<string> lblVec = split(sVec[i], ',');
        for (auto& lblStr: lblVec)
        {
            vector<string> subVec = split(lblStr, '_');
            Label lbl = Label(rsm->getLabelID(subVec[0]), 0);
            if (subVec.size() > 1)
                lbl.second = stoi(subVec.back());
            xyLabels[i - XYLABEL_START].insert(lbl);
        }
        i++;
    }
}


/*!
 * Subsumption relation
 */
bool GFPattern::subsume(GStateTy s1, GStateTy s2, set<Label>& lblSet)
{
    if (!s1.second)     // a false state subsume to any other state
        return true;

    if (acptStates.find(s1) != acptStates.end() && acptStates.find(s2) == acptStates.end())
        return false;

    for (auto& lbl: lblSet)
    {
        if (transition(s1, lbl).second && (transition(s1, lbl) != transition(s2, lbl)))
            return false;
    }
    return true;
}


/*!
 * Get target states of a label
 */
set<RSM::GStateTy> GFPattern::getTgtStatesOfLabels(set<Label>& lblSet)
{
    set<GStateTy> retSet;
    for (auto lbl: lblSet)
    {
        for (auto& ruleMap: transRules)
        {
            if (ruleMap.second.count(lbl.first))    // find valid transition
            {
                /// Get valid src state
                GStateTy src;
                src.second = ruleMap.first.second;
                if (!ruleMap.first.first.empty())
                    src.first.emplace_back(ruleMap.first.first.back(), lbl.second);

                /// Get target state
                GStateTy dst = transition(src, lbl);
                if (dst.second)
                    retSet.insert(transition(src, lbl));
            }
        }
    }
    return retSet;
}


/*!
 * Identify all foldable patterns from a file
 */
void GFPattern::identify(const std::string& fname)
{
    /// Open file
    std::ifstream inFile;
    inFile.open(fname, std::ios::in);
    if (!inFile.is_open())
    {
        std::cout << "error opening " << fname << std::endl;
        exit(0);
    }

    cout << "Input patterns:" << endl;
    string line;
    while (getline(inFile, line))
        cout << line << endl;
    cout << endl;
    inFile.close();

    inFile.open(fname, std::ios::in);
    cout << "Foldable patterns:" << endl;
    /// Read file
    while (getline(inFile, line))
    {
        NPPattern p = NPPattern(line, this);
        if (isFoldable(p))
            cout << line << endl;
    }
    cout << endl;
    inFile.close();
}


/*!
 * Identify whether a node-pair pattern is foldable
 */
bool GFPattern::isFoldable(NPPattern pattern)
{
    if (pattern.getYXLabels().empty() && (pattern.isySrc() || !pattern.getNotXYLabels().empty()))
        return false;

    set<Label> xLabels[3];
    xLabels[0] = pattern.getNotYXLabels();
    xLabels[1] = pattern.getXYLabels();
    xLabels[2] = pattern.getXNotYLabels();

    set<Label> yLabels[3];
    yLabels[0] = pattern.getNotXYLabels();
    yLabels[1] = pattern.getYXLabels();
    yLabels[2] = pattern.getYNotXLabels();

    return check(xLabels, yLabels, pattern.isxSrc()) && check(yLabels, xLabels, pattern.isySrc());
}


/*!
 * @param xLabels [0]: Noty-x, [1]: x-y, [2]: x-Noty
 * @param yLabels [0]: Notx-y, [1]: y-x, [2]: y-Notx
 */
bool GFPattern::check(const std::set<Label>* xLabels, const std::set<Label>* yLabels, bool isxSrc)
{
    auto lblNYX = xLabels[0];
    auto lblXY = xLabels[1];
    auto lblXNY = xLabels[2];
    auto lblNXY = yLabels[0];
    auto lblYX = yLabels[1];
    auto lblYNX = yLabels[2];

    set<Label> lblInX = lblNYX;
    for (auto lbl : lblYX)
        lblInX.insert(lbl);

    set <Label> lblOutX = lblXNY;
    for (auto lbl: lblXY)
        lblOutX.insert(lbl);

    set<GStateTy> NrInx = getTgtStatesOfLabels(lblInX);
    set<GStateTy> Nrnyx = getTgtStatesOfLabels(lblNYX);

    if (isxSrc)
    {
        NrInx.insert(initState);
        Nrnyx.insert(initState);
    }

    /// Construct Qx and Qnyx
    set<GStateTy> Qx;
    for (GStateTy nr: NrInx)
    {
        Qx.insert(nr);
        if (!nr.first.empty())      // nr is reached by entering boxes
            for (auto& boxItem1: intToBoxMap)
                for (auto& boxItem2: intToBoxMap)
                {
                    nr.first.emplace_front(boxItem2.first, 0);  // temporarily set the idx of box as 0
                    nr.first.emplace_front(boxItem1.first, 0);
                    Qx.insert(nr);
                }
    }
    set<GStateTy> Qnyx;
    for (GStateTy nr: Nrnyx)
    {
        Qnyx.insert(nr);
        if (!nr.first.empty())      // nr is reached by entering boxes
            for (auto& boxItem1: intToBoxMap)
                for (auto& boxItem2: intToBoxMap)
                {
                    nr.first.emplace_front(boxItem2.first, 0);  // temporarily set the idx of box as 0
                    nr.first.emplace_front(boxItem1.first, 0);
                    Qnyx.insert(nr);
                }
    }

    for (auto sx: Qx)
        for (auto& xylbl: lblXY)
        {
            bool isInQnyx = Qnyx.count(sx);
            if (!sx.first.empty())
                sx.first.back().second = xylbl.second;    // set the box ID the same as xlbl
            GStateTy sy = transition(sx, xylbl);

            if (isInQnyx)     // rule [x-y], only for Qnyx
                if (!subsume(sx, sy, lblYNX) || !subsume(sy, sx, lblYNX))
                    return false;

            if (sy.second)      // sy is a false state
                continue;

            for (auto& yxlbl: lblYX)
            {
                if (!sy.first.empty() && sy.first.size() <= sx.first.size())     // xlbl does not enter a box
                    sy.first.back().second = yxlbl.second;   // set the box ID the same as ylbl

                GStateTy sx1 = transition(sy, yxlbl);
                if (sx1.second && !subsume(sx1, sx, lblOutX))   // rule [x-x]
                    return false;
            }
        }
    return true;
}
//
// Created by kisslune on 7/5/22.
//

#include "Util/SVFUtil.h"
#include "CFLGraphs/CFGrammar.h"
#include <fstream>
#include <iostream>

using namespace SVF;
using namespace SVFUtil;


void CFGrammar::readGrammarFile(std::string fname)
{
    std::ifstream gFile;
    gFile.open(fname, std::ios::in);
    if (!gFile.is_open()) {
        std::cout << "error opening " << fname << std::endl;
        exit(0);
    }

    std::string line;
    while (getline(gFile, line)) {
        std::vector<std::string> vec = split(line, '\t');
        if (vec.empty())
            continue;

        if (vec.size() == 1) {
            addLabel(vec[0]);
            emptyRules.insert(getLabelId(vec[0]));
        }
        else if (vec.size() == 2) {
            addLabel(vec[0]);
            addLabel(vec[1]);
            unaryRules[getLabelId(vec[1])] = getLabelId(vec[0]);
        }
        else if (vec.size() == 3) {
            addLabel(vec[0]);
            addLabel(vec[1]);
            addLabel(vec[2]);
            binaryRules[std::make_pair(getLabelId(vec[1]), getLabelId(vec[2]))] = getLabelId(vec[0]);
        }
    }

    gFile.close();
}


void CFGrammar::addLabel(std::string& s)
{
    if (hasLabel(s))
        return;

    numOfLabels++;
    labelToIntMap[s] = numOfLabels;
    intToLabelMap[numOfLabels] = s;

    /// check whether the label has an index
    if (s.find("_i") == s.size() - 2 && s.find("_i") != -1)
        variantLabels.insert(numOfLabels);
}

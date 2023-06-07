//
// Created by kisslune on 6/6/23.
//

#include "CFLData/BasicUtils.h"
#include <fstream>
#include <iostream>

namespace SVF
{
void processArgs(int argc, char** argv, int& arg_num, char** arg_vec, std::vector<std::string>& inFileVec)
{
    for (int i = 0; i < argc; ++i)
    {
        std::ifstream f(argv[i]);
        if (i == 0)
        {
            arg_vec[arg_num] = argv[i];
            arg_num++;
        }
        else if (f.good())
        {
            inFileVec.push_back(argv[i]);
        }
        else
        {
            arg_vec[arg_num] = argv[i];
            arg_num++;
        }
    }
}

//std::vector<std::string> split(std::string str, char s)
//{
//    std::vector<std::string> sVec;
//    std::string::iterator it = str.begin();
//    std::string subStr;
//    while (it != str.end())
//    {
//        if (*it == s && !subStr.empty())
//        {
//            sVec.push_back(subStr);
//            subStr.clear();
//        }
//        else
//        {
//            subStr.push_back(*it);
//        }
//        it++;
//    }
//    if (!subStr.empty())
//        sVec.push_back(subStr);
//    return sVec;
//}

/*!
 * Remove spaces from the two ends of the input string
 */
std::string strip(std::string& str)
{
    std::string whiteSpace = " \n\r\t\f\v";

    size_t lPos = str.find_first_not_of(whiteSpace);
    size_t rPos = str.find_first_not_of(whiteSpace);

    if (lPos != std::string::npos && rPos != std::string::npos && rPos >= lPos)
        return str.substr(lPos, rPos + 1);

    return "";
}

}
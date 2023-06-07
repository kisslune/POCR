//
// Created by kisslune on 6/5/23.
//

#ifndef POCR_SVF_CFLBASICUTILS_H
#define POCR_SVF_CFLBASICUTILS_H

#include <SVFIR/SVFType.h>

namespace SVF
{
/// basic types for CFL-reachability
typedef unsigned CFGSymbTy;
typedef std::pair<CFGSymbTy, unsigned> Label;


/// basic methods for CFL-reachability
void processArgs(int argc, char** argv, int& arg_num, char** arg_vec, std::vector<std::string>& inFileVec);
//std::vector<std::string> split(std::string str, char s);
std::string strip(std::string str);

}

#endif //POCR_SVF_CFLBASICUTILS_H

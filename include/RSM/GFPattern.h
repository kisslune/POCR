/* -------------------- GFPattern.h ------------------ */
//
// Created by kisslune on 3/5/23.
//

#ifndef POCR_SVF_GFPATTERN_H
#define POCR_SVF_GFPATTERN_H

#include "RSM.h"

#define PATTERN_WIDTH 8
#define XYLABEL_START 2

namespace SVF
{
/*!
 * Node-pair pattern
 */
class NPPattern
{
private:
    std::set<Label> xyLabels[PATTERN_WIDTH];
    bool _isxSrc, _isySrc;

public:
    NPPattern(u32_t p1, u32_t p2, std::set<Label> p3, std::set<Label> p4, std::set<Label> p5,
              std::set<Label> p6, std::set<Label> p7, std::set<Label> p8)
            : _isxSrc(p1),
              _isySrc(p2)
    {
        xyLabels[0] = std::move(p3);    // labels of in-x edges
        xyLabels[1] = std::move(p4);    // labels of in-y edges
        xyLabels[2] = std::move(p5);    // labels of x-y edges
        xyLabels[3] = std::move(p6);    // labels of y-x edges
        xyLabels[4] = std::move(p7);    // labels of edges starting with x and not ending with y
        xyLabels[5] = std::move(p8);    // labels of edges starting with y and not ending with x
    }

    /// Construct by parsing input string
    NPPattern(const std::string& s, RSM* rsm)
    { parse(s, rsm); }

    void parse(std::string s, RSM* rsm);

    bool isxSrc() const
    { return _isxSrc; }

    bool isySrc() const
    { return _isySrc; }

    /// Get labels
    //@{
    std::set<Label> getNotYXLabels()
    { return xyLabels[0]; }

    std::set<Label> getNotXYLabels()
    { return xyLabels[1]; }

    std::set<Label> getXYLabels()
    { return xyLabels[2]; }

    std::set<Label> getYXLabels()
    { return xyLabels[3]; }

    std::set<Label> getXNotYLabels()
    { return xyLabels[4]; }

    std::set<Label> getYNotXLabels()
    { return xyLabels[5]; }
    //@}

    inline bool operator==(const NPPattern* rhs) const
    {
        return xyLabels == rhs->xyLabels;
    }

};


/*!
 * Identifier for foldable patterns
 */
class GFPattern : public RSM
{
public:
    GFPattern() = default;

    bool subsume(GStateTy s1, GStateTy s2, std::set<Label>& lblSet);  // return true only if s1 is subsumed by s2
    std::set<GStateTy> getTgtStatesOfLabels(std::set<Label>& lblSet);                  // get target states of a label

    bool isFoldable(NPPattern pattern);
    bool check(const std::set<Label>* xLabels, const std::set<Label>* yLabels, bool isxSrc);

    void identify(const std::string& fname);
};

}

#endif //POCR_SVF_GFPATTERN_H

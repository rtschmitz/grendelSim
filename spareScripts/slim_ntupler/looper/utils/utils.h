#include <vector>

const std::vector<std::vector<std::vector<int> > > patts_top = 
    {
        {{-1,-1},
         { 1,-1},
         { 1, 1}},

        {{-1,-1},
         { 1, 0},
         {-1, 1}},

        {{ 1,-1},
         { 0, 0},
         {-1, 1}},

        {{ 1,-1},
         { 1,-1},
         { 1,-1}},

        {{-1, 1},
         {-1, 1},
         {-1, 1}},

        {{-1, 1},
         { 0, 0},
         { 1,-1}},

        {{-1,-1},
         { 0, 1},
         { 1,-1}},

        {{-1,-1},
         {-1, 1},
         { 1, 1}}
    };

const std::vector<std::vector<std::vector<int> > > patts_side = 
    {
        {{ 0,-1,-1},
         { 1, 0,-1},
         { 0, 1, 1}},

        {{ 1, 0,-1},
         { 0, 1, 0},
         {-1, 0, 1}},

        {{ 1,-1,-1},
         { 0, 0,-1},
         {-1, 1,-1}},

        {{ 0, 1,-1},
         {-1, 0, 0},
         {-1,-1, 1}},

        {{ 1,-1,-1},
         { 1,-1,-1},
         { 1,-1,-1}},

        {{-1, 1,-1},
         {-1, 1,-1},
         {-1, 1,-1}},

        {{-1,-1, 1},
         {-1,-1, 1},
         {-1,-1, 1}},

        {{-1, 1, 0},
         { 0, 0,-1},
         { 1,-1,-1}},

        {{-1,-1, 1},
         {-1, 0, 0},
         {-1, 1,-1}},

        {{-1, 0, 1},
         { 0, 1, 0},
         { 1, 0,-1}},

        {{-1,-1, 0},
         {-1, 0, 1},
         { 1, 1, 0}}
    };

std::vector<std::vector<bool> > get_top_hits(int nPE[32], int thresh);
std::vector<std::vector<bool> > get_side_hits(int nPE[32], int thresh);
bool pass_pattern(std::vector<std::vector<int> > patt, std::vector<std::vector<bool> > hits);

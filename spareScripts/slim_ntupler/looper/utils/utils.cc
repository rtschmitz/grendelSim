#include "utils.h"

std::vector<std::vector<bool> > get_top_hits(int nPE[32], int thresh){
    return
        {
            {nPE[2]>thresh || nPE[22]>thresh || nPE[4]>thresh,  nPE[3]>thresh || nPE[23]>thresh || nPE[5]>thresh}, 
            {nPE[6]>thresh || nPE[16]>thresh || nPE[12]>thresh, nPE[7]>thresh || nPE[17]>thresh || nPE[13]>thresh}, 
            {nPE[0]>thresh || nPE[24]>thresh || nPE[8]>thresh,  nPE[1]>thresh || nPE[25]>thresh || nPE[9]>thresh} 
        };
}

std::vector<std::vector<bool> > get_side_hits(int nPE[32], int thresh){
    return
        {
            {nPE[4]>thresh  || nPE[5]>thresh,  nPE[22]>thresh || nPE[23]>thresh, nPE[2]>thresh || nPE[3]>thresh},
            {nPE[12]>thresh || nPE[13]>thresh, nPE[16]>thresh || nPE[17]>thresh, nPE[6]>thresh || nPE[7]>thresh},
            {nPE[8]>thresh  || nPE[9]>thresh,  nPE[24]>thresh || nPE[25]>thresh, nPE[0]>thresh || nPE[1]>thresh}
        };
}

bool pass_pattern(std::vector<std::vector<int> > patt, std::vector<std::vector<bool> > hits){
    for(int i=0; i<patt.size(); i++){
        for(int j=0; j<patt.at(i).size(); j++){
            if(patt.at(i).at(j)==1 && !hits.at(i).at(j))
                return false;
            if(patt.at(i).at(j)==-1 && hits.at(i).at(j))
                return false;
        }
    }
    return true;
}


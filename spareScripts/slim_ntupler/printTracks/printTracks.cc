#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>

#include "mqROOTEvent.hh"
#include "mqPMTRHit.hh"
#include "mqMuonTrack.hh"
#include "mqElectronTrack.hh"
#include "mqGammaTrack.hh"
#include "mqPhotonTrack.hh"
#include "mqNeutronTrack.hh"

#include "../slim_ntupler/utils/TrackFinding.h"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

/* A mapping of geant PMT numbers to milliQan channel numbers
 * Note that the real milliQan channel numbers appear to skip 15 (??)
 * so these go up to 31 while the geant PMT numbers only go up to 30
*/
int channel_map[31] = {
    /* 0 */  8,
    /* 1 */  24,
    /* 2 */  0,
    /* 3 */  9,
    /* 4 */  25,
    /* 5 */  1,
    /* 6 */  12,
    /* 7 */  16,
    /* 8 */  6,
    /* 9 */  13,
    /* 10 */ 17,
    /* 11 */ 7,
    /* 12 */ 4,
    /* 13 */ 22,
    /* 14 */ 2,
    /* 15 */ 5,
    /* 16 */ 23,
    /* 17 */ 3,
    /* 18 */ 18,
    /* 19 */ 20,
    /* 20 */ 28,
    /* 21 */ 21,
    /* 22 */ 27,
    /* 23 */ 10,
    /* 24 */ 29,
    /* 25 */ 30,
    /* 26 */ 11,
    /* 27 */ 19,
    /* 28 */ 31,
    /* 29 */ 14,
    /* 30 */ 26,
};

int main(int argc, char** argv){

    if(argc<4){
        std::cout << "usage: " << argv[0] << " file event_number ch1 [ch2 ch3 ...]\n";
        return 1;
    }

    TFile *f = new TFile(argv[1]);
    mqROOTEvent *revent = new mqROOTEvent();
    TTree *t = (TTree*)f->Get("Events");
    uint nEvents = t->GetEntries();
    // uint nEvents = 100;
    uint evtstart = atoi(argv[2]);
    std::vector<int> doChan;
    for(int i=3; i<argc; i++)
        doChan.push_back(atoi(argv[i]));

    t->SetBranchAddress("ROOTEvent", &revent);
    
    std::vector<track>* tracks = new std::vector<track>;
    
    std::cout << "input file: " << argv[1] << std::endl;
    std::cout << "Event: " << evtstart << std::endl;
    t->GetEntry(evtstart);
    
    populate_tracks(revent, tracks);
    std::cout << "# tracks: " << tracks->size() << std::endl;        
    
    // loop over PMT hits and store relevant information
    mqPMTRHitVector* pes = revent->GetPMTRHits();
    std::map<int, int> unique_parents[32];
    std::pair<float, int> first_parent[32];
    for(int i=0; i<32; i++)
        first_parent[i] = {9999, 0};
    std::map<int, int> npe;
    for(mqPMTRHit *hit : *pes){
        int mqchan = channel_map[hit->GetPMTNumber()];
        // if(std::find(doChan.begin(), doChan.end(), mqchan) == doChan.end())
        //     continue;
        int pid = hit->GetParentID();
        if(unique_parents[mqchan].find(pid) == unique_parents[mqchan].end())
            unique_parents[mqchan][pid] = 0;
        unique_parents[mqchan][pid] += 1;
        float time = hit->GetFirstHitTime();
        if(time < first_parent[mqchan].first)
            first_parent[mqchan] = {time, pid};
        if(npe.find(mqchan) == npe.end())
            npe[mqchan] = 0;
        npe[mqchan]++;
    }

    if(doChan.at(0)<0){
        doChan.clear();
        for(int i=0; i<32; i++){
            if(npe[i] > 0)
                doChan.push_back(i);
        }
    }

    for(int ch : doChan){
        std::cout << "Chan " << ch << std::endl;
        std::cout << "# PE: " << npe[ch] << std::endl;

        for(std::pair<int,int> up : unique_parents[ch]){
            int pid = up.first;
            int count = up.second;
            std::cout << count << " entries";
            if(pid == first_parent[ch].second)
                std::cout << " (FIRST)\n";
            else
                std::cout << "\n";
            std::vector<track> path = get_path(pid, tracks);
            for(int i=0; i<path.size(); i++){
                track& tk = path.at(i);
                track_info ti = get_track_info(tk, revent);
                std::cout << "  " << tk.id << " " << tk.pid << " " << ti.type << " " << ti.start_volume << " " << ti.prod_process << " " << 
                    channel_map[ti.start_copyNo] << " " << tk.init_x << " " << tk.init_y << " " << tk.init_z << " " << tk.init_t << std::endl;
            }
            std::cout << "  -----------------------------\n";
            // std::cin.get();
        }
    }

    f->Close();
    delete f;

    return 0;

}

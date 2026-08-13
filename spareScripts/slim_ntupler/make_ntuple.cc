#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <cmath>

#include "mqROOTEvent.hh"
#include "mqPMTRHit.hh"
#include "mqScintRHit.hh"
#include "mqMuonTrack.hh"
#include "SlimTree/SlimTree.h"
#include "utils/TrackFinding.h"

#include "TRandom.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

bool isscint(std::string volname);

// constants for conversion from sim coordinates
const float rotAngle = 43.1 * M_PI / 180.0;
const float xOffset = -2.5023;
const float zOffset = -2.7381;
const bool APPLY_CALIB = true;

/* A mapping of geant PMT numbers to milliQan channel numbers
 * Note that the real milliQan channel numbers appear to skip 15 (??)
 * so these go up to 31 while the geant PMT numbers only go up to 30
*/
const int channel_map[31] = {
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

// channel time calibrations (manually extracted by finding peaks of uncalibrated time distributions)
const float time_calibs[32] = {9.31, 9.31, 17.55, 17.55, 17.55, 17.55, 13.38, 13.38, 9.31, 9.31, 10.0, 14.0, 13.38, 13.38,
                         18.2, 0.0, 13.38, 13.38, 7.81, 11.2, 11.84, 19.67, 17.55, 17.55, 9.31, 9.31, 15.2, 7.1, 
                         16.03, 7.1, 11.2, 15.2};

const vector<vector<int> > cosmicPaths = 
    {
        {10, 0, 24, 8},
        {10, 1, 25, 9},
        {11, 6, 16, 12},
        {11, 7, 17, 13},
        {14, 2, 22, 4},
        {14, 3, 23, 5},
    };

const vector<vector<float> > pmt_positions = 
    {
        { -0.5410,  -0.0300,  -0.7537}, //   0      35409
        { -0.5411,   0.0298,  -0.7537}, //   1       2593
        {  1.1477,  -0.0331,   1.0488}, //   2         27
        {  1.1470,   0.0305,   1.0495}, //   3        277
        {  1.0590,  -0.0301,   1.1318}, //   4     203669
        {  1.0591,   0.0254,   1.1318}, //   5         10
        {  0.2923,  -0.0302,   0.1374}, //   6     115500
        {  0.2919,   0.0288,   0.1377}, //   7        230
        { -0.6284,  -0.0299,  -0.6720}, //   8     196011
        { -0.6286,   0.0301,  -0.6718}, //   9       4487
        { -0.3879,  -0.0889,  -0.6482}, //  10         33
        {  0.4456,  -0.0889,   0.2425}, //  11         84
        {  0.2050,  -0.0301,   0.2190}, //  12     194188
        {  0.2049,   0.0299,   0.2191}, //  13      61465
        {  1.3001,  -0.0889,   1.1552}, //  14       3168
        {  0.0000,   0.0000,   0.0000}, //  15          0
        {  0.2489,  -0.0294,   0.1780}, //  16        286
        {  0.2500,   0.0295,   0.1769}, //  17         48
        { -1.0417,   0.1016,  -1.4079}, //  18       5112
        { -0.3443,   0.1005,  -0.3372}, //  19         66
        { -0.2150,   0.1016,  -0.5248}, //  20       3137
        {  1.3874,   0.1016,   1.1877}, //  21       3111
        {  1.1036,  -0.0291,   1.0901}, //  22       1309
        {  1.1029,   0.0296,   1.0907}, //  23       2668
        { -0.5849,  -0.0307,  -0.7127}, //  24      44306
        { -0.5849,   0.0300,  -0.7126}, //  25      24056
        {  0.5084,   0.1004,   0.5741}, //  26       1868
        { -1.1787,  -0.1006,  -1.2289}, //  27        212
        {  0.6427,   0.1016,   0.3914}, //  28       2864
        { -1.1820,   0.0996,  -1.2321}, //  29         23
        { -0.3442,  -0.1004,  -0.3370}, //  30       1418
        {  0.5093,  -0.1006,   0.5751}, //  31        300
    };

const std::vector<float> pmt_calibrations = 
    {
        /* 0 -4  */ 0.339, 0.544, 0.391, 0.590, 0.441,
        /* 5 -9  */ 0.637, 0.354, 0.487, 0.301, 0.736,
        /* 10-14 */ 0.250, 0.475, 0.475, 0.394, 0.275,
        /* 15-19 */ 0.999, 0.542, 0.591, 0.285, 0.235,
        /* 20-24 */ 0.360, 0.385, 0.246, 0.340, 0.548,
        /* 25-29 */ 0.884, 0.255, 0.210, 0.180, 0.100,
        /* 30-31 */ 0.315, 0.285
    };

// bar, slab, and panel channel numbers
const int NBARS = 18;
const int NSLABS = 4;
const int NPANELS = 9;
const int bar_chans[NBARS] = {0, 1, 24, 25, 8, 9, 6, 7, 16, 17, 12, 13, 2, 3, 22, 23, 4, 5};
const int slab_chans[NSLABS] = {18, 20, 28, 21};
const int panel_chans[NPANELS] = {27, 10, 29, 30, 11, 19, 31, 14, 26};

int main(int argc, char** argv){

    if(argc < 4){
        std::cout << "usage: " << argv[0] << " <outdir> <file_id> <file1> [<file2> ...]\n";
        return -1;
    }

    gRandom->SetSeed(42);

    char* outdir = argv[1];
    char* file_id = argv[2];
    int file_id_int = atoi(file_id);

    mqROOTEvent *revent = new mqROOTEvent();   
    TFile *fout = new TFile(Form("%s/output_%s.root", outdir, file_id), "RECREATE");
    SlimTree s;
    s.Init();

    std::vector<track>* tracks = new std::vector<track>;
    std::map<std::string, int> origin_counts;

    TChain *ch = new TChain("Events");
    for(int i=3; i<argc; i++)
        ch->Add(argv[i]);
    TIter fileIter(ch->GetListOfFiles());
    TFile *currentFile = 0;
    while ( (currentFile = (TFile*)fileIter.Next()) ) {
        std::cout << "running on file: " << currentFile->GetTitle() << std::endl;
        
        TFile *f = TFile::Open(currentFile->GetTitle());
        if(!f or f->IsZombie()){
            std::cout << "Problem with file, skipping!" << std::endl;
            delete f;
            continue;
        }
        TTree *t = (TTree*)f->Get("Events");
        if(!t){
            std::cout << "Problem with tree in file, skipping!" << std::endl;
            delete f;
            continue;
        }
        uint nEvents = t->GetEntries();
        // uint nEvents = 100;

        t->SetBranchAddress("ROOTEvent", &revent);


        for(int ievt=0 ; ievt<nEvents; ievt++){
            s.progress(ievt, nEvents, 20);

            t->GetEntry(ievt);
            s.Reset();

            // compile a list of tracks so that we can trace track ancestry
            populate_tracks(revent, tracks);
            
            bool ISMU = false;
            mqMuonTrack* mutrack = revent->GetMuonTrack(1);
            if(mutrack){
                ISMU = true;
                s.mu_E0 = mutrack->GetInitialEnergy() / 1000;
                float x0 = mutrack->GetFirstPositionX();
                float y0 = mutrack->GetFirstPositionY();
                float z0 = mutrack->GetFirstPositionZ();
                s.mu_x0 = ((x0-xOffset)/cos(rotAngle) - (z0-zOffset)/sin(rotAngle)) / 2;
                s.mu_y0 = y0;
            }
            mqMCPTrack* mcptrack = revent->GetMCPTrack(1);
            if(mcptrack){
                s.mu_E0 = mcptrack->GetInitialEnergy() / 1000;
                float x0 = mcptrack->GetFirstPositionX();
                float y0 = mcptrack->GetFirstPositionY();
                float z0 = mcptrack->GetFirstPositionZ();
                s.mu_x0 = ((x0-xOffset)/cos(rotAngle) - (z0-zOffset)/sin(rotAngle)) / 2;
                s.mu_y0 = y0;
            }

            // initialize values
            for(int ich=0; ich<32; ich++){
                s.chan_nPE[ich] = 0;
                s.chan_fracMuon[ich] = -1;
                s.chan_fracElScint[ich] = -1;
                s.chan_fracElRock[ich] = -1;
                s.chan_fracElAl[ich] = -1;
                s.chan_fracElPb[ich] = -1;
                s.chan_fracOther[ich] = -1;
                s.chan_firstTime[ich] = 9999999.;
                s.chan_meanTime[ich] = 0.0;
                for(int j=0; j<100; j++)
                    s.chans_PEtimes[ich][j] = 0;
                //channel type: 0=slab, 1=bar, 2=panel
                s.chan_type[ich] = -1;
                for(int j=0; j<max(NBARS, max(NPANELS, NSLABS)); j++){
                    if(j < NSLABS && ich == slab_chans[j]){
                        s.chan_type[ich] = 0;
                        break;
                    }else if(j < NBARS && ich == bar_chans[j]){
                        s.chan_type[ich] = 1;
                        break;
                    }else if(j < NPANELS && ich == panel_chans[j]){
                        s.chan_type[ich] = 2;
                        break;
                    }
                }
                s.chan_timeCalib[ich] = time_calibs[ich];
                s.chan_muDist[ich] = -1;
                s.chan_muTime[ich] = -1;
            }

            s.fileID = file_id_int;
            s.orig_evt = ievt;
            s.scale1fb = revent->GetEventWeight() * (ISMU ? 1000 : 1);
            s.procID = revent->GetProcessID();

            std::map<int,int> unique_parents[32];
            std::pair<float, int> first_parent[32];
            std::pair<int, int> max_parent[32];
            for(int i=0; i<32; i++){
                first_parent[i] = {9999, 0};
                max_parent[i] = {0, 0};
            }

            // loop over PMT hits and store relevant information
            mqPMTRHitVector* pes = revent->GetPMTRHits();
            for(mqPMTRHit *hit : *pes){                
                int mqchan = channel_map[hit->GetPMTNumber()];
                if(APPLY_CALIB && gRandom->Rndm() > pmt_calibrations[mqchan])
                    continue;
                s.chan_nPE[mqchan]++;
                
                //timing information
                float time = hit->GetFirstHitTime() - (time_calibs[mqchan] - 7.0);
                int time_index = std::max(0, std::min(159, int(time/0.625)));
                s.chans_PEtimes[mqchan][time_index] += 1;
                s.chan_firstTime[mqchan] = std::min(s.chan_firstTime[mqchan], time);
                s.chan_meanTime[mqchan] += time;

                // find the unique hit parent IDs for this channel
                int pid = hit->GetParentID();
                if(unique_parents[mqchan].find(pid) == unique_parents[mqchan].end())
                    unique_parents[mqchan][pid] = 0;
                unique_parents[mqchan][pid] += 1;

                if(time < first_parent[mqchan].first)
                    first_parent[mqchan] = {time, pid};
            }

            // trace track ancestry
            for(int ich=0; ich<32; ich++){
                std::map<int,int> source_encodings;
                for(std::pair<int,int> up : unique_parents[ich]){
                    int pid = up.first;
                    int count = up.second;
                    if(count > max_parent[ich].first)
                        max_parent[ich] = {count, pid};
                    std::vector<track> path = get_path(pid, tracks);
                    string source = "UNDEFINED";
                    int enc = -1;
                    if(path.size() > 0){
                        track source_track = path.back();
                        if(path.size() > 1)
                            source_track = path.at(path.size()-2);
                        track_info ti = get_track_info(source_track, revent);
                        if(isscint(ti.start_volume) && ich == channel_map[ti.start_copyNo]){
                            // if muon produces an electron in the same chan, count as muon
                            source_track = path.at(path.size()-1);
                            ti = get_track_info(source_track, revent);
                        }
                        source = ti.type + "_" + ti.start_volume + "_" + ti.prod_process;
                        enc = ti.source_encoding;
                    }
                    if(origin_counts.find(source) == origin_counts.end())
                        origin_counts[source] = 0;
                    origin_counts[source] += count;
                    if(source_encodings.find(enc) == source_encodings.end())
                        source_encodings[enc] = 0;
                    source_encodings[enc] += count; 
                }
                if(s.chan_nPE[ich] > 0){
                    s.chan_fracMuon[ich] = source_encodings[0] / float(s.chan_nPE[ich]);
                    s.chan_fracElScint[ich] = source_encodings[1] / float(s.chan_nPE[ich]);
                    s.chan_fracElRock[ich] = source_encodings[2] / float(s.chan_nPE[ich]);
                    s.chan_fracElAl[ich] = source_encodings[3] / float(s.chan_nPE[ich]);
                    s.chan_fracElPb[ich] = source_encodings[4] / float(s.chan_nPE[ich]);
                    s.chan_fracOther[ich] = source_encodings[-1] / float(s.chan_nPE[ich]);
                }
                // get path length info for first PE
                if(first_parent[ich].second > 0){
                    std::vector<track> path = get_path(first_parent[ich].second, tracks);
                    if(path.size() > 0){
                        std::vector<track> justmu = {path.back()};
                        float path_length = get_path_distance(path, pmt_positions.at(ich));
                        float nom_length = 0.0;
                        if(path_length > 0)
                            nom_length = get_path_distance(justmu, pmt_positions.at(ich));
                        s.chan_firstPEPathLength[ich] = path_length - nom_length;
                    }
                }
                // get source info for max PE
                if(max_parent[ich].second > 0){
                    std::vector<track> path = get_path(max_parent[ich].second, tracks);
                    if(path.size() > 0){
                        int ip;
                        for(ip=0; ; ip++){
                            track_info ti = get_track_info(path.at(ip), revent);
                            if(!isscint(ti.start_volume) || ich != channel_map[ti.start_copyNo])
                                break;                        
                        }
                        s.chan_maxSourceType[ich] = path.at(ip).type;
                        s.chan_maxSourceE0[ich] = path.at(ip).init_E;
                        s.chan_maxSourceNPE[ich] = max_parent[ich].first;
                    }
                }
            }
            
            // normalize the mean, and set default values for 0-hit events
            for(int ich=0; ich<32; ich++){
                if(s.chan_nPE[ich] == 0){
                    s.chan_firstTime[ich] = -1;
                    s.chan_meanTime[ich] = -1;
                }else{
                    s.chan_meanTime[ich] /= s.chan_nPE[ich];
                }
            }

            // handle scintHits
            mqScintRHitVector* scinthits = revent->GetScintRHits();
            for(mqScintRHit *hit : *scinthits){
                int ch = channel_map[hit->GetCopyNo()];
                s.chan_muDist[ch] = hit->GetTrackLength();
                s.chan_muTime[ch] = hit->GetHitTime();
            }

            // set the mcTruth booleans
            s.mcTruth_nMuBarHits = 0;
            s.mcTruth_nMuSlabHits = 0;
            s.mcTruth_nMuPanelHits = 0;
            s.mcTruth_noChan = true;
            for(int ich=0; ich<32; ich++){
                float muDist = s.chan_muDist[ich];
                bool ishit = (muDist > 0);
                s.mcTruth_noChan &= !ishit;
                if(s.chan_type[ich] == 0)
                    s.mcTruth_nMuSlabHits += int(ishit);
                else if(s.chan_type[ich] == 1)
                    s.mcTruth_nMuBarHits += int(ishit);
                else if(s.chan_type[ich] == 2)
                    s.mcTruth_nMuPanelHits += int(ishit);
                }                    
            s.mcTruth_threeBarLine = (
                                      (s.chan_muDist[0]>0  && s.chan_muDist[6]>0  && s.chan_muDist[2]>0)  ||
                                      (s.chan_muDist[1]>0  && s.chan_muDist[7]>0  && s.chan_muDist[3]>0)  ||
                                      (s.chan_muDist[24]>0 && s.chan_muDist[16]>0 && s.chan_muDist[22]>0) ||
                                      (s.chan_muDist[25]>0 && s.chan_muDist[17]>0 && s.chan_muDist[23]>0) ||
                                      (s.chan_muDist[8]>0  && s.chan_muDist[12]>0 && s.chan_muDist[4]>0)  ||
                                      (s.chan_muDist[9]>0  && s.chan_muDist[13]>0 && s.chan_muDist[5]>0)
                                      );
            // redundant, but keep for consistency reasons
            s.mcTruth_fourSlab = (s.mcTruth_nMuSlabHits == 4);
            s.mcTruth_noBar = (s.mcTruth_nMuBarHits == 0);
            // check cosmic paths
            s.mcTruth_verticalCosmic = 0;
            for(int ipath=0; ipath<cosmicPaths.size(); ipath++){
                bool pass = true;
                for(int ch : cosmicPaths.at(ipath))
                    if(s.chan_muDist[ch] <= 0)
                        pass = false;
                if(pass){
                    s.mcTruth_verticalCosmic = ipath+1;
                    break;
                }
            }

            s.Fill();
        }

        f->Close();
        delete f;
        delete currentFile;
    }

    // print PE origin counts
    std::cout << "Sources of PEs\n-------------------------------------------\n";
    std::vector<std::pair<std::string, int> > vec;
    std::copy(origin_counts.begin(), origin_counts.end(), std::back_inserter<std::vector<std::pair<std::string, int> > >(vec));
    std::sort(vec.begin(), vec.end(), 
              [](const std::pair<std::string, int> l, const std::pair<std::string, int> r) {
                  return l.second>r.second;
              });
    for(std::pair<std::string, int> oc : vec){
        std::cout << oc.second << "\t" << oc.first << std::endl;
    }
        
    s.Write(fout);
    fout->Close();

    return 0;

}

bool isscint(std::string volname){
    return volname.find("plScin") != std::string::npos ||
        volname.find("slab_physic") != std::string::npos ||
        volname.find("panel_physic") != std::string::npos;
}

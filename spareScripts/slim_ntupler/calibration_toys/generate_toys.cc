#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <cmath>

#include "mqROOTEvent.hh"
#include "mqPMTRHit.hh"
#include "mqScintRHit.hh"
#include "mqMuonTrack.hh"
#include "../SlimTree/SlimTree.h"

#include "TRandom3.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"

int bar_layer(int ch);
bool is_linepath(int ch1, int ch2, int ch3);

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

// channel time calibrations
const float time_calibs[32] = {9.31, 9.31, 17.55, 17.55, 17.55, 17.55, 13.38, 13.38, 9.31, 9.31, 10.0, 14.0, 13.38, 13.38,
                         18.2, 0.0, 13.38, 13.38, 7.81, 11.2, 11.84, 19.67, 17.55, 17.55, 9.31, 9.31, 15.2, 7.1, 
                         16.03, 7.1, 11.2, 15.2};

const std::vector<std::vector<int> > line_paths = 
    {
        {0,  6,  2},
        {1,  7,  3},
        {24, 16, 22},
        {25, 17, 23},
        {8,  12, 4},
        {9,  13, 5}
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

// 0=bar878, 1=bar7725, 2=barET, 3=slab, 4=panel
const std::vector<float> pmt_types = 
    {
        /* 0 -4  */ 0, 0, 0, 0, 0,
        /* 5 -9  */ 1, 0, 0, 0, 2,
        /* 10-14 */ 4, 4, 0, 0, 4, 
        /* 15-19 */ 0, 0, 2, 3, 4,
        /* 20-24 */ 3, 3, 1, 0, 2,
        /* 25-29 */ 2, 4, 4, 3, 4,
        /* 30-31 */ 4, 4
    };

const std::vector<float> uncorr_unc =
    {
        /* 0 -4  */ 0.18, 0.18, 0.17, 0.22, 0.10, 
        /* 5 -9  */ 0.15, 0.18, 0.18, 0.16, 0.11,
        /* 10-14 */ 0.10, 0.10, 0.18, 0.26, 0.10,
        /* 15-19 */ 0.10, 0.18, 0.10, 0.08, 0.10,
        /* 20-24 */ 0.08, 0.08, 0.16, 0.16, 0.12,
        /* 25-29 */ 0.13, 0.10, 0.10, 0.08, 0.10,
        /* 30-31 */ 0.10, 0.10
    };

const std::vector<float> type_corr_unc =
    {
        /* 0 -4  */ 0.06, 0.09, 0.09, 0.09, 0.07, 
        /* 5 -9  */ 0.05, 0.06, 0.06, 0.05, 0.01,
        /* 10-14 */ 0.10, 0.10, 0.12, 0.04, 0.10,
        /* 15-19 */ 0.10, 0.10, 0.01, 0.08, 0.10,
        /* 20-24 */ 0.08, 0.08, 0.05, 0.13, 0.01,
        /* 25-29 */ 0.01, 0.10, 0.10, 0.08, 0.10,
        /* 30-31 */ 0.10, 0.10
    };

// width of the gaussian of recoNPE is X*sqrt(genNPE). These are the measured X for each channel
// (used  when genNPE > 10)
const std::vector<float> recogen_npe_width = 
    {
        /* 0 -4  */ 0.428, 0.432, 0.421, 0.440, 0.442, 
        /* 5 -9  */ 0.440, 0.435, 0.429, 0.439, 0.440,
        /* 10-14 */ 0.432, 0.428, 0.431, 0.431, 0.454,
        /* 15-19 */ 0.000, 0.435, 0.422, 0.440, 0.449, 
        /* 20-24 */ 0.442, 0.445, 0.440, 0.452, 0.430, 
        /* 25-29 */ 0.442, 0.471, 0.436, 0.445, 0.447, 
        /* 30-31 */ 0.442, 0.456,        
    };

const std::vector<float> npe_corrections = 
    {
        1.3888888888888888, // 0-4
        0.9090909090909091,
        1.1627906976744187,
        0.7936507936507936,
        1.0416666666666667,
        1.0638297872340425, // 5-9
        1.3157894736842104,
        0.9433962264150942,
        1.5151515151515151,
        0.8771929824561403,
        0.9523809523809523, // 10-14
        0.5,
        0.9615384615384616,
        1.2195121951219512,
        0.8333333333333333,
        20.0,               // 15-19
        0.8064516129032258,
        1.0638297872340425,
        1.3333333333333335,
        0.4444444444444445,
        1.0256410256410255, // 20-24
        0.975609756097561,
        2.631578947368421,
        1.3157894736842104,
        1.1363636363636365,
        0.7142857142857143, // 25-29
        0.4,
        0.4444444444444445,
        2.1052631578947367,
        0.8,
        0.3076923076923077, // 30-31
        0.4
    };

const float bar_corr_unc = 0.05;

// bar, slab, and panel channel numbers
const std::vector<int> BAR_CHANS = {0, 1, 24, 25, 8, 9, 6, 7, 16, 17, 12, 13, 2, 3, 22, 23, 4, 5};
const std::vector<int> SLAB_CHANS = {18, 20, 28, 21};
const std::vector<int> PANEL_CHANS = {27, 10, 29, 30, 11, 19, 31, 14, 26};

int main(int argc, char** argv){

    if(argc < 4){
        std::cout << "usage: " << argv[0] << " <outdir> <file_id> <file1> [<file2> ...]\n";
        return -1;
    }

    char* outdir = argv[1];
    char* file_id = argv[2];
    int file_id_int = atoi(file_id);

    std::vector<TH1D*> recogen_templates;
    TFile *ft = new TFile("recogen_templates.root");
    for(int i=0; i<32; i++){
        recogen_templates.push_back((TH1D*)ft->Get(Form("recogen_ch%d", i)));
        recogen_templates.back()->SetDirectory(0);
    }
    ft->Close();
    TH1D* h_lownpe = new TH1D("h_lownpe","", 40,0,20);
    if(h_lownpe->GetBinWidth(1) != recogen_templates.at(0)->GetYaxis()->GetBinWidth(1)){
        std::cout << "ERROR: expecting bin width of recogen templates to be 0.5!";
        return 1;
    }

    mqROOTEvent *revent = new mqROOTEvent();   
    TFile *fout = new TFile(Form("%s/output_%s.root", outdir, file_id), "RECREATE");

    float sr_yields[5];
    float pmt_varcalib[32];
    TTree *tout = new TTree("Events","Events");
    tout->Branch("sr_yields", sr_yields, "sr_yields[5]/F");
    tout->Branch("pmt_calib", pmt_varcalib, "pmt_calib[32]/F");

    TChain *ch = new TChain("Events");
    for(int i=3; i<argc; i++)
        ch->Add(argv[i]);
    TIter fileIter(ch->GetListOfFiles());
    TFile *currentFile = 0;

    TRandom3 rand_calib; // for determining calibrations. want same seed for each job
    rand_calib.SetSeed(42);
    TRandom3 rand_npe; // for determing if npe pass, want different seed for each job
    rand_npe.SetSeed(0);
    gRandom->SetSeed(0);
    for(int icalib=0; icalib<1000; icalib++){

        std::vector<float> type_vars;
        for(int i=0; i<5; i++)
            type_vars.push_back(rand_calib.Gaus(0.0, 1.0));
        float bar_var = rand_calib.Gaus(1.0, bar_corr_unc);
        for(int ich=0; ich<32; ich++){
            float nomcalib = pmt_calibrations.at(ich);
            // correlated within a given type (878, 7725, ET, slab, panel)
            float type_var = 1.0 + type_vars.at(pmt_types.at(ich)) * type_corr_unc.at(ich);
            // completely uncorrelated between channels
            float uncorr_var = rand_calib.Gaus(1.0, uncorr_unc.at(ich));
            // totally correlated uncertainty in bars only
            float corr_var = (pmt_types.at(ich) <= 2) ? bar_var : 1.0;
            pmt_varcalib[ich] = pmt_calibrations.at(ich) * type_var * uncorr_var * corr_var;
            pmt_varcalib[ich] = std::min(1.0f, pmt_varcalib[ich]);
            pmt_varcalib[ich] = std::max(0.0f, pmt_varcalib[ich]);
        }

        fileIter.Reset();
        while ( (currentFile = (TFile*)fileIter.Next()) ) {
            std::cout << "iter " << icalib << ", running on file: " << currentFile->GetTitle() << std::endl;
        
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

            for(int i=0; i<5; i++)
                sr_yields[i] = 0.0;

            for(int ievt=0 ; ievt<nEvents; ievt++){

                t->GetEntry(ievt);
            
                float scale1fb = revent->GetEventWeight();

                float chan_nPE[32] = {0};
                float chan_firstTime[32] = {99999.0};

                // loop over PMT hits and store relevant information
                mqPMTRHitVector* pes = revent->GetPMTRHits();
                for(mqPMTRHit *hit : *pes){
                    int mqchan = channel_map[hit->GetPMTNumber()];
                    if(rand_npe.Rndm() > pmt_varcalib[mqchan])
                        continue;
                    
                    chan_nPE[mqchan] += 1;
                
                    //timing information
                    float time = hit->GetFirstHitTime() - (time_calibs[mqchan] - 7.0);
                    chan_firstTime[mqchan] = std::min(chan_firstTime[mqchan], time);
                }

                // convert from genNPE to recoNPE
                
                for(int ch=0; ch<32; ch++){
                    if(chan_nPE[ch] == 0)
                        continue;
                    if(chan_nPE[ch] > 10){
                        chan_nPE[ch] = rand_npe.Gaus(chan_nPE[ch], recogen_npe_width.at(ch)*sqrt(chan_nPE[ch]));
                    }else{
                        for(int i=1; i<=40; i++){
                            h_lownpe->SetBinContent(i, recogen_templates.at(ch)->GetBinContent(chan_nPE[ch]+1, i));
                        }
                        chan_nPE[ch] = h_lownpe->GetRandom();
                    }

                    // correct for non-uniform NPE response
                    chan_nPE[ch] *= npe_corrections.at(ch);

                }
            
                std::vector<std::vector<int> > layer_pulses = {{}, {}, {}};
                for(int ch : BAR_CHANS){
                    if(chan_nPE[ch] > 0.5)
                        layer_pulses[bar_layer(ch)-1].push_back(ch);
                }
                int npanelhits = 0;
                for(int ch : PANEL_CHANS){
                    if(chan_nPE[ch] > 0.5)
                        npanelhits++;
                }
                int nslabhits = 0;
                bool slabVeto = false;
                for(int ch : SLAB_CHANS){
                    if(chan_nPE[ch] > 250){
                        slabVeto = true;
                        break;
                    }else if(chan_nPE[ch] >= 0.5){
                        nslabhits++;
                    }
                }

                if(slabVeto)
                    continue;

                // veto any panel hits
                if(npanelhits != 0)
                    continue;

                // check that there is exactly 1 bar pulse per layer
                if(layer_pulses.at(0).size() != 1 || layer_pulses.at(1).size() != 1 || layer_pulses.at(2).size() != 1)
                    continue;

                int ch1 = layer_pulses.at(0).at(0), \
                    ch2 = layer_pulses.at(1).at(0), \
                    ch3 = layer_pulses.at(2).at(0);

                if(!is_linepath(ch1, ch2, ch3))
                    continue;

                float npe1 = chan_nPE[ch1], npe2 = chan_nPE[ch2], npe3 = chan_nPE[ch3];
                // if(npe1<0.5 || npe2<0.5 || npe3<0.5)
                //     continue;
                float t1 = chan_firstTime[ch1], t2 = chan_firstTime[ch2], t3 = chan_firstTime[ch3];
                float min_npe = std::min(std::min(npe1, npe2), npe3);
                float max_npe = std::max(std::max(npe1, npe2), npe3);
                if(min_npe==0)
                    std::cout << ch1 << " " << ch2 << " " << ch3 << " " << chan_nPE[ch1] << " " << chan_nPE[ch2] << " " << chan_nPE[ch3] << std::endl;
                if(max_npe/min_npe > 10)
                    continue;
                float max_dt = std::max(std::max(t1,t2),t3) - std::min(std::min(t1,t2),t3);
                if(max_dt > 15.0)
                    continue;


                if(nslabhits==0 && min_npe >= 2.0 && min_npe < 20)
                    sr_yields[0] += scale1fb;
                if(nslabhits==0 && min_npe >= 20)
                    sr_yields[1] += scale1fb;
                if(nslabhits==1 && min_npe >= 5.0 && min_npe < 30)
                    sr_yields[2] += scale1fb;
                if(nslabhits==1 && min_npe >= 30)
                    sr_yields[3] += scale1fb;
                if(nslabhits>=2 && min_npe >= 0.5)
                    sr_yields[4] += scale1fb;

            } // event loop

            f->Close();
            delete f;
            // delete currentFile;
        } // file loop

        tout->Fill();

    } // calib loop
     
    fout->cd();
    tout->Write();
    fout->Close();

    return 0;

}

int bar_layer(int ch){
    if(ch==0 || ch==1 || ch==24 || ch==25 || ch==8 || ch==9)
        return 1;
    if(ch==6 || ch==7 || ch==16 || ch==17 || ch==12 || ch==13)
        return 2;
    if(ch==2 || ch==3 || ch==22 || ch==23 || ch==4 || ch==5)
        return 3;
    return -1;
}

bool is_linepath(int ch1, int ch2, int ch3){
    for(const std::vector<int>& path : line_paths){
        if(ch1==path.at(0) && ch2==path.at(1) && ch3==path.at(2))
            return true;
    }
    return false;
}

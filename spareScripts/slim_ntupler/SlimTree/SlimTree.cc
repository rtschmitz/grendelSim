
#include "SlimTree.h"
#include "TTree.h"
#include "TDirectory.h"
#include <iostream>
#include <vector>

using namespace std;

SlimTree::SlimTree(TTree *t){
    if(t != NULL)
        Init(t);
}

void SlimTree::Init(TTree *t){

    if(t == NULL){
        // if we don't pass a tree, open in "write" mode. vector pointers have to be initialized,
        // and then create the branches.
        this->t = new TTree("Events","");


        b_fileID                 = this->t->Branch("fileID", &fileID, "fileID/I");
        b_orig_evt               = this->t->Branch("orig_evt", &orig_evt, "orig_evt/I");
        b_scale1fb               = this->t->Branch("scale1fb", &scale1fb, "scale1fb/F");
        b_procID                 = this->t->Branch("procID", &procID, "procID/I");
        b_mu_E0                  = this->t->Branch("mu_E0", &mu_E0, "mu_E0/F");
        b_mu_x0                  = this->t->Branch("mu_x0", &mu_x0, "mu_x0/F");
        b_mu_y0                  = this->t->Branch("mu_y0", &mu_y0, "mu_y0/F");
        b_chan_nPE               = this->t->Branch("chan_nPE", chan_nPE, "chan_nPE[32]/I");
        b_chan_fracMuon          = this->t->Branch("chan_fracMuon", chan_fracMuon, "chan_fracMuon[32]/F");
        b_chan_fracElScint       = this->t->Branch("chan_fracElScint", chan_fracElScint, "chan_fracElScint[32]/F");
        b_chan_fracElAl          = this->t->Branch("chan_fracElAl", chan_fracElAl, "chan_fracElAl[32]/F");
        b_chan_fracElRock        = this->t->Branch("chan_fracElRock", chan_fracElRock, "chan_fracElRock[32]/F");
        b_chan_fracElPb          = this->t->Branch("chan_fracElPb", chan_fracElPb, "chan_fracElPb[32]/F");
        b_chan_fracOther         = this->t->Branch("chan_fracOther", chan_fracOther, "chan_fracOther[32]/F");
        b_chan_firstTime         = this->t->Branch("chan_firstTime", chan_firstTime, "chan_firstTime[32]/F");
        b_chan_meanTime          = this->t->Branch("chan_meanTime", chan_meanTime, "chan_meanTime[32]/F");
        b_chan_timeCalib         = this->t->Branch("chan_timeCalib", chan_timeCalib, "chan_timeCalib[32]/F");
        b_chan_muDist            = this->t->Branch("chan_muDist", chan_muDist, "chan_muDist[32]/F");
        b_chan_muTime            = this->t->Branch("chan_muTime", chan_muTime, "chan_muTime[32]/F");
        b_chan_firstPEPathLength = this->t->Branch("chan_firstPEPathLength", chan_firstPEPathLength, "chan_firstPEPathLength[32]/F");
        b_chan_maxSourceType     = this->t->Branch("chan_maxSourceType", chan_maxSourceType, "chan_maxSourceType[32]/I");
        b_chan_maxSourceE0       = this->t->Branch("chan_maxSourceE0", chan_maxSourceE0, "chan_maxSourceE0[32]/F");
        b_chan_maxSourceNPE      = this->t->Branch("chan_maxSourceNPE", chan_maxSourceNPE, "chan_maxSourceNPE[32]/I");
        b_chan_type              = this->t->Branch("chan_type", chan_type, "chan_type[32]/I");
        b_mcTruth_nMuSlabHits    = this->t->Branch("mcTruth_nMuSlabHits", &mcTruth_nMuSlabHits, "mcTruth_nMuSlabHits/I");
        b_mcTruth_nMuPanelHits   = this->t->Branch("mcTruth_nMuPanelHits", &mcTruth_nMuPanelHits, "mcTruth_nMuPanelHits/I");
        b_mcTruth_nMuBarHits     = this->t->Branch("mcTruth_nMuBarHits", &mcTruth_nMuBarHits, "mcTruth_nMuBarHits/I");
        b_mcTruth_fourSlab       = this->t->Branch("mcTruth_fourSlab", &mcTruth_fourSlab, "mcTruth_fourSlab/O");
        b_mcTruth_threeBarLine   = this->t->Branch("mcTruth_threeBarLine", &mcTruth_threeBarLine, "mcTruth_threeBarLine/O");
        b_mcTruth_noBar          = this->t->Branch("mcTruth_noBar", &mcTruth_noBar, "mcTruth_noBar/O");
        b_mcTruth_noChan         = this->t->Branch("mcTruth_noChan", &mcTruth_noChan, "mcTruth_noChan/O");
        b_mcTruth_verticalCosmic = this->t->Branch("mcTruth_verticalCosmic", &mcTruth_verticalCosmic, "mcTruth_verticalCosmic/I");
        b_chan0_PEtimes          = this->t->Branch("chan0_PEtimes", chan0_PEtimes, "chan0_PEtimes[160]/F");
        b_chan1_PEtimes          = this->t->Branch("chan1_PEtimes", chan1_PEtimes, "chan1_PEtimes[160]/F");
        b_chan2_PEtimes          = this->t->Branch("chan2_PEtimes", chan2_PEtimes, "chan2_PEtimes[160]/F");
        b_chan3_PEtimes          = this->t->Branch("chan3_PEtimes", chan3_PEtimes, "chan3_PEtimes[160]/F");
        b_chan4_PEtimes          = this->t->Branch("chan4_PEtimes", chan4_PEtimes, "chan4_PEtimes[160]/F");
        b_chan5_PEtimes          = this->t->Branch("chan5_PEtimes", chan5_PEtimes, "chan5_PEtimes[160]/F");
        b_chan6_PEtimes          = this->t->Branch("chan6_PEtimes", chan6_PEtimes, "chan6_PEtimes[160]/F");
        b_chan7_PEtimes          = this->t->Branch("chan7_PEtimes", chan7_PEtimes, "chan7_PEtimes[160]/F");
        b_chan8_PEtimes          = this->t->Branch("chan8_PEtimes", chan8_PEtimes, "chan8_PEtimes[160]/F");
        b_chan9_PEtimes          = this->t->Branch("chan9_PEtimes", chan9_PEtimes, "chan9_PEtimes[160]/F");
        b_chan10_PEtimes         = this->t->Branch("chan10_PEtimes", chan10_PEtimes, "chan10_PEtimes[160]/F");
        b_chan11_PEtimes         = this->t->Branch("chan11_PEtimes", chan11_PEtimes, "chan11_PEtimes[160]/F");
        b_chan12_PEtimes         = this->t->Branch("chan12_PEtimes", chan12_PEtimes, "chan12_PEtimes[160]/F");
        b_chan13_PEtimes         = this->t->Branch("chan13_PEtimes", chan13_PEtimes, "chan13_PEtimes[160]/F");
        b_chan14_PEtimes         = this->t->Branch("chan14_PEtimes", chan14_PEtimes, "chan14_PEtimes[160]/F");
        b_chan15_PEtimes         = this->t->Branch("chan15_PEtimes", chan15_PEtimes, "chan15_PEtimes[160]/F");
        b_chan16_PEtimes         = this->t->Branch("chan16_PEtimes", chan16_PEtimes, "chan16_PEtimes[160]/F");
        b_chan17_PEtimes         = this->t->Branch("chan17_PEtimes", chan17_PEtimes, "chan17_PEtimes[160]/F");
        b_chan18_PEtimes         = this->t->Branch("chan18_PEtimes", chan18_PEtimes, "chan18_PEtimes[160]/F");
        b_chan19_PEtimes         = this->t->Branch("chan19_PEtimes", chan19_PEtimes, "chan19_PEtimes[160]/F");
        b_chan20_PEtimes         = this->t->Branch("chan20_PEtimes", chan20_PEtimes, "chan20_PEtimes[160]/F");
        b_chan21_PEtimes         = this->t->Branch("chan21_PEtimes", chan21_PEtimes, "chan21_PEtimes[160]/F");
        b_chan22_PEtimes         = this->t->Branch("chan22_PEtimes", chan22_PEtimes, "chan22_PEtimes[160]/F");
        b_chan23_PEtimes         = this->t->Branch("chan23_PEtimes", chan23_PEtimes, "chan23_PEtimes[160]/F");
        b_chan24_PEtimes         = this->t->Branch("chan24_PEtimes", chan24_PEtimes, "chan24_PEtimes[160]/F");
        b_chan25_PEtimes         = this->t->Branch("chan25_PEtimes", chan25_PEtimes, "chan25_PEtimes[160]/F");
        b_chan26_PEtimes         = this->t->Branch("chan26_PEtimes", chan26_PEtimes, "chan26_PEtimes[160]/F");
        b_chan27_PEtimes         = this->t->Branch("chan27_PEtimes", chan27_PEtimes, "chan27_PEtimes[160]/F");
        b_chan28_PEtimes         = this->t->Branch("chan28_PEtimes", chan28_PEtimes, "chan28_PEtimes[160]/F");
        b_chan29_PEtimes         = this->t->Branch("chan29_PEtimes", chan29_PEtimes, "chan29_PEtimes[160]/F");
        b_chan30_PEtimes         = this->t->Branch("chan30_PEtimes", chan30_PEtimes, "chan30_PEtimes[160]/F");
        b_chan31_PEtimes         = this->t->Branch("chan31_PEtimes", chan31_PEtimes, "chan31_PEtimes[160]/F");

        Reset();
    }else{
        // if we do pass a tree, open in "read" mode
        this->t = t;
        //this->t->SetMakeClass(1);

        this->t->SetBranchAddress("fileID", &fileID, &b_fileID);
        this->t->SetBranchAddress("orig_evt", &orig_evt, &b_orig_evt);
        this->t->SetBranchAddress("scale1fb", &scale1fb, &b_scale1fb);
        this->t->SetBranchAddress("procID", &procID, &b_procID);
        this->t->SetBranchAddress("mu_E0", &mu_E0, &b_mu_E0);
        this->t->SetBranchAddress("mu_x0", &mu_x0, &b_mu_x0);
        this->t->SetBranchAddress("mu_y0", &mu_y0, &b_mu_y0);
        this->t->SetBranchAddress("chan_nPE", chan_nPE, &b_chan_nPE);
        this->t->SetBranchAddress("chan_fracMuon", chan_fracMuon, &b_chan_fracMuon);
        this->t->SetBranchAddress("chan_fracElScint", chan_fracElScint, &b_chan_fracElScint);
        this->t->SetBranchAddress("chan_fracElAl", chan_fracElAl, &b_chan_fracElAl);
        this->t->SetBranchAddress("chan_fracElRock", chan_fracElRock, &b_chan_fracElRock);
        this->t->SetBranchAddress("chan_fracElPb", chan_fracElPb, &b_chan_fracElPb);
        this->t->SetBranchAddress("chan_fracOther", chan_fracOther, &b_chan_fracOther);
        this->t->SetBranchAddress("chan_firstTime", chan_firstTime, &b_chan_firstTime);
        this->t->SetBranchAddress("chan_meanTime", chan_meanTime, &b_chan_meanTime);
        this->t->SetBranchAddress("chan_timeCalib", chan_timeCalib, &b_chan_timeCalib);
        this->t->SetBranchAddress("chan_muDist", chan_muDist, &b_chan_muDist);
        this->t->SetBranchAddress("chan_muTime", chan_muTime, &b_chan_muTime);
        this->t->SetBranchAddress("chan_firstPEPathLength", chan_firstPEPathLength, &b_chan_firstPEPathLength);
        this->t->SetBranchAddress("chan_maxSourceType", chan_maxSourceType, &b_chan_maxSourceType);
        this->t->SetBranchAddress("chan_maxSourceE0", chan_maxSourceE0, &b_chan_maxSourceE0);
        this->t->SetBranchAddress("chan_maxSourceNPE", chan_maxSourceNPE, &b_chan_maxSourceNPE);
        this->t->SetBranchAddress("chan_type", chan_type, &b_chan_type);
        this->t->SetBranchAddress("mcTruth_nMuSlabHits", &mcTruth_nMuSlabHits, &b_mcTruth_nMuSlabHits);
        this->t->SetBranchAddress("mcTruth_nMuPanelHits", &mcTruth_nMuPanelHits, &b_mcTruth_nMuPanelHits);
        this->t->SetBranchAddress("mcTruth_nMuBarHits", &mcTruth_nMuBarHits, &b_mcTruth_nMuBarHits);
        this->t->SetBranchAddress("mcTruth_fourSlab", &mcTruth_fourSlab, &b_mcTruth_fourSlab);
        this->t->SetBranchAddress("mcTruth_threeBarLine", &mcTruth_threeBarLine, &b_mcTruth_threeBarLine);
        this->t->SetBranchAddress("mcTruth_noBar", &mcTruth_noBar, &b_mcTruth_noBar);
        this->t->SetBranchAddress("mcTruth_noChan", &mcTruth_noChan, &b_mcTruth_noChan);
        this->t->SetBranchAddress("mcTruth_verticalCosmic", &mcTruth_verticalCosmic, &b_mcTruth_verticalCosmic);
        this->t->SetBranchAddress("chan0_PEtimes", chan0_PEtimes, &b_chan0_PEtimes);
        this->t->SetBranchAddress("chan1_PEtimes", chan1_PEtimes, &b_chan1_PEtimes);
        this->t->SetBranchAddress("chan2_PEtimes", chan2_PEtimes, &b_chan2_PEtimes);
        this->t->SetBranchAddress("chan3_PEtimes", chan3_PEtimes, &b_chan3_PEtimes);
        this->t->SetBranchAddress("chan4_PEtimes", chan4_PEtimes, &b_chan4_PEtimes);
        this->t->SetBranchAddress("chan5_PEtimes", chan5_PEtimes, &b_chan5_PEtimes);
        this->t->SetBranchAddress("chan6_PEtimes", chan6_PEtimes, &b_chan6_PEtimes);
        this->t->SetBranchAddress("chan7_PEtimes", chan7_PEtimes, &b_chan7_PEtimes);
        this->t->SetBranchAddress("chan8_PEtimes", chan8_PEtimes, &b_chan8_PEtimes);
        this->t->SetBranchAddress("chan9_PEtimes", chan9_PEtimes, &b_chan9_PEtimes);
        this->t->SetBranchAddress("chan10_PEtimes", chan10_PEtimes, &b_chan10_PEtimes);
        this->t->SetBranchAddress("chan11_PEtimes", chan11_PEtimes, &b_chan11_PEtimes);
        this->t->SetBranchAddress("chan12_PEtimes", chan12_PEtimes, &b_chan12_PEtimes);
        this->t->SetBranchAddress("chan13_PEtimes", chan13_PEtimes, &b_chan13_PEtimes);
        this->t->SetBranchAddress("chan14_PEtimes", chan14_PEtimes, &b_chan14_PEtimes);
        this->t->SetBranchAddress("chan15_PEtimes", chan15_PEtimes, &b_chan15_PEtimes);
        this->t->SetBranchAddress("chan16_PEtimes", chan16_PEtimes, &b_chan16_PEtimes);
        this->t->SetBranchAddress("chan17_PEtimes", chan17_PEtimes, &b_chan17_PEtimes);
        this->t->SetBranchAddress("chan18_PEtimes", chan18_PEtimes, &b_chan18_PEtimes);
        this->t->SetBranchAddress("chan19_PEtimes", chan19_PEtimes, &b_chan19_PEtimes);
        this->t->SetBranchAddress("chan20_PEtimes", chan20_PEtimes, &b_chan20_PEtimes);
        this->t->SetBranchAddress("chan21_PEtimes", chan21_PEtimes, &b_chan21_PEtimes);
        this->t->SetBranchAddress("chan22_PEtimes", chan22_PEtimes, &b_chan22_PEtimes);
        this->t->SetBranchAddress("chan23_PEtimes", chan23_PEtimes, &b_chan23_PEtimes);
        this->t->SetBranchAddress("chan24_PEtimes", chan24_PEtimes, &b_chan24_PEtimes);
        this->t->SetBranchAddress("chan25_PEtimes", chan25_PEtimes, &b_chan25_PEtimes);
        this->t->SetBranchAddress("chan26_PEtimes", chan26_PEtimes, &b_chan26_PEtimes);
        this->t->SetBranchAddress("chan27_PEtimes", chan27_PEtimes, &b_chan27_PEtimes);
        this->t->SetBranchAddress("chan28_PEtimes", chan28_PEtimes, &b_chan28_PEtimes);
        this->t->SetBranchAddress("chan29_PEtimes", chan29_PEtimes, &b_chan29_PEtimes);
        this->t->SetBranchAddress("chan30_PEtimes", chan30_PEtimes, &b_chan30_PEtimes);
        this->t->SetBranchAddress("chan31_PEtimes", chan31_PEtimes, &b_chan31_PEtimes);

    }

    this->chans_PEtimes[0] = this->chan0_PEtimes;
    this->chans_PEtimes[1] = this->chan1_PEtimes;
    this->chans_PEtimes[2] = this->chan2_PEtimes;
    this->chans_PEtimes[3] = this->chan3_PEtimes;
    this->chans_PEtimes[4] = this->chan4_PEtimes;
    this->chans_PEtimes[5] = this->chan5_PEtimes;
    this->chans_PEtimes[6] = this->chan6_PEtimes;
    this->chans_PEtimes[7] = this->chan7_PEtimes;
    this->chans_PEtimes[8] = this->chan8_PEtimes;
    this->chans_PEtimes[9] = this->chan9_PEtimes;
    this->chans_PEtimes[10] = this->chan10_PEtimes;
    this->chans_PEtimes[11] = this->chan11_PEtimes;
    this->chans_PEtimes[12] = this->chan12_PEtimes;
    this->chans_PEtimes[13] = this->chan13_PEtimes;
    this->chans_PEtimes[14] = this->chan14_PEtimes;
    this->chans_PEtimes[15] = this->chan15_PEtimes;
    this->chans_PEtimes[16] = this->chan16_PEtimes;
    this->chans_PEtimes[17] = this->chan17_PEtimes;
    this->chans_PEtimes[18] = this->chan18_PEtimes;
    this->chans_PEtimes[19] = this->chan19_PEtimes;
    this->chans_PEtimes[20] = this->chan20_PEtimes;
    this->chans_PEtimes[21] = this->chan21_PEtimes;
    this->chans_PEtimes[22] = this->chan22_PEtimes;
    this->chans_PEtimes[23] = this->chan23_PEtimes;
    this->chans_PEtimes[24] = this->chan24_PEtimes;
    this->chans_PEtimes[25] = this->chan25_PEtimes;
    this->chans_PEtimes[26] = this->chan26_PEtimes;
    this->chans_PEtimes[27] = this->chan27_PEtimes;
    this->chans_PEtimes[28] = this->chan28_PEtimes;
    this->chans_PEtimes[29] = this->chan29_PEtimes;
    this->chans_PEtimes[30] = this->chan30_PEtimes;
    this->chans_PEtimes[31] = this->chan31_PEtimes;

    t_old = std::chrono::system_clock::now();
}

void SlimTree::Fill(){
    t->Fill();
}

void SlimTree::Reset(){
    fileID = -1;
    orig_evt = -1;
    scale1fb = 1.0;
    procID = 0;
    mu_E0 = -1;
    mu_x0 = -999;
    mu_y0 = -999;
    mcTruth_nMuSlabHits = -1;
    mcTruth_nMuPanelHits = -1;
    mcTruth_nMuBarHits = -1;
    mcTruth_fourSlab = false;
    mcTruth_threeBarLine = false;
    mcTruth_noBar = false;
    mcTruth_noChan = false;
    mcTruth_verticalCosmic = 0;
    for(int i=0; i<32; i++){
        chan_nPE[i] = 0;
        chan_fracMuon[i] = -1;
        chan_fracElScint[i] = -1;
        chan_fracElAl[i] = -1;
        chan_fracElRock[i] = -1;
        chan_fracElPb[i] = -1;
        chan_fracOther[i] = -1;
        chan_firstTime[i] = -999;
        chan_meanTime[i] = -999;
        chan_timeCalib[i] = -999;
        chan_muDist[i] = -1;
        chan_muTime[i] = -1;
        chan_firstPEPathLength[i] = -1;
        chan_maxSourceType[i] = -1;
        chan_maxSourceE0[i] = -1;
        chan_maxSourceNPE[i] = -1;
        chan_type[i] = -1;
    }
    for(int i=0; i<160; i++){
        chan0_PEtimes[i] = 0;
        chan1_PEtimes[i] = 0;
        chan2_PEtimes[i] = 0;
        chan3_PEtimes[i] = 0;
        chan4_PEtimes[i] = 0;
        chan5_PEtimes[i] = 0;
        chan6_PEtimes[i] = 0;
        chan7_PEtimes[i] = 0;
        chan8_PEtimes[i] = 0;
        chan9_PEtimes[i] = 0;
        chan10_PEtimes[i] = 0;
        chan11_PEtimes[i] = 0;
        chan12_PEtimes[i] = 0;
        chan13_PEtimes[i] = 0;
        chan14_PEtimes[i] = 0;
        chan15_PEtimes[i] = 0;
        chan16_PEtimes[i] = 0;
        chan17_PEtimes[i] = 0;
        chan18_PEtimes[i] = 0;
        chan19_PEtimes[i] = 0;
        chan20_PEtimes[i] = 0;
        chan21_PEtimes[i] = 0;
        chan22_PEtimes[i] = 0;
        chan23_PEtimes[i] = 0;
        chan24_PEtimes[i] = 0;
        chan25_PEtimes[i] = 0;
        chan26_PEtimes[i] = 0;
        chan27_PEtimes[i] = 0;
        chan28_PEtimes[i] = 0;
        chan29_PEtimes[i] = 0;
        chan30_PEtimes[i] = 0;
        chan31_PEtimes[i] = 0;
    }

}

void SlimTree::Write(TDirectory *d){
    d->cd();
    t->Write();
}

void SlimTree::GetEntry(ULong64_t i){
    this->t->GetEntry(i);
}

void SlimTree::progress( int curr, int tot, int period, unsigned int smoothing) {
    if(curr%period == 0) {
        auto now = std::chrono::system_clock::now();
        double dt = ((std::chrono::duration<double>)(now - t_old)).count();
        t_old = now;
        // if (deq.size() >= smoothing) deq.pop_front();                                                                                                                                                            
        if (deq.size() >= smoothing) deq.erase(deq.begin());
        deq.push_back(dt);
        double avgdt = std::accumulate(deq.begin(),deq.end(),0.)/deq.size();
        float prate = (float)period/avgdt;
        float peta = (tot-curr)/prate;
        if (isatty(1)) {
            float pct = (float)curr/(tot*0.01);
            if( ( tot - curr ) <= period ) pct = 100.0;
            printf("\015\033[32m ---> \033[1m\033[31m%4.1f%% \033[34m [%.3f kHz, ETA: %.0f s] \033[0m\033[32m  <---\033[0m\015 ", pct, prate/1000.0, peta);
            if( ( tot - curr ) > period ) fflush(stdout);
            else cout << endl;
        }
    }
}

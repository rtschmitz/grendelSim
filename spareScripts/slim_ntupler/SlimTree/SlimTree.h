
#ifndef SlimTree_h
#define SlimTree_h

#include <vector>
#include <chrono>
#include <numeric>

#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TDirectory.h"


using namespace std;

class SlimTree {
  public:

    int    fileID;
    int    orig_evt;
    float  scale1fb;
    int    procID;
    float  mu_E0;
    float  mu_x0;
    float  mu_y0;
    int    chan_nPE[32];
    float  chan_fracMuon[32];
    float  chan_fracElScint[32];
    float  chan_fracElAl[32];
    float  chan_fracElRock[32];
    float  chan_fracElPb[32];
    float  chan_fracOther[32];
    float  chan_firstTime[32];
    float  chan_meanTime[32];
    float  chan_timeCalib[32];
    float  chan_muDist[32];
    float  chan_muTime[32];
    float  chan_firstPEPathLength[32];
    int    chan_maxSourceType[32];
    float  chan_maxSourceE0[32];
    int    chan_maxSourceNPE[32];
    int    chan_type[32];
    int    mcTruth_nMuSlabHits;
    int    mcTruth_nMuPanelHits;
    int    mcTruth_nMuBarHits;
    bool   mcTruth_fourSlab;
    bool   mcTruth_threeBarLine;
    bool   mcTruth_noBar;
    bool   mcTruth_noChan;
    int    mcTruth_verticalCosmic;
    float  chan0_PEtimes[160];
    float  chan1_PEtimes[160];
    float  chan2_PEtimes[160];
    float  chan3_PEtimes[160];
    float  chan4_PEtimes[160];
    float  chan5_PEtimes[160];
    float  chan6_PEtimes[160];
    float  chan7_PEtimes[160];
    float  chan8_PEtimes[160];
    float  chan9_PEtimes[160];
    float  chan10_PEtimes[160];
    float  chan11_PEtimes[160];
    float  chan12_PEtimes[160];
    float  chan13_PEtimes[160];
    float  chan14_PEtimes[160];
    float  chan15_PEtimes[160];
    float  chan16_PEtimes[160];
    float  chan17_PEtimes[160];
    float  chan18_PEtimes[160];
    float  chan19_PEtimes[160];
    float  chan20_PEtimes[160];
    float  chan21_PEtimes[160];
    float  chan22_PEtimes[160];
    float  chan23_PEtimes[160];
    float  chan24_PEtimes[160];
    float  chan25_PEtimes[160];
    float  chan26_PEtimes[160];
    float  chan27_PEtimes[160];
    float  chan28_PEtimes[160];
    float  chan29_PEtimes[160];
    float  chan30_PEtimes[160];
    float  chan31_PEtimes[160];

    float* chans_PEtimes[32];

    SlimTree(TTree *t=0);
    void Init(TTree *t=0);
    void Fill();
    void Reset();
    void Write(TDirectory *d);
    void GetEntry(ULong64_t entry);
    TTree * tree(){ return t; }
    void progress(int nEventsTotal, int nEventsChain, int period=1000, uint smoothing=30);

  private:
    TTree *t;
    std::chrono::time_point<std::chrono::system_clock> t_old;
    std::vector<double> deq;

    TBranch *b_fileID = 0;
    TBranch *b_orig_evt = 0;
    TBranch *b_scale1fb = 0;
    TBranch *b_procID = 0;
    TBranch *b_mu_E0 = 0;
    TBranch *b_mu_x0 = 0;
    TBranch *b_mu_y0 = 0;
    TBranch *b_chan_nPE = 0;
    TBranch *b_chan_fracMuon = 0;
    TBranch *b_chan_fracElScint = 0;
    TBranch *b_chan_fracElAl = 0;
    TBranch *b_chan_fracElRock = 0;
    TBranch *b_chan_fracElPb = 0;
    TBranch *b_chan_fracOther = 0;
    TBranch *b_chan_firstTime = 0;
    TBranch *b_chan_meanTime = 0;
    TBranch *b_chan_timeCalib = 0;
    TBranch *b_chan_muDist = 0;
    TBranch *b_chan_muTime = 0;
    TBranch *b_chan_firstPEPathLength = 0;
    TBranch *b_chan_maxSourceType = 0;
    TBranch *b_chan_maxSourceE0 = 0;
    TBranch *b_chan_maxSourceNPE = 0;
    TBranch *b_chan_type = 0;
    TBranch *b_mcTruth_nMuSlabHits = 0;
    TBranch *b_mcTruth_nMuPanelHits = 0;
    TBranch *b_mcTruth_nMuBarHits = 0;
    TBranch *b_mcTruth_fourSlab = 0;
    TBranch *b_mcTruth_threeBarLine = 0;
    TBranch *b_mcTruth_noBar = 0;
    TBranch *b_mcTruth_noChan = 0;
    TBranch *b_mcTruth_verticalCosmic = 0;
    TBranch *b_chan0_PEtimes = 0;
    TBranch *b_chan1_PEtimes = 0;
    TBranch *b_chan2_PEtimes = 0;
    TBranch *b_chan3_PEtimes = 0;
    TBranch *b_chan4_PEtimes = 0;
    TBranch *b_chan5_PEtimes = 0;
    TBranch *b_chan6_PEtimes = 0;
    TBranch *b_chan7_PEtimes = 0;
    TBranch *b_chan8_PEtimes = 0;
    TBranch *b_chan9_PEtimes = 0;
    TBranch *b_chan10_PEtimes = 0;
    TBranch *b_chan11_PEtimes = 0;
    TBranch *b_chan12_PEtimes = 0;
    TBranch *b_chan13_PEtimes = 0;
    TBranch *b_chan14_PEtimes = 0;
    TBranch *b_chan15_PEtimes = 0;
    TBranch *b_chan16_PEtimes = 0;
    TBranch *b_chan17_PEtimes = 0;
    TBranch *b_chan18_PEtimes = 0;
    TBranch *b_chan19_PEtimes = 0;
    TBranch *b_chan20_PEtimes = 0;
    TBranch *b_chan21_PEtimes = 0;
    TBranch *b_chan22_PEtimes = 0;
    TBranch *b_chan23_PEtimes = 0;
    TBranch *b_chan24_PEtimes = 0;
    TBranch *b_chan25_PEtimes = 0;
    TBranch *b_chan26_PEtimes = 0;
    TBranch *b_chan27_PEtimes = 0;
    TBranch *b_chan28_PEtimes = 0;
    TBranch *b_chan29_PEtimes = 0;
    TBranch *b_chan30_PEtimes = 0;
    TBranch *b_chan31_PEtimes = 0;

};

#endif

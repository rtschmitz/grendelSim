#include <algorithm>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TRandom.h"

#include "looper.h"
#include "../SlimTree/SlimTree.h"
#include "utils/utils.h"

const float BAR_THRESH = 750;
const float SLAB_THRESH = 150;
const float PANEL_THRESH = 15;
const int NCHAN = 32;
const int NBARS = 18;
const int NSLABS = 4;
const int NPANELS = 9;
const vector<int> BAR_CHANS = {0,1,24,25,8,9,6,7,16,17,12,13,2,3,22,23,4,5};
const vector<int> SLAB_CHANS = {18,20,28,21};
const vector<int> PANEL_CHANS = {27,10,29,30,11,19,31,14,26};

const float lumi = 35.0;

const std::vector<std::vector<std::vector<int> > > chan_neighbs = 
    {
        /*        {non-neighbs, neighbs} */
        /* 0  */  {{8,9}  , {1,24}},
        /* 1  */  {{8,9}  , {0,25}},
        /* 24 */  {{}     , {0,8,25}},
        /* 25 */  {{}     , {1,9,24}},
        /* 8  */  {{0,1}  , {9,24}},
        /* 9  */  {{0,1}  , {8,25}},
        /* 6  */  {{12,13}, {7,16}},
        /* 7  */  {{12,13}, {6,17}},
        /* 16 */  {{}     , {6,12,17}},
        /* 17 */  {{}     , {7,13,16}},
        /* 12 */  {{6,7}  , {13,16}},
        /* 13 */  {{6,7}  , {12,17}},
        /* 2  */  {{4,5}  , {3,22}},
        /* 3  */  {{4,5}  , {2,23}},
        /* 22 */  {{}     , {2,4,23}},
        /* 23 */  {{}     , {3,5,22}},
        /* 4  */  {{2,3}  , {5,22}},
        /* 5  */  {{2,3}  , {4,23}}

        // /*        {non-neighbs, neighbs} */
        // /* 0  */  {{8,9}  , {1,24,25}},
        // /* 1  */  {{8,9}  , {0,25,24}},
        // /* 24 */  {{}     , {0,8,25}},
        // /* 25 */  {{}     , {1,9,24}},
        // /* 8  */  {{0,1}  , {9,24,25}},
        // /* 9  */  {{0,1}  , {8,25,24}},
        // /* 6  */  {{12,13}, {7,16,17}},
        // /* 7  */  {{12,13}, {6,17,16}},
        // /* 16 */  {{}     , {6,12,17}},
        // /* 17 */  {{}     , {7,13,16}},
        // /* 12 */  {{6,7}  , {13,16,17}},
        // /* 13 */  {{6,7}  , {12,17,16}},
        // /* 2  */  {{4,5}  , {3,22,23}},
        // /* 3  */  {{4,5}  , {2,23,22}},
        // /* 22 */  {{}     , {2,4,23}},
        // /* 23 */  {{}     , {3,5,22}},
        // /* 4  */  {{2,3}  , {5,22,23}},
        // /* 5  */  {{2,3}  , {4,23,22}}
    };

const std::vector<std::vector<std::vector<int> > > cosmic_paths = 
    {
        /* {{path}, {neighbors}} */
        {{10,0,24,8} , {1,25,9}},
        {{10,1,25,9} , {0,24,8}},
        {{11,6,16,12}, {7,17,13}},
        {{11,7,17,13}, {6,16,12}},
        {{14,2,22,4} , {3,23,5}},
        {{14,3,23,5} , {2,22,4}}
    };

const std::vector<std::vector<std::vector<int> > > side_cosmic_paths = 
    {
        /* {{panels}, {bars1}, {bars2}} */
        {{27,29} , {0,24,8}, {1,25,9}},
        {{30,19} , {6,16,12}, {7,17,13}},
        {{31,26} , {2,22,4}, {3,23,5}},
    };

const std::vector<std::vector<int> > line_paths = 
    {
        {0,  6,  2},
        {1,  7,  3},
        {24, 16, 22},
        {25, 17, 23},
        {8,  12, 4},
        {9,  13, 5}
    };

const std::vector<std::vector<int> > same_digi_nonpointing = 
    {
        {0, 3, 6}, {0, 4, 6}, {0, 5, 6}, {0, 2, 7}, {0, 3, 7}, {0, 4, 7}, {0, 5, 7}, {0, 2, 12}, {0, 3, 12}, 
        {0, 4, 12}, {0, 5, 12}, {0, 2, 13}, {0, 3, 13}, {0, 4, 13}, {0, 5, 13}, {1, 2, 6}, {1, 3, 6}, {1, 4, 6}, 
        {1, 5, 6}, {1, 2, 7}, {1, 4, 7}, {1, 5, 7}, {1, 2, 12}, {1, 3, 12}, {1, 4, 12}, {1, 5, 12}, {1, 2, 13}, 
        {1, 3, 13}, {1, 4, 13}, {1, 5, 13}, {16, 23, 24}, {17, 22, 24}, {17, 23, 24}, {16, 22, 25}, {16, 23, 25}, 
        {17, 22, 25}, {2, 6, 8}, {3, 6, 8}, {4, 6, 8}, {5, 6, 8}, {2, 7, 8}, {3, 7, 8}, {4, 7, 8}, {5, 7, 8}, 
        {2, 8, 12}, {3, 8, 12}, {5, 8, 12}, {2, 8, 13}, {3, 8, 13}, {4, 8, 13}, {5, 8, 13}, {2, 6, 9}, {3, 6, 9}, 
        {4, 6, 9}, {5, 6, 9}, {2, 7, 9}, {3, 7, 9}, {4, 7, 9}, {5, 7, 9}, {2, 9, 12}, {3, 9, 12}, {4, 9, 12}, 
        {5, 9, 12}, {2, 9, 13}, {3, 9, 13}, {4, 9, 13}
    };

const std::vector<float> pmt_calibrations = 
    {
        /* 0 -4  */ 0.489, 0.356, 0.391, 0.525, 0.358,
        /* 5 -9  */ 0.746, 0.289, 0.524, 0.301, 0.827,
        /* 10-14 */ 0.250, 0.475, 0.384, 0.327, 0.275,
        /* 15-19 */ 0.999, 0.760, 0.629, 0.285, 0.235,
        /* 20-24 */ 0.360, 0.385, 0.272, 0.304, 0.922,
        /* 25-29 */ 0.795, 0.255, 0.210, 0.180, 0.100,
        /* 30-31 */ 0.315, 0.285
    };

int main(int argc, char** argv) {

    TChain *c = new TChain("Events");

    // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/v5_v6_save2m_testcondor_GEANT_v2/*.root");
    // TFile *fout = new TFile("test_beam.root","RECREATE");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/v5_v6_save2m_pencilBelow_1m_GEANT_v1/*.root");
    // // TFile *fout = new TFile("test_beam_pencilBelow_1m.root","RECREATE");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam2_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam3_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam4_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam5_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam6_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam7_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam8_v1/merged/merged.root");
    // // c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam9_v1/merged/merged.root");
    // // c->Add(("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/custombeam"+std::string(argv[1])+"_v1/merged/merged.root").c_str());
    // // TFile *fout = new TFile(("output/test"+std::string(argv[1])+".root").c_str(), "RECREATE");    
    // loop(c, fout, false);

    c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/cosmicmuons_v3_lowerCavernDoublenPEQE_GEANT_v1/cosmics.root");
    TFile *fout = new TFile("test_cosmics_calib.root","RECREATE");
    loop(c, fout, true);

    fout->Close();
    return 0;
}

void loop(TChain *ch, TFile* fout, const bool COSMICS){

    std::map<std::string,Zoom> zooms;
    zooms["barzoom"] = {100, 0, 200000};
    zooms["slabzoom"] = {100, 0, 2000};
    zooms["spezoom"] = {100, 0, 100};
    zooms["datazoom"] = {100, 0, 1000};
    zooms["cosmicbarzoom"] = {100, 0, 30000};

    ///////////////////////////////////////////////////////////////////////
    // SET UP DIRECTORIES                                                //
    // add whatever directories you want in the file to the below list   //
    // "proc_names" list the various subprocess that we want to separate //
    // each directory will get a sub-directory for each process          //
    ///////////////////////////////////////////////////////////////////////
    std::vector<std::string> dnames; 
    if(COSMICS)
        dnames = {"inclusive", "neighbBars", "noScint", "verticalCosmic", "reco_verticalCosmic",
                  "verticalCosmic_all", "reco_verticalCosmic_all",
                  "verticalCosmic_neighbs", "reco_verticalCosmic_neighbs", 
                  "sideCosmic", "reco_sideCosmic",
                  "fourSlab", "reco_fourSlab", "slabNoBar"};
    else
        dnames = {"inclusive", "slabNoBar", "fourSlab", "noScint", "neighbBars", "nonneighbBars", 
                  "reco_fourSlab", "reco_slabNoBar", "reco_neighbBars", "reco_nonneighbBars"};
    std::vector<std::string> proc_names = {"muon","elscint", "elal", "elrock", "elpb", "other"};
    const int NPROCS = proc_names.size();
    std::map<std::string, Directory> dirs;
    for(std::string& dname : dnames){
        dirs[dname] = Directory();
        dirs[dname].dir = fout->mkdir(dname.c_str());

        // set up sub-directories for each process type
        // can comment out if you don't need these
        for(std::string& proc : proc_names){
            dirs[dname+"_"+proc] = Directory();
            dirs[dname+"_"+proc].dir = dirs[dname].dir->mkdir(proc.c_str());
        }

        if(COSMICS && dname.find("verticalCosmic") != std::string::npos){
            for(int i=0; i<6; i++){
                std::string pname = "path"+std::to_string(i);
                dirs[dname+"_"+pname] = Directory();
                dirs[dname+"_"+pname].dir = dirs[dname].dir->mkdir(pname.c_str());
                for(std::string& proc : proc_names){
                    dirs[dname+"_"+pname+"_"+proc] = Directory();
                    dirs[dname+"_"+pname+"_"+proc].dir = dirs[dname+"_"+pname].dir->mkdir(proc.c_str());
                }
            }
        }
    }
    // use the empty string to represent the "base" dir (i.e., the root level directory in the TFile)
    dirs[""] = Directory();
    dirs[""].dir = fout;

    std::vector<Config> cfgs;
    if(!COSMICS){
        // just beam muons
        cfgs.push_back({"inclusive",
                       {"barzoom", "slabzoom"},
                       false
                       });
        cfgs.push_back({"slabNoBar",
                       {"slabzoom", "spezoom"},
                       false
                       });
        cfgs.push_back({"fourSlab",
                       {"barzoom", "slabzoom"},
                       false
                       });
        cfgs.push_back({"nonneighbBars",
                       {"slabzoom", "spezoom"},
                       true
                       });
        cfgs.push_back({"reco_slabNoBar",
                       {"datazoom"},
                       false
                       });
        cfgs.push_back({"reco_fourSlab",
                       {"barzoom", "datazoom"},
                       false
                       });
        cfgs.push_back({"reco_neighbBars",
                       {"datazoom"},
                       true
                       });
        cfgs.push_back({"reco_nonneighbBars",
                       {"datazoom"},
                       true
                       });
    }else{
        // just cosmics
        cfgs.push_back({"inclusive",
                       {"cosmicbarzoom", "slabzoom"},
                       false
                       });
        cfgs.push_back({"verticalCosmic",
                       {"cosmicbarzoom", "slabzoom", "spezoom"},
                       false
                       });
        cfgs.push_back({"reco_verticalCosmic",
                       {"cosmicbarzoom", "slabzoom", "spezoom"},
                       false
                       });
        cfgs.push_back({"sideCosmic",
                       {"cosmicbarzoom", "datazoom", "spezoom"},
                       false
                       });
        cfgs.push_back({"reco_sideCosmic",
                       {"cosmicbarzoom", "datazoom", "spezoom"},
                       false
                       });
        cfgs.push_back({"verticalCosmic_all",
                       {},
                       false
                       });
        cfgs.push_back({"reco_verticalCosmic_all",
                       {},
                       false
                       });
        cfgs.push_back({"verticalCosmic_neighbs",
                       {"slabzoom", "datazoom", "spezoom"},
                       false
                       });
        cfgs.push_back({"reco_verticalCosmic_neighbs",
                       {"slabzoom", "datazoom", "spezoom"},
                       false
                       });
    }
    // both
    cfgs.push_back({"noScint",
                   {"slabzoom", "spezoom"},
                   false
                   });
    cfgs.push_back({"neighbBars",
                   {"slabzoom", "spezoom"},
                   true
                   });
    
    // logarithmic nPE binning
    const int nbins_nPE = 100;
    const float expmin=0, expmax=5;
    double nPE_logbins[nbins_nPE+1];
    for(int i=0; i<nbins_nPE+1; i++){
        nPE_logbins[i] = pow(10, expmin + (expmax-expmin)/nbins_nPE * i);
    }

    SlimTree s;
    uint nEvents = ch->GetEntries();
    vector<float*> proc_fracs = {s.chan_fracMuon, s.chan_fracElScint, s.chan_fracElAl, s.chan_fracElRock, s.chan_fracElPb, s.chan_fracOther};

    TObjArray *listOfFiles = ch->GetListOfFiles();
    TIter fileIter(listOfFiles);
    TFile *currentFile = 0;
    while ( (currentFile = (TFile*)fileIter.Next()) ) {

        std::cout << "Running over: " << currentFile->GetTitle() << std::endl;
        // Get File Content
        TFile f( currentFile->GetTitle() );
        TTree *tree = (TTree*)f.Get("Events");
        s.Init(tree);

        uint nEventsTree = tree->GetEntriesFast();
        float wgt = 1.0, global_wgt = 1.0;
        if(!COSMICS){
            if(std::string(currentFile->GetTitle()).find("qcd_nonbc") != std::string::npos){
                wgt = 1000 * 165.86 / 5e7 * 182168 / nEventsTree;
            }else if(std::string(currentFile->GetTitle()).find("qcd.root") != std::string::npos){
                wgt = 1000 * 714.76 / 5e7 * 187087 / nEventsTree;
            }else if(std::string(currentFile->GetTitle()).find("dy.root") != std::string::npos){
                wgt = 1000 * 5.5115 / 5e7 * 198614 / nEventsTree;
            }else if(std::string(currentFile->GetTitle()).find("w.root") != std::string::npos){
                wgt = 1000 * 13.293 / 5e7 * 200070 / nEventsTree;
            }else if(std::string(currentFile->GetTitle()).find("custombeam") != std::string::npos){
                wgt = 0.03838612108153 / 25;
            }else{
                wgt = 1.0;
            }
            global_wgt = 1000 * (165.86+714.76+5.5115+13.293) / 20e7 * (182168+187087+198614+200070) / nEvents;
            wgt *= lumi;
            global_wgt *= lumi;
        }
        
        for(uint ievt=0; ievt < nEventsTree; ievt++){
            s.GetEntry(ievt);
            s.progress(ievt, nEventsTree, 200);

            /////////////////////////
            // ANALYSIS CODE BELOW //
            /////////////////////////

            // calibrate nPE in each channel with empiracle factors
            if(TString(fout->GetName()).Contains("calib")){
                for(int i=0; i<32; i++)
                    // s.chan_nPE[i] *= pmt_calibrations.at(i);
                    s.chan_nPE[i] = gRandom->Binomial(s.chan_nPE[i], pmt_calibrations.at(i));
            }

            // some useful booleans for later
            bool reco_fourSlab = true, reco_noBar = true, reco_threeBarLine=false;
            for(int ch : SLAB_CHANS)
                if(s.chan_nPE[ch] < SLAB_THRESH)
                    reco_fourSlab = false;
            for(int ch : BAR_CHANS)
                if(s.chan_nPE[ch] >= BAR_THRESH)
                    reco_noBar = false;
            for(const std::vector<int>& path : line_paths){
                int ch1 = path.at(0), ch2 = path.at(1), ch3 = path.at(2);
                int npe1 = s.chan_nPE[ch1], npe2 = s.chan_nPE[ch2], npe3 = s.chan_nPE[ch3];
                if(npe1 >= BAR_THRESH && npe2 >= BAR_THRESH && npe3 >= BAR_THRESH){
                    reco_threeBarLine=true;
                    break;
                }
            }

            // fill global histograms
            if(s.mcTruth_fourSlab)
                plot1D("muE", s.mu_E0, wgt, dirs["fourSlab"], ";initial muon E [GeV]", 100, 0, 100);

            if(reco_fourSlab){
                plot1D("muE", s.mu_E0, wgt, dirs["reco_fourSlab"], ";initial muon E [GeV]", 100, 0, 100);
                if(reco_threeBarLine)
                    plot1D("muE_threeBarLine", s.mu_E0, wgt, dirs["reco_fourSlab"], ";initial muon E [GeV]", 100, 0, 100);
                // muon angle histograms
                // std::vector<int> threshs = {500, 750, 1000, 1500, 2000, 2500, 3500, 5000, 7500, 10000};
                if(!COSMICS){
                    std::vector<int> threshs = {750, 2500};
                    for(int thresh : threshs){
                        std::vector<std::vector<bool> > top_hits = get_top_hits(s.chan_nPE, thresh);
                        std::vector<std::vector<bool> > side_hits = get_side_hits(s.chan_nPE, thresh);
                        for(int ipatt=0; ipatt<patts_top.size(); ipatt++){
                            if(pass_pattern(patts_top.at(ipatt), top_hits))
                                plot1D("muangles_top_"+std::to_string(thresh), ipatt, wgt, dirs["reco_fourSlab"], "", patts_top.size(), 0, patts_top.size());
                        }
                        for(int ipatt=0; ipatt<patts_side.size(); ipatt++){
                            if(pass_pattern(patts_side.at(ipatt), side_hits))
                                plot1D("muangles_side_"+std::to_string(thresh), ipatt, wgt, dirs["reco_fourSlab"], "", patts_side.size(), 0, patts_side.size());
                        }
                    }
                }
            }                
            // signal-like event (3-bar-line) maxNPE histograms
            if((s.mcTruth_fourSlab && s.mcTruth_noBar) || s.mcTruth_noChan){
                std::string base_dir = s.mcTruth_noChan ? "noScint" : "slabNoBar";
                bool has_linepath = false;
                for(const std::vector<int>& path : line_paths){
                    int ch1 = path.at(0), ch2 = path.at(1), ch3 = path.at(2);
                    int npe1 = s.chan_nPE[ch1], npe2 = s.chan_nPE[ch2], npe3 = s.chan_nPE[ch3];
                    if(npe1==0 || npe2==0 || npe3==0)
                        continue;
                    has_linepath = true;
                    float t1 = s.chan_firstTime[ch1], t2 = s.chan_firstTime[ch2], t3 = s.chan_firstTime[ch3];
                    int max_npe = std::max(std::max(npe1, npe2), npe3);
                    float max_dt = std::max(std::max(t1,t2),t3) - std::min(std::min(t1,t2),t3);
                    plot1D("threeBarLine_maxDT",  max_dt,  wgt, dirs[base_dir], ";max dt [ns]", 100, 0, 100);
                    plot1D("threeBarLine_maxNPE", max_npe, wgt, dirs[base_dir], ";max nPE", 1000, 0, 1000);
                    plot2D("threeBarLine_muXY", s.mu_y0, s.mu_x0, 1.0, dirs[base_dir], ";y0 [m]; x0 [m]", 100, -0.3 ,0.3, 100, -0.3, 0.3);
                    plot2D("threeBarLine_muXYfar", s.mu_y0, s.mu_x0, 1.0, dirs[base_dir], ";y0 [m]; x0 [m]", 100, -3 ,3, 100, -3, 3);
                    vector<float> fracs;
                    for(int i=0; i<NPROCS; i++){
                        Directory &subd = dirs[base_dir+"_"+proc_names.at(i)];
                        float frac = npe1*proc_fracs.at(i)[ch1]+npe2*proc_fracs.at(i)[ch2]+npe3*proc_fracs.at(i)[ch3];
                        frac /= (npe1+npe2+npe3);
                        fracs.push_back(frac);
                        plot1D("threeBarLine_maxNPE", max_npe, wgt*frac, subd, ";max nPE", 1000, 0, 1000); 
                    }
                    // if(std::string(currentFile->GetTitle()).find(".root") != std::string::npos){
                    //     std::cout << "FOUNDEVENT threeBarLine " << base_dir << ": " << s.fileID << " " << s.orig_evt << " ";
                    //     for(float frac : fracs)
                    //         std::cout << frac << " ";
                    //     std::cout << "\n";
                    // }
                }
                std::vector<std::vector<int> > layer_pulses = {{}, {}, {}};
                for(int ch : BAR_CHANS){
                    if(s.chan_nPE[ch] > 0)
                        layer_pulses[bar_layer(ch)-1].push_back(ch);
                }
                int npanelhits = 0;
                for(int ch : PANEL_CHANS){
                    if(s.chan_nPE[ch] > 0)
                        npanelhits++;
                }
                if(layer_pulses.at(0).size()==1 && layer_pulses.at(1).size()==1 && layer_pulses.at(2).size()==1){
                    int ch1 = layer_pulses.at(0).at(0), ch2 = layer_pulses.at(1).at(0), ch3 = layer_pulses.at(2).at(0);
                    int npe1 = s.chan_nPE[ch1], npe2 = s.chan_nPE[ch2], npe3 = s.chan_nPE[ch3];
                    float t1 = s.chan_firstTime[ch1], t2 = s.chan_firstTime[ch2], t3 = s.chan_firstTime[ch3];
                    std::vector<int> sorted = {ch1, ch2, ch3};
                    std::sort(sorted.begin(), sorted.end());
                    bool is_nonpoint = false;
                    for(const std::vector<int>& path : same_digi_nonpointing){
                        if(sorted.at(0)==path.at(0) && sorted.at(1)==path.at(1) && sorted.at(2)==path.at(2)){
                            is_nonpoint = true;
                            break;
                        }
                    }
                    int min_npe = std::min(std::min(npe1, npe2), npe3);
                    int max_npe = std::max(std::max(npe1, npe2), npe3);
                    float max_dt = std::max(std::max(t1,t2),t3) - std::min(std::min(t1,t2),t3);
                    vector<std::string> prefs = {"onePulsePerLayer"};
                    if(npanelhits==0)
                        prefs.push_back("onePulsePerLayerNoPanel");
                    if(has_linepath)
                        prefs.push_back("onePulsePerLayerPoint");
                    if(npanelhits==0 && has_linepath)
                        prefs.push_back("onePulsePerLayerPointNoPanel");
                    if(is_nonpoint)
                        prefs.push_back("onePulsePerLayerNonPoint");
                    if(npanelhits==0 && is_nonpoint)
                        prefs.push_back("onePulsePerLayerNonPointNoPanel");
                    for(std::string pref : prefs){
                        plot1D(pref+"_maxDT",  max_dt,  wgt, dirs[base_dir], ";max dt [ns]", 100, 0, 100);
                        plot1D(pref+"_minNPE", min_npe, wgt, dirs[base_dir], ";max nPE", 1000, 0, 1000);
                        plot1D(pref+"_maxNPE", max_npe, wgt, dirs[base_dir], ";max nPE", 1000, 0, 1000);
                        plot2D(pref+"_muXY", s.mu_y0, s.mu_x0, 1.0, dirs[base_dir], ";y0 [m]; x0 [m]", 100, -0.3 ,0.3, 100, -0.3, 0.3);
                        plot2D(pref+"_muXYfar", s.mu_y0, s.mu_x0, 1.0, dirs[base_dir], ";y0 [m]; x0 [m]", 100, -3 ,3, 100, -3, 3);
                    }
                    vector<float> fracs;
                    for(int i=0; i<NPROCS; i++){
                        Directory &subd = dirs[base_dir+"_"+proc_names.at(i)];
                        float frac = npe1*proc_fracs.at(i)[ch1]+npe2*proc_fracs.at(i)[ch2]+npe3*proc_fracs.at(i)[ch3];
                        frac /= (npe1+npe2+npe3);
                        fracs.push_back(frac);
                        for(std::string pref : prefs)
                            plot1D(pref+"_maxNPE", max_npe, wgt*frac, subd, ";max nPE", 1000, 0, 1000); 
                    }
                    // if(std::string(currentFile->GetTitle()).find(".root") != std::string::npos && npanelhits==0){
                    //     std::cout << "FOUNDEVENT onePulsePerLayer " << base_dir << ": " << s.fileID << " " << s.orig_evt << " ";
                    //     // for(float frac : fracs)
                    //     //     std::cout << frac << " ";
                    //     if(has_linepath) std::cout << "LINE ";
                    //     std::cout << "\n";
                    // }
                }
            }

            // baseline nPE and timing plots for all channels
            for(int ich=0; ich<NCHAN; ich++){
                if(s.chan_nPE[ich] == 0)
                    continue;
                std::string sch = std::to_string(ich);

                for(Config& cfg : cfgs){
                    if(cfg.name=="slabNoBar" && (!s.mcTruth_fourSlab || !s.mcTruth_noBar))
                        continue;
                    if(cfg.name=="fourSlab" && (!s.mcTruth_fourSlab))
                        continue;
                    if(cfg.name=="reco_fourSlab" && (!reco_fourSlab))
                        continue;
                    if(cfg.name=="noScint" && (!s.mcTruth_noChan))
                        continue;
                    if(cfg.justbars && !isbar(ich))
                        continue;

                    if(cfg.name.find("neighbBars") != std::string::npos){
                        bool isreco = (cfg.name.find("reco") != std::string::npos);
                        bool isnonneighb = (cfg.name.find("nonneighb") != std::string::npos);
                        
                        if(!isreco && ((!COSMICS && !s.mcTruth_fourSlab) || s.chan_fracMuon[ich] > 0))
                            continue;
                        if(isreco && !reco_fourSlab)
                            continue;

                        bool neighbhit = false;
                        bool nonneighbhit = false;
                        int idx = std::find(BAR_CHANS.begin(), BAR_CHANS.end(), ich) - BAR_CHANS.begin();
                        for(int i : chan_neighbs.at(idx).at(1)){
                            if((!isreco && s.chan_fracMuon[i] > 0) || (isreco && s.chan_nPE[i] >= BAR_THRESH))
                                neighbhit = true;
                        }
                        for(int i : chan_neighbs.at(idx).at(0)){
                            if((!isreco && s.chan_fracMuon[i] > 0) || (isreco && s.chan_nPE[i] >= BAR_THRESH))
                                nonneighbhit = true;
                        }
                        if(!isnonneighb && !neighbhit)
                            continue;
                        if(isnonneighb && (neighbhit || !nonneighbhit))
                            continue;

                        // if(isreco && !isnonneighb && s.chan_nPE[ich]<10){
                        //     std::cout << s.fileID << " " << s.orig_evt << " " << ich << std::endl;
                        // }
                    }

                    std::vector<int> vcosmic_paths;
                    if(cfg.name.find("verticalCosmic") != string::npos){
                        bool isreco = (cfg.name.find("reco") != std::string::npos);
                        if(!isreco && !s.mcTruth_verticalCosmic)
                            continue; 
                        int ipath = 0;
                        if(isreco){
                            bool found = false;
                            for( ; ipath<cosmic_paths.size(); ipath++){
                                const std::vector<int>& path = cosmic_paths.at(ipath).at(0);
                                if(s.chan_nPE[path.at(0)] >= PANEL_THRESH &&
                                   s.chan_nPE[path.at(1)] >= BAR_THRESH &&
                                   s.chan_nPE[path.at(2)] >= BAR_THRESH &&
                                   s.chan_nPE[path.at(3)] >= BAR_THRESH){
                                    found = true;
                                    vcosmic_paths.push_back(ipath);
                                    break;
                                }                                    
                            }
                            if(!found) continue;
                        }else{
                            ipath = s.mcTruth_verticalCosmic-1;
                            vcosmic_paths.push_back(ipath);
                        }
                        const std::vector<int>& path = cosmic_paths.at(ipath).at(0);
                        const std::vector<int>& neighbs = cosmic_paths.at(ipath).at(1);
                        if(cfg.name.find("neighb") != std::string::npos){
                            if(!isreco && s.chan_fracMuon[ich] > 0)
                                continue;
                            if(std::find(neighbs.begin(), neighbs.end(), ich) == neighbs.end())
                                continue;
                        }else if(cfg.name.find("all") != std::string::npos){
                            ;
                        }else{
                            if(std::find(path.begin(), path.end(), ich) == path.end())
                                continue;
                        }
                    }

                    if(cfg.name.find("sideCosmic") != string::npos){
                        bool isreco = (cfg.name.find("reco") != std::string::npos);
                        int ipath = 0;
                        bool found = false;
                        if(isreco){
                            for( ; ipath<side_cosmic_paths.size(); ipath++){
                                const std::vector<std::vector<int> >& path = side_cosmic_paths.at(ipath);
                                if(s.chan_nPE[path.at(0).at(0)] >= PANEL_THRESH &&
                                   s.chan_nPE[path.at(0).at(1)] >= PANEL_THRESH &&
                                   // (s.chan_nPE[path.at(1).at(0)] >= BAR_THRESH ||
                                   //  s.chan_nPE[path.at(1).at(1)] >= BAR_THRESH ||
                                   //  s.chan_nPE[path.at(1).at(2)] >= BAR_THRESH ) &&
                                   // (s.chan_nPE[path.at(2).at(0)] >= BAR_THRESH ||
                                   //  s.chan_nPE[path.at(2).at(1)] >= BAR_THRESH ||
                                   //  s.chan_nPE[path.at(2).at(2)] >= BAR_THRESH ) ){
                                   (s.chan_nPE[path.at(1).at(1)] >= BAR_THRESH ||
                                    s.chan_nPE[path.at(2).at(1)] >= BAR_THRESH) ){
                                    found = true;
                                    break;
                                }                                    
                            }
                        }else{
                            for( ; ipath<side_cosmic_paths.size(); ipath++){
                                const std::vector<std::vector<int> >& path = side_cosmic_paths.at(ipath);
                                if(s.chan_fracMuon[path.at(0).at(0)] > 0 &&
                                   s.chan_fracMuon[path.at(0).at(1)] > 0 &&
                                   // (s.chan_fracMuon[path.at(1).at(0)] > 0 ||
                                   //  s.chan_fracMuon[path.at(1).at(1)] > 0 ||
                                   //  s.chan_fracMuon[path.at(1).at(2)] > 0 ) &&
                                   // (s.chan_fracMuon[path.at(2).at(0)] > 0 ||
                                   //  s.chan_fracMuon[path.at(2).at(1)] > 0 ||
                                   //  s.chan_fracMuon[path.at(2).at(2)] > 0 ) ) {
                                   (s.chan_fracMuon[path.at(1).at(1)] > 0 ||
                                    s.chan_fracMuon[path.at(2).at(1)] > 0) ) {
                                    found = true;
                                    break;
                                }                                    
                            }
                        }
                        if(!found) continue;
                        const std::vector<std::vector<int> >& path = side_cosmic_paths.at(ipath);
                        // if(std::find(path.at(0).begin(), path.at(0).end(), ich) == path.at(0).end() &&
                        //    std::find(path.at(1).begin(), path.at(1).end(), ich) == path.at(1).end() &&
                        //    std::find(path.at(2).begin(), path.at(2).end(), ich) == path.at(2).end() )
                        if(std::find(path.at(0).begin(), path.at(0).end(), ich) == path.at(0).end() &&
                           !(ich == path.at(1).at(1) && s.chan_fracMuon[path.at(1).at(1)] > 0) &&
                           !(ich == path.at(2).at(1) && s.chan_fracMuon[path.at(2).at(1)] > 0) )
                            continue;
                    }
                    
                    if(cfg.name=="reco_slabNoBar" && (!reco_fourSlab || !reco_noBar))
                            continue;

                    for(std::string zname : cfg.zooms){
                        Zoom& z = zooms[zname];
                        for(int i=-1; i<NPROCS; i++){
                            Directory& subd = (i<0) ? dirs[cfg.name] : dirs[cfg.name+"_"+proc_names.at(i)];
                            float frac = (i<0) ? 1.0 : proc_fracs.at(i)[ich];
                            plot1D("nPE_"+zname+"_ch"+sch, s.chan_nPE[ich], frac*wgt, subd, ";nPE", z.nbins, z.xlow, z.xhigh);
                            if(isbar(ich))
                                plot1D("nPE_"+zname+"_totBar", s.chan_nPE[ich], frac*wgt, subd, ";nPE", z.nbins, z.xlow, z.xhigh);
                            if(isslab(ich))
                                plot1D("nPE_"+zname+"_totSlab", s.chan_nPE[ich], frac*wgt, subd, ";nPE", z.nbins, z.xlow, z.xhigh);
                            if(ispanel(ich))
                                plot1D("nPE_"+zname+"_totPanel", s.chan_nPE[ich], frac*wgt, subd, ";nPE", z.nbins, z.xlow, z.xhigh);
                            
                        }// proc loop
                        
                    }// zoom loop

                    if(cfg.name.find("verticalCosmic") != std::string::npos){
                        for(int ipath : vcosmic_paths){
                            std::string dname = cfg.name+"_path"+std::to_string(ipath);
                            plot1D("nPE_ch"+sch, s.chan_nPE[ich], wgt, dirs[dname], ";nPE", 1000,0,1000);
                            for(int i=0; i<NPROCS; i++){
                                Directory& subd = dirs[cfg.name+"_path"+std::to_string(ipath)+"_"+proc_names.at(i)];
                                float frac = proc_fracs.at(i)[ich];
                                plot1D("nPE_ch"+sch, s.chan_nPE[ich], wgt*frac, subd, ";nPE", 1000,0,1000);
                            }
                        }
                    }

                    if(isbar(ich)){
                        plot2D("maxSource_nPE_E0", s.chan_maxSourceE0[ich], s.chan_maxSourceNPE[ich], wgt, dirs[cfg.name],
                               ";E0 [MeV]; nPE", 100, 0, 2, 100, 0, 100);
                        if(s.chan_maxSourceType[ich] == 1)
                            plot2D("maxSource_nPE_E0_ele", s.chan_maxSourceE0[ich], s.chan_maxSourceNPE[ich], wgt, dirs[cfg.name],
                                   ";E0 [MeV]; nPE", 100, 0, 2, 100, 0, 100);
                        if(s.chan_maxSourceType[ich] == 2)
                            plot2D("maxSource_nPE_E0_gam", s.chan_maxSourceE0[ich], s.chan_maxSourceNPE[ich], wgt, dirs[cfg.name],
                                   ";E0 [MeV]; nPE", 100, 0, 2, 100, 0, 100);
                        plot2D("maxSource_type", s.chan_maxSourceType[ich], s.chan_maxSourceNPE[ich], wgt, dirs[cfg.name], 
                               ";type;nPE", 6, 0, 6, 10, 0, 100);
                    }

                    plot1D("chan", ich, wgt, dirs[cfg.name], ";chan", 32, 0, 32);
                    if(isbar(ich) && s.chan_nPE[ich]<20)
                        plot1D("chan_bar_lowNPE", ich, wgt, dirs[cfg.name], ";chan", 32, 0, 32);

                    // timing info
                    if((cfg.name.find("slabNoBar")!=std::string::npos || 
                        cfg.name.find("fourSlab")!=std::string::npos || 
                        cfg.name.find("neighbBar")!=std::string::npos ||
                        cfg.name.find("verticalCosmic_neighbs")!=std::string::npos) 
                       && isbar(ich)){
                        bool isreco = (cfg.name.find("reco") != std::string::npos);
                        float reftime = s.chan_firstTime[20];
                        if(cfg.name.find("neighbBar")!=std::string::npos){
                            // find the channel to use for the reference time. Want one of the neighbors/non-neighbors with a muon
                            int idx = std::find(BAR_CHANS.begin(), BAR_CHANS.end(), ich) - BAR_CHANS.begin();
                            vector<int> candidates = chan_neighbs.at(idx).at(1);
                            if(cfg.name.find("non")!=std::string::npos)
                                candidates = chan_neighbs.at(idx).at(0);
                            reftime = -100;
                            for(int jch : candidates){
                                if((isreco && s.chan_nPE[jch] >= BAR_THRESH) || (!isreco && s.chan_fracMuon[jch]>0)){
                                    reftime = s.chan_firstTime[jch];
                                    break;
                                }
                            }
                            if(reftime<-99) \
                                std::cout << "WARNING: didn't find a suitable reference, and we should have!" << std::endl;
                        }
                        if(cfg.name.find("verticalCosmic_neighbs")!=std::string::npos){
                            int refch = ich%2==0 ? ich+1 : ich-1;
                            reftime = s.chan_firstTime[refch];
                        }
                        float dt = s.chan_firstTime[ich] - reftime;
                        plot1D("dT_mu", dt, wgt, dirs[cfg.name], ";#Delta#kern[0]{t}(#mu, secondary) [ns])", 100, -20, 20);
                        plotProfile("dT_mu_prof", s.chan_nPE[ich], dt, wgt, dirs[cfg.name], 
                                    ";nPE;#Delta#kern[0]{t}(#mu, secondary) [ns])", 50, 0, 500, -30, 30, "S");
                        plot2D("h2d_npe_dt_totBar", dt, s.chan_nPE[ich], wgt, dirs[cfg.name], ";dt [ns];nPE",
                               80, -10, 30, nbins_nPE, nPE_logbins);
                        plot2D("h2d_npe_dt_ch"+std::to_string(ich), dt, s.chan_nPE[ich], wgt, dirs[cfg.name], ";dt [ns];nPE",
                               80, -10, 30, nbins_nPE, nPE_logbins);
                        for(int ip=0; ip<NPROCS; ip++){
                            Directory &subd = dirs[cfg.name+"_"+proc_names.at(ip)];
                            float frac = proc_fracs.at(ip)[ich];
                            plot1D("dT_mu", dt, frac*wgt, subd, ";#Delta#kern[0]{t}(#mu, secondary) [ns])", 100, -20, 20);
                        }
                    }

                } // config loop

            } // channel loop               
               
        } // event loop
        delete tree;
        f.Close();
    } // file loop

    // write histos
    for(std::pair<std::string, Directory> d : dirs){
        d.second.dir->cd();
        for(std::pair<std::string, TH1*> pair : d.second.histmap){
            pair.second->Write();
        }
    }

}

void plot1D(std::string name, float xval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax){
    dir.dir->cd();
    if (title=="") title=name;
    std::map<string, TH1*>::iterator iter= dir.histmap.find(name);
    if(iter == dir.histmap.end()){ //no histo for this yet, so make a new one
        TH1D* currentHisto= new TH1D(name.c_str(), title.c_str(), numbinsx, xmin, xmax);
        currentHisto->Sumw2();
        currentHisto->Fill(xval, weight);
        dir.histmap.insert(std::pair<string, TH1D*> (name,currentHisto) );
    }else{ // exists already, so just fill it
        (*iter).second->Fill(xval, weight);
    }
    
}

void plot2D(std::string name, float xval, float yval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax,
            int numbinsy, float ymin, float ymax){
    dir.dir->cd();
    if (title=="") title=name;
    std::map<string, TH1*>::iterator iter= dir.histmap.find(name);
    if(iter == dir.histmap.end()){ //no histo for this yet, so make a new one
        TH2D* currentHisto= new TH2D(name.c_str(), title.c_str(), numbinsx, xmin, xmax, numbinsy, ymin, ymax);
        currentHisto->Sumw2();
        currentHisto->Fill(xval, yval, weight);
        dir.histmap.insert(std::pair<string, TH2D*> (name,currentHisto) );
    }else{ // exists already, so just fill it
        ((TH2D*) (*iter).second)->Fill(xval, yval, weight);
    }
    
}

void plot2D(std::string name, float xval, float yval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax,
            int numbinsy, const double *ybins){
    dir.dir->cd();
    if (title=="") title=name;
    std::map<string, TH1*>::iterator iter= dir.histmap.find(name);
    if(iter == dir.histmap.end()){ //no histo for this yet, so make a new one
        TH2D* currentHisto= new TH2D(name.c_str(), title.c_str(), numbinsx, xmin, xmax, numbinsy, ybins);
        currentHisto->Sumw2();
        currentHisto->Fill(xval, yval, weight);
        dir.histmap.insert(std::pair<string, TH2D*> (name,currentHisto) );
    }else{ // exists already, so just fill it
        ((TH2D*) (*iter).second)->Fill(xval, yval, weight);
    }
    
}

void plotProfile(std::string name, float xval, float yval, double weight, Directory& dir,
                 std::string title, int numbinsx, float xmin, float xmax, 
                 float ymin, float ymax, std::string opt){
    dir.dir->cd();
    if (title=="") title=name;
    std::map<string, TH1*>::iterator iter= dir.histmap.find(name);
    if(iter == dir.histmap.end()){ //no histo for this yet, so make a new one
        TProfile* currentHisto= new TProfile(name.c_str(), title.c_str(), numbinsx, xmin, xmax, ymin, ymax, opt.c_str());
        currentHisto->Fill(xval, yval, weight);
        dir.histmap.insert(std::pair<string, TProfile*> (name,currentHisto) );
    }else{ // exists already, so just fill it
        ((TProfile*)(*iter).second)->Fill(xval, yval, weight);
    }

}

bool isbar(int ch){
    auto p = std::find(BAR_CHANS.begin(), BAR_CHANS.end(), ch);
    return (p != BAR_CHANS.end());
}
bool isslab(int ch){
    auto p = std::find(SLAB_CHANS.begin(), SLAB_CHANS.end(), ch);
    return (p != SLAB_CHANS.end());
}
bool ispanel(int ch){
    auto p = std::find(PANEL_CHANS.begin(), PANEL_CHANS.end(), ch);
    return (p != PANEL_CHANS.end());
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

#include <algorithm>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"

#include "looper.h"
#include "../SlimTree/SlimTree.h"

const int NCHAN = 32;
const int NBARS = 18;
const int NSLABS = 4;
const int NPANELS = 9;
int BAR_CHANS[NBARS] = {0,1,24,25,8,9,6,7,16,17,12,13,2,3,22,23,4,5};
int SLAB_CHANS[NSLABS] = {18,20,28,21};
int PANEL_CHANS[NPANELS] = {27,10,29,30,11,19,31,14,26};

int main() {

    TChain *c = new TChain("Events");
    c->Add("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/run2/merged/merged.root");

    TFile *fout = new TFile("test.root","RECREATE");

    loop(c, fout);

    fout->Close();
    return 0;
}

void loop(TChain *ch, TFile* fout){

    ///////////////////////////////////////////////////////////////////////
    // SET UP DIRECTORIES                                                //
    // add whatever directories you want in the file to the below list   //
    // "proc_names" list the various subprocess that we want to separate //
    // each directory will get a sub-directory for each process          //
    ///////////////////////////////////////////////////////////////////////
    vector<std::string> dnames = {"inclusive", "fourSlab"};
    vector<std::string> proc_names = {"muon","elscint", "elal", "elrock", "elpb", "other"};

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
    }
    // use the empty string to represent the "base" dir (i.e., the root level directory in the TFile)
    dirs[""] = Directory();
    dirs[""].dir = fout;

    SlimTree s;
    uint nEvents = ch->GetEntries();

    TObjArray *listOfFiles = ch->GetListOfFiles();
    TIter fileIter(listOfFiles);
    TFile *currentFile = 0;
    while ( (currentFile = (TFile*)fileIter.Next()) ) {

        // Get File Content
        TFile f( currentFile->GetTitle() );
        TTree *tree = (TTree*)f.Get("Events");
        s.Init(tree);

        uint nEventsTree = tree->GetEntriesFast();
        for(uint ievt=0; ievt < nEventsTree; ievt++){
            s.GetEntry(ievt);
            
            /////////////////////////
            // ANALYSIS CODE BELOW //
            /////////////////////////

            // sample loop that fill nPE histograms for every channel, both inclusively and in fourSlab events
            for(int ich=0; ich<NCHAN; ich++){
                if(s.chan_nPE[ich] == 0)
                    continue;
                std::string sch = std::to_string(ich);

                for(std::string& dname : dnames){
                    if(dname=="fourSlab" && !s.mcTruth_fourSlab)
                        continue;
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], 1.0, dirs[dname], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], 1.0, dirs[dname], ";nPE", 100, 0, 2000);
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracMuon[ich], dirs[dname+"_muon"], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracMuon[ich], dirs[dname+"_muon"], ";nPE", 100, 0, 2000);
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElScint[ich], dirs[dname+"_elscint"], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElScint[ich], dirs[dname+"_elscint"], ";nPE", 100, 0, 2000);
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElAl[ich], dirs[dname+"_elal"], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElAl[ich], dirs[dname+"_elal"], ";nPE", 100, 0, 2000);
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElRock[ich], dirs[dname+"_elrock"], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElRock[ich], dirs[dname+"_elrock"], ";nPE", 100, 0, 2000);
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElPb[ich], dirs[dname+"_elpb"], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracElPb[ich], dirs[dname+"_elpb"], ";nPE", 100, 0, 2000);
                    plot1D("nPE_barzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracOther[ich], dirs[dname+"_other"], ";nPE", 100, 0, 200000);
                    plot1D("nPE_slabzoom_ch"+sch, s.chan_nPE[ich], s.chan_fracOther[ich], dirs[dname+"_other"], ";nPE", 100, 0, 2000);
                    
                }

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
            std::string title, int numbinsx, float xmin, float xmax)
{
    dir.dir->cd();
    if (title=="") title=name;
    std::map<string, TH1*>::iterator iter= dir.histmap.find(name);
    if(iter == dir.histmap.end()) //no histo for this yet, so make a new one
        {
            TH1D* currentHisto= new TH1D(name.c_str(), title.c_str(), numbinsx, xmin, xmax);
            currentHisto->Sumw2();
            currentHisto->Fill(xval, weight);
            dir.histmap.insert(std::pair<string, TH1D*> (name,currentHisto) );
        }
    else // exists already, so just fill it
        {
            (*iter).second->Fill(xval, weight);
        }

}

bool isbar(int ch){
    int *p = std::find(BAR_CHANS, BAR_CHANS+NBARS, ch);
    return (p != BAR_CHANS+NBARS);
}
bool isslab(int ch){
    int *p = std::find(SLAB_CHANS, SLAB_CHANS+NSLABS, ch);
    return (p != SLAB_CHANS+NSLABS);
}
bool ispanel(int ch){
    int *p = std::find(PANEL_CHANS, PANEL_CHANS+NPANELS, ch);
    return (p != PANEL_CHANS+NPANELS);
}

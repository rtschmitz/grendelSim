#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
#include "/home/schmitz/milliQanSim/include/mqROOTEvent.hh"
R__LOAD_LIBRARY(../../build/libBenchCore.so)

void PMTHitNumberAndNPE(){

  TChain ch("Events");
   ch.Add("/home/schmitz/test/milliQanSim/build/Sim_-422636624MilliQan.root"); //some test sim output file

  TH1F *hNPE = new TH1F("hNPE","Photoelectrons Detected Per Incident Gamma, Test",50,0,50); //
  hNPE->GetXaxis()->SetTitle("Number of Photoelectrons Detected");
  hNPE->GetYaxis()->SetTitle("Bin Members");

  TCanvas *c1 = new TCanvas("c1","c1",0,400,600,300);
  c1->SetLogy();
  ch.Draw("NbOfPMTHits>>hNPE"); 

mqROOTEvent* myROOTEvent = new mqROOTEvent();
ch.SetBranchAddress("ROOTEvent",&myROOTEvent);
Long64_t nentries = ch.GetEntries();
std::cout << nentries << std::endl; 

int pmtNumber,numPMTHits;
vector<int> pmtNumberVec;
for (Long64_t i=0;i<nentries;i++) {
	 ch.GetEntry(i);			   //get the content for this event
	 numPMTHits=myROOTEvent->GetPMTRHits()->size();    //get the number of hits in this event
         if(numPMTHits>0){                                 //sometimes you only want to loop if you have any hits
  	 	for(int h=0;h<numPMTHits;h++){             //loop over hits
			pmtNumber = myROOTEvent->GetPMTRHits()->at(h)->GetPMTNumber(); //get the pmt number of the hit
			pmtNumberVec.push_back(pmtNumber); //store the pmt number of the hit
		}
	}
   	if(i%(nentries/100)==0) cout << i*100/nentries <<"% complete" << endl; //monitor loop progress
}

TH1I* hPMTNumber = new TH1I("hPMTNumber","PMT Number of hits",100,1000,1100); //change to fit your pmt numbering range
hPMTNumber->GetXaxis()->SetTitle("PMT Number");
hPMTNumber->GetYaxis()->SetTitle("Bin Members");
hPMTNumber->SetLineColor(kBlue);
for(int m=0;m<pmtNumberVec.size();m++) hPMTNumber->Fill(pmtNumberVec[m]);
TCanvas* c2 = new TCanvas("c2","c2",0,400,600,300);
hPMTNumber->Draw();
}

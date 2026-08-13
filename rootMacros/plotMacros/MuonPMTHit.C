#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"
R__LOAD_LIBRARY(../../build/libBenchCore.so)

void MuonPMTHit(){

  TChain ch("Events");
   ch.Add("/home/schmitz/milliQanSim/build/Sim_1125852016MilliQan.root"); //some test sim output file

  TH1F *hNPE = new TH1F("hNPE","Photoelectrons Detected Per Incident Gamma, Test",50,0,50); //
  hNPE->GetXaxis()->SetTitle("Number of Photoelectrons Detected");
  hNPE->GetYaxis()->SetTitle("Bin Members");

  TCanvas *c1 = new TCanvas("c1","c1",0,400,600,300);
  c1->SetLogy();
  ch.Draw("NbOfPMTHits>>hNPE");
  
}

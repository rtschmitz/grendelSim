#include "TCanvas.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TChain.h"
#include "../include/mqROOTEvent.hh"
#include "TGraph.h"
#include "TVector.h"
#include "TVectorD.h"
#include "TVectorF.h"
#include "TH1.h"
#include "TH1F.h"
#include "TString.h"
#include "TChain.h"
#include "TMultiGraph.h"
#include <algorithm>
#include <string>
R__LOAD_LIBRARY(../lib/libPhysics.so)
R__LOAD_LIBRARY(../lib/libMilliQanCore.so)
using namespace std;


void BeamHits(){

  TChain rootEvents("Events");
  TString fileDir;

//  for(int i=10;i<100;i++){ // some file won't be there but that's expected
 //               fileDir="/hadoop/cms/store/user/ryan/beammuons/beammuons."+to_string(i)+".root";
//                rootEvents.Add(fileDir);
//  }
  fileDir="/media/ryan/Storage/computing/milliQanFullSim/data/gammaEDepSpectrum/MilliQan.root";
  rootEvents.Add(fileDir);
//  TString xTitle = "Number of Detections for 3-Bar-Hit Gammas";
  std::cout << "Reading file..." << std::endl;
  mqROOTEvent* myROOTEvent = new mqROOTEvent();
  rootEvents.SetBranchAddress("ROOTEvent", &myROOTEvent);
  Long64_t nentries=rootEvents.GetEntries();

//initialize variables
int numPMTHits=0;
int barHits=0;
int slabHits=0;
int panelHits=0;
int demoHits=0;
int hitN=0;

bool h6=false;
bool h7=false;
bool h8=false;

int count = 0;
int hit=0;
int depth=0;
vector<int> PMTHits;
vector<int> nPMTHitDepth;
vector<int> hitDepthCount;
const int maxdepth=12;
int hitDepth[maxdepth];
for(int init=0;init<maxdepth;init++) hitDepth[init]=0;
TH1I *hHitDepthNPE[maxdepth];
for(int i=0;i<maxdepth;i++) hHitDepthNPE[i] = new TH1I("","",100,0,1000);

  for (Long64_t i=0;i<nentries;i++) {
//  for (Long64_t i=0;i<100;i++) { //for debugging

  	rootEvents.GetEntry(i);
	
	//get number of hits in the target region
	barHits = myROOTEvent->GetBarHit();
//	slabHits = myROOTEvent->GetSlabHit();
//	panelHits = myROOTEvent->GetPanelHit();
//	demoHits = barHits + slabHits + panelHits;
	//look at the event if we had hits
//	if(demoHits > 0){
//	if(barHits > 0) {
		//get number of hits in PMTs anywhere
		numPMTHits=myROOTEvent->GetPMTRHits()->size();
		if(numPMTHits>0){
	        	for(int h=0;h<numPMTHits;h++){
                        	hitN=myROOTEvent->GetPMTRHits()->at(h)->GetPMTNumber();
				//hitN % 216 = in-layer number, 4*substack + volnum(1-4)
				//(hitN % 216) % 4 = volnum, 1-2 = higher, 3-4 = lower
				//((hitN-1 % 216)-1) / 4  = substack num
				//((hitN-1 % 216) / 4) % 9 = substack pos, 8-7-6-5-4-3-2-1-0 descending
				//depth = 8-(((hitN-1 % 216) / 4) % 9)*2+(((hitN-1 % 216) % 4)) / 2
				//17*4+1 = upper;
				//cout << "hitN: " << hitN << endl;
				if(hitN<=6*9*4*4) depth=(8-(((hitN-1) % 216) / 4) % 9)*2+(((hitN-1) % 216) % 4) / 2;
				else depth = -1;

				if(depth==-1) hitDepth[maxdepth-1]++; //extra hits
				else if(depth>(maxdepth-4)) hitDepth[maxdepth-3]++; //high depth hits
				else hitDepth[depth]++; //all other hits

				//if(hitN==6 && !h6){hit++;h6=true;}
				//if(hitN==7 && !h7){hit++;h7=true;}
				//if(hitN==8 && !h8){hit++;h8=true;}
				nPMTHitDepth.push_back(depth);
				depth=0;
                 	}
			for(int k=0;k<maxdepth;k++) {hHitDepthNPE[k]->Fill(hitDepth[k]);
							hitDepth[k]=0;}
		}
//	}
//	}
	//reset all the temporary variables
	if(i%(nentries/100)==0) cout << i*100/nentries <<"% complete" << endl;
//	if(i%(100/100)==0) cout << i*100/100 <<"% complete" << endl;
}
//TFile *out = new TFile("/home/users/ryan/milliQanDemoSim/data/beammuons.root","RECREATE");

int nbins=10;

//Number of times in each event a hit was registered in a distinct bar
TH1I* hNumBarsHit = new TH1I("hNumBarsHit","Bar Hit Depth (all hits), gammas",20,-1.5,18.5);
hNumBarsHit->GetXaxis()->SetTitle("Depth of PMT Hits");
hNumBarsHit->GetYaxis()->SetTitle("Bin Members");
hNumBarsHit->SetTitle(xTitle);
hNumBarsHit->SetLineColor(kBlue);
for(int m=0;m<nPMTHitDepth.size();m++) hNumBarsHit->Fill(nPMTHitDepth[m]);
TCanvas* c1 = new TCanvas("c1","c1",0,400,600,300);
hNumBarsHit->Draw();

TCanvas* c2 = new TCanvas("c2","c2",0,400,600,300);
hHitDepthNPE[0]->GetXaxis()->SetTitle("nPE Per Hit Depth");
hHitDepthNPE[0]->GetYaxis()->SetTitle("Bin Members");
hHitDepthNPE[0]->SetTitle("Hit Depth nPE Spectra, gammas");
hHitDepthNPE[0]->SetLineColor(kRed);
hHitDepthNPE[0]->Draw();

for(int c=1;c<maxdepth-3;c++) {hHitDepthNPE[c]->SetLineColor(kRed-c); hHitDepthNPE[c]->Draw("same");}
hHitDepthNPE[maxdepth-3]->SetLineColor(kBlue);
hHitDepthNPE[maxdepth-3]->Draw("same");
hHitDepthNPE[maxdepth-1]->SetLineColor(kGreen+2);
hHitDepthNPE[maxdepth-1]->Draw("same");

TLegend *leg = new TLegend(0.6,0.6,0.8,0.8);
//for(int m=0;m<maxdepth-3;m++)
leg->AddEntry(hHitDepthNPE[0], "depth: 0","l");
leg->AddEntry(hHitDepthNPE[1], "depth: 1","l");
leg->AddEntry(hHitDepthNPE[2], "depth: 2","l");
leg->AddEntry(hHitDepthNPE[3], "depth: 3","l");
leg->AddEntry(hHitDepthNPE[4], "depth: 4","l");
leg->AddEntry(hHitDepthNPE[5], "depth: 5","l");
leg->AddEntry(hHitDepthNPE[6], "depth: 6","l");
leg->AddEntry(hHitDepthNPE[7], "depth: 7","l");
leg->AddEntry(hHitDepthNPE[8], "depth: 8","l");

leg->AddEntry(hHitDepthNPE[maxdepth-3],"this bin + deeper hits","l");
leg->AddEntry(hHitDepthNPE[maxdepth-1],"panel hits","l");
leg->Draw();

//clean up memory
PMTHits.clear();
nPMTHitDepth.clear();
}

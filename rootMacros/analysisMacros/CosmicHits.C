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
R__LOAD_LIBRARY(libPhysics.so)
R__LOAD_LIBRARY(../build/libMilliQanCore.so)
using namespace std;

void CosmicHits(){

  TChain rootEvents("Events");

  TString fileDir;
 
  for(int i=10;i<100;i++){ // some file won't be there but that's expected
		fileDir="/hadoop/cms/store/user/ryan/cosmicmuons/cosmicmuons."+to_string(i)+".root";
		rootEvents.Add(fileDir);
  }

//  TString fileDir10 = "/hadoop/cms/store/user/ryan/cosmicmuons/cosmicmuons.10.root";
//  TString fileDir11 = "/hadoop/cms/store/user/ryan/cosmicmuons/cosmicmuons.11.root";



//  TString textFilepath = "/media/ryan/Storage/computing/mqFullSim/data/";
  TString xTitle = "Number of Muon-Generated Photons";
//  rootEvents.Add(fileDir10);
//  rootEvents.Add(fileDir11);
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

int count = 0;
vector<vector<Int_t>> PMTHitNumVec;
int PMTHits[31]={0};
Int_t hitN=0;
int muonThresholdBar=500;
int muonThresholdSlab=200;
int muonThresholdPanel=50;
int anyHit[31]={false};
int muonHit[31]={false};
int muonLayerHit[3]={0};
int anyLayerHit[3]={0};
int muonSlabHit[4]={0};
int anySlabHit[4]={0};
int barHitNum=0;
int trigOtherHitZero=0;
int trigOtherHit=0;
int trigLayerNum=0;
int trigSameHit=0;
int trigSameHitZero=0;
int peFromHit=0;
int peFromHitZero=0;
int numBarHitCount=0;
int numTriggers=0;
int k,h;
//vector<int> sameLayerBarHit;
//vector<int> otherLayerBarHit;
vector<Double_t> sameLayerBarHit;
vector<Double_t> otherLayerBarHit;
vector<int> totalNumBarHitCount;
//get data in file
  for (Long64_t i=0;i<nentries;i++) {
//  for (Long64_t i=0;i<5000;i++) { //for debugging

  	rootEvents.GetEntry(i);
	
	//get number of hits in the target region
	barHits = myROOTEvent->GetBarHit();
	slabHits = myROOTEvent->GetSlabHit();
	panelHits = myROOTEvent->GetPanelHit();
	demoHits = barHits + slabHits + panelHits;

	//look at the event if we had hits
	if(demoHits > 0){
		
		//get number of hits in PMTs anywhere
		numPMTHits=myROOTEvent->GetPMTRHits()->size();
		if(numPMTHits>0){
			
			//loop over PMT Hits and store hits in different regions into a hit count array
			for(h=0;h<numPMTHits;h++){
				hitN=myROOTEvent->GetPMTRHits()->at(h)->GetPMTNumber();
				PMTHits[hitN]++;
			}

			//check to see if muons or other particles hit bars/slabs/panels
			for(h=0;h<31;h++){
				if(PMTHits[h]>0){

				if(h<18) {
					anyHit[h]=true;
					numBarHitCount++;
					anyLayerHit[h/6]++;
					if(PMTHits[h]>muonThresholdBar){
						muonHit[h]=true;
						muonLayerHit[h/6]++;
					}
				}
				else if(h<22 && h>17) {
					anyHit[h]=true;
					anySlabHit[h-18]++;
					if(PMTHits[h]>muonThresholdBar){
						muonHit[h]=true;
						muonSlabHit[h-18]=true;
					}
				}
				else if(h>21) {
					anyHit[h]=true;
					anyLayerHit[(h-22)/3]++;
					if(PMTHits[h]>muonThresholdBar){
						muonHit[h]=true;
						muonLayerHit[(h-22)/3]++;
					}
				}
				}
			}
			totalNumBarHitCount.push_back(numBarHitCount);
			numBarHitCount=0;
			
			//for muons which triggered on a 3 bar stack, populate  histogram for other bars
			for(h=0;h<6;h++){
				if(muonHit[3*h] && muonHit[3*h+1] && muonHit[3*h+2]){
					numTriggers++;
					trigLayerNum=h/2;
					cout << "Triple trigger event in barstack " << h << " in layer " << h/2 << endl;
					if(trigLayerNum==0){
						
						if(anyLayerHit[1]==0 && anyLayerHit[2]==0) {
							cout << "No hits in other layers! " << endl;
							trigOtherHitZero++;
						}
						else {
							for(k=6;k<18;k++) if(anyHit[k]) otherLayerBarHit.push_back((double)PMTHits[k]);
							trigOtherHit++;
						}
					}
					if(trigLayerNum==1){
						if(anyLayerHit[0]==0 && anyLayerHit[2]==0) {
							cout << "No hits in other layers! " << endl;
							trigOtherHitZero++;
						}
						else {
							for(k=0;k<6;k++) if(anyHit[k])  otherLayerBarHit.push_back((double)PMTHits[k]);
							for(k=12;k<18;k++) if(anyHit[k])  otherLayerBarHit.push_back((double)PMTHits[k]);
							trigOtherHit++;
						}
					}
					if(trigLayerNum==2){
						if(anyLayerHit[0]==0 && anyLayerHit[1]==0) {
							cout << "No hits in other layers! " << endl;
							trigOtherHitZero++;
						}
						else {
							for(k=0;k<12;k++) if(anyHit[k]) otherLayerBarHit.push_back((double)PMTHits[k]);
							trigOtherHit++;
						}
					}
					for(k=trigLayerNum*6;k<trigLayerNum*6+6;k++){
						if(k!=3*h && k!=3*h+1 && k!=3*h+2){
							if(PMTHits[k]>0) { sameLayerBarHit.push_back((double)PMTHits[k]); trigSameHit++;}
							else {trigSameHitZero++;} 
						}
					}
				}
			}	
		peFromHit++;	
		}
		else peFromHitZero++;
	}
	for(h=0;h<31;h++) {PMTHits[h]=0; anyHit[h]=false; muonHit[h]=false;}
	for(h=0;h<3;h++) {muonLayerHit[h]=0; anyLayerHit[h]=0;}
	for(h=0;h<4;h++) {muonSlabHit[h]=0; anySlabHit[h]=0;}

	if(i%10000==0) cout << "Finished analyzing event number " << i << endl;
}

cout << "============================= Event Analysis =====================================" << endl;
cout << "FOR ALL EVENTS: " << endl;
cout << "Events which had PE detections, as a percentage of events in which any particle hit the detector: " << (double)peFromHit/(peFromHit+peFromHitZero) << endl;
cout << "Number of 3 bar stack triggers: " << numTriggers << endl;
cout << "=====================================================" << endl;
cout << "FOR 3 BAR STACK MUON TRIGGERS:" << endl;
cout << "Percentage of times we saw a hit in other layers: " << (double)trigOtherHit/(trigOtherHit+trigOtherHitZero) << endl;
cout << "Percentage of times we saw a hit in the other 3-bar stack in the same layer: " << (double)trigSameHit/(trigSameHit+trigSameHitZero) << endl;

TFile *out = new TFile("/home/users/ryan/milliQanDemoSim/data/cosmicmuons.root","RECREATE");

//create some datastructures to set upper bounds
TVectorD tsameLayerBarHit(sameLayerBarHit.size(), &sameLayerBarHit[0]);
TVectorD totherLayerBarHit(otherLayerBarHit.size(), &otherLayerBarHit[0]);

int nbins=100;

//Number of times in each event a hit was registered in a distinct bar
TH1I* hNumBarsHit = new TH1I("hNumBarsHit","",18,0,18);//tmuonHitsBack.Max()*(1+(double)1/nbins));
hNumBarsHit->GetXaxis()->SetTitle("Number of Bars which detected photons per event");
hNumBarsHit->GetYaxis()->SetTitle("Bin Members");
hNumBarsHit->SetLineColor(kBlue);

//PMT Hit spectrum, bars in the same layer as a triggering muon
TH1D* hSameHit = new TH1D("hSameHit","",nbins,0,tsameLayerBarHit.Max()*(1+(double)1/nbins));
hSameHit->GetXaxis()->SetTitle("Number of PE detected in bars in same layer as trigger stack");
hSameHit->GetYaxis()->SetTitle("Bin Members");
hSameHit->SetLineColor(kBlue);

//PMT Hit spectrum, bars in the same layer as a triggering muon
TH1D* hSameHitZoom = new TH1D("hSameHitZoom","",nbins,0,nbins);
hSameHitZoom->GetXaxis()->SetTitle("Number of PE detected in bars in same layer as trigger stack");
hSameHitZoom->GetYaxis()->SetTitle("Bin Members");
hSameHitZoom->SetLineColor(kBlue);

//PMT Hit spectrum, bars in the different layers as a triggering muon
TH1D* hOtherHit = new TH1D("hOtherHit","",nbins,0,totherLayerBarHit.Max()*(1+(double)1/nbins));
hOtherHit->GetXaxis()->SetTitle("Number of PE detected in bars in different layer as trigger stack");
hOtherHit->GetYaxis()->SetTitle("Bin Members");
hOtherHit->SetLineColor(kBlue);

//PMT Hit spectrum, bars in the different layers as a triggering muon
TH1D* hOtherHitZoom = new TH1D("hOtherHit","",nbins,0,nbins);
hOtherHitZoom->GetXaxis()->SetTitle("Number of PE detected in bars in different layer as trigger stack");
hOtherHitZoom->GetYaxis()->SetTitle("Bin Members");
hOtherHitZoom->SetLineColor(kBlue);


for(int m=0;m<totalNumBarHitCount.size();m++) hNumBarsHit->Fill(totalNumBarHitCount[m]);
for(int m=0;m<sameLayerBarHit.size();m++) hSameHit->Fill(sameLayerBarHit[m]);
for(int m=0;m<sameLayerBarHit.size();m++) hSameHitZoom->Fill(sameLayerBarHit[m]);
for(int m=0;m<otherLayerBarHit.size();m++) hOtherHit->Fill(otherLayerBarHit[m]);
for(int m=0;m<otherLayerBarHit.size();m++) hOtherHitZoom->Fill(otherLayerBarHit[m]);


//plot stuff
TCanvas* c1 = new TCanvas("c1","c1",0,400,600,300);
hNumBarsHit->Draw();

TCanvas* c2 = new TCanvas("c2","c2",0,400,600,300);
hSameHit->Draw();

TCanvas* c3 = new TCanvas("c3","c3",0,400,600,300);
hSameHitZoom->Draw();

TCanvas* c4 = new TCanvas("c4","c4",0,400,600,300);
hOtherHit->Draw();

TCanvas* c5 = new TCanvas("c5","c5",0,400,600,300);
hOtherHitZoom->Draw();

out->Write();

//clean up memory
sameLayerBarHit.clear();
otherLayerBarHit.clear();
totalNumBarHitCount.clear();
}

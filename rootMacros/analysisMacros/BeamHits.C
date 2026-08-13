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
R__LOAD_LIBRARY(../lib/libPhysics.so)
R__LOAD_LIBRARY(../lib/libMilliQanCore.so)
using namespace std;


void BeamHits(){

  TChain rootEvents("Events");
  TString fileDir;

  for(int i=10;i<100;i++){ // some file won't be there but that's expected
                fileDir="/hadoop/cms/store/user/ryan/beammuons/beammuons."+to_string(i)+".root";
                rootEvents.Add(fileDir);
  }

/*
  TString fileDir13 = "/hadoop/cms/store/user/ryan/beammuons/beammuons.13.root";
  TString fileDir11 = "/hadoop/cms/store/user/ryan/beammuons/beammuons.11.root";
  TString fileDir70 = "/hadoop/cms/store/user/ryan/beammuons/beammuons.70.root";
  TString fileDir72 = "/hadoop/cms/store/user/ryan/beammuons/beammuons.72.root";
  TString fileDir73 = "/hadoop/cms/store/user/ryan/beammuons/beammuons.73.root";
  TString fileDir75 = "/hadoop/cms/store/user/ryan/beammuons/beammuons.75.root";
*/
//  TString textFilepath = "/media/ryan/Storage/computing/mqFullSim/data/";
  TString xTitle = "Number of Muon-Generated Photons";
/*
  rootEvents.Add(fileDir13);
  rootEvents.Add(fileDir11);
  rootEvents.Add(fileDir70);
  rootEvents.Add(fileDir72);
  rootEvents.Add(fileDir73);
  rootEvents.Add(fileDir75);
*/
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

const double barSlabCalTime=1.454;
double tempTime1 = 1000000;
double tempTime2 = 1000000;
double tempTime3 = 1000000;
double muonTime1=0;
double muonTime2=0;
double muonTime3=0;
double slabTime1=0;
double slabTime2=0;
double slabTime3=0;

int count = 0;
int barHitSum=0;
bool noBarHit=true;
int slabHitSum=0;
vector<vector<Int_t>> PMTHitNumVec;
int PMTHits[31]={0};
double PMTHitTime[31]={1000000};
Int_t hitN=0;
Double_t hitTime=0;
int muonThresholdBar=60000;
int muonThresholdSlab=400;
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
int slabOtherHitZero=0;
int slabOtherHit=0;
int peFromHit=0;
int peFromHitZero=0;
int numBarHitCount=0;
int numSlabBarHitCount=0;
int numTriggers=0;
int numTriggersOnly3Bar=0;
int numSlabOnlyTriggers=0;
int numTriggersOnly4Slab=0;
int k,h;
int slabOnly=0;
int slabAndBar=0;
bool trig=false;
bool fourSlabEvent=false;
int hitsTemp1=0;
int hitsTemp2=0;
int hitsTemp3=0;
int ktemp1=-1;
int ktemp2=-1;
int ktemp3=-1;
vector<double> slabTime;
vector<double> barTime;
vector<Double_t> otherBarHit;
vector<Double_t> otherSlabHit;
vector<int> totalNumBarHitCount;
vector<int> totalNumSlabBarHitCount;
vector<Double_t> layerDelay;
vector<Double_t> layerDelayLow;
vector<Double_t> layerDelayMid;
vector<Double_t> layerDelayHigh;

vector<Double_t> slabDelay;
vector<Double_t> slabDelayLow;
vector<Double_t> slabDelayMid;
vector<Double_t> slabDelayHigh;
//get data in file
vector<Double_t> slabHitSpectrum;
vector<Double_t> barHitSpectrum;
  for (Long64_t i=0;i<nentries;i++) {
//  for (Long64_t i=0;i<50;i++) { //for debugging

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
				hitTime=myROOTEvent->GetPMTRHits()->at(h)->GetFirstHitTime();
				if(hitTime<PMTHitTime[hitN]) PMTHitTime[hitN]=hitTime;
			}

			//check to see if muons or other particles hit bars/slabs/panels
			for(h=0;h<31;h++){
				if(PMTHits[h]>0){

				if(h<18) {
					anyHit[h]=true;
					anyLayerHit[h/6]++;
					barHitSpectrum.push_back(PMTHits[h]);
					if(PMTHits[h]>muonThresholdBar){
						muonHit[h]=true;
						muonLayerHit[h/6]++;
						noBarHit=false;
					}
				}
				else if(h<22 && h>17) {
					anyHit[h]=true;
					anySlabHit[h-18]++;
					if(h==18) slabHitSpectrum.push_back(PMTHits[h]);
					if(PMTHits[h]>muonThresholdSlab){
						muonHit[h]=true;
						muonSlabHit[h-18]=true;
					}
				}
				else if(h>21) {
					anyHit[h]=true;
					anyLayerHit[(h-22)/3]++;
					if(PMTHits[h]>muonThresholdPanel){
						muonHit[h]=true;
						muonLayerHit[(h-22)/3]++;
					}
				}
				}
			}


		        
			if(muonHit[18] && muonHit[19] && muonHit[20] && muonHit[21]){
				if(noBarHit) {slabHitSum++; fourSlabEvent=true;}
			        else {barHitSum++;}
			}

				if(fourSlabEvent){
					slabTime1=PMTHitTime[18]+barSlabCalTime;
					slabTime2=PMTHitTime[19]+barSlabCalTime;
					slabTime3=PMTHitTime[20]+barSlabCalTime;
					numSlabOnlyTriggers++;
					for(k=0;k<18;k++){
						if(PMTHits[k]>0) {
						        numSlabBarHitCount++;
							otherSlabHit.push_back((double)PMTHits[k]);
							slabOtherHit++;
							if(k<6){
								if(PMTHits[k]>hitsTemp1){
									hitsTemp1=PMTHits[k];
									ktemp1=k;
								}
							}
							else if(k<12){
								if(PMTHits[k]>hitsTemp2){
									hitsTemp2=PMTHits[k];
									ktemp2=k;
								}
							}								
							else if(PMTHits[k]>hitsTemp3){
								hitsTemp3=PMTHits[k];
								ktemp3=k;	
							}
						} else {slabOtherHitZero++;}
					}
					if(hitsTemp1+hitsTemp2+hitsTemp3==0) numTriggersOnly4Slab++;
					if(ktemp1>-1) {
						tempTime1=PMTHitTime[ktemp1];
						if(tempTime1!=1000000){ //should always be true, but as a precaution
							if(PMTHits[ktemp1]>1000) slabDelayHigh.push_back(slabTime1-tempTime1);
							else if(PMTHits[ktemp1]>100) slabDelayMid.push_back(slabTime1-tempTime1);
							else if(PMTHits[ktemp1]>0) slabDelayLow.push_back(slabTime1-tempTime1);
							slabDelay.push_back(slabTime1-tempTime1);
						}
					}
					if(ktemp2>-1) {
						tempTime2=PMTHitTime[ktemp2];
						if(tempTime2!=1000000){ //should always be true, but as a precaution
							if(PMTHits[ktemp2]>1000) slabDelayHigh.push_back(slabTime2-tempTime2);
							else if(PMTHits[ktemp2]>100) slabDelayMid.push_back(slabTime2-tempTime2);
							else if(PMTHits[ktemp2]>0) slabDelayLow.push_back(slabTime2-tempTime2);
							slabDelay.push_back(slabTime2-tempTime2);
						}
					}
					if(ktemp3>-1) {
						tempTime3=PMTHitTime[ktemp3];
						if(tempTime3!=1000000){ //should always be true, but as a precaution
							if(PMTHits[ktemp3]>1000) slabDelayHigh.push_back(slabTime3-tempTime3);
							else if(PMTHits[ktemp3]>100) slabDelayMid.push_back(slabTime3-tempTime3);
							else if(PMTHits[ktemp3]>0) slabDelayLow.push_back(slabTime3-tempTime3);
							slabDelay.push_back(slabTime3-tempTime3);
						}
					}
				
				hitsTemp1=0;
				hitsTemp2=0;
				hitsTemp3=0;
				ktemp1=-1;
				ktemp2=-1;
				ktemp3=-1;
				tempTime1=1000000;
				tempTime2=1000000;
				tempTime3=1000000;
				
				}

			//for muons which triggered on a 3 bar stack, populate  histogram for other bars
			for(h=0;h<6;h++){
				///*
				//*/
				//script for triple trigger on a straight line path for a muon
				if(muonHit[h] && muonHit[h+6] && muonHit[h+12]){
					if(!trig) trig=true;
					else cout << "ERROR: TWO TRIGGERS IN ONE EVENT! Possibly adjust muon threshold" << endl;
					numTriggers++;
					numBarHitCount=3;
					cout << "Triple trigger event in barstack position " << h << endl;
					muonTime1=PMTHitTime[h];
					muonTime2=PMTHitTime[h+6];
					muonTime3=PMTHitTime[h+12];
					barTime.push_back(PMTHitTime[h]);
					slabTime.push_back(PMTHitTime[18]);
					for(k=0;k<18;k++){
						if(k % 3 != 1){
							if(k!=h && k!=h+6 && k!=h+12){
								if(PMTHits[k]>0) {
									numBarHitCount++;
									trigOtherHit++;
									otherBarHit.push_back((double)PMTHits[k]);
									if(k<6){
										if(PMTHits[k]>hitsTemp1){
											hitsTemp1=PMTHits[k];
											ktemp1=k;
										}
									}
									else if(k<12){
										if(PMTHits[k]>hitsTemp2){
											hitsTemp2=PMTHits[k];
											ktemp2=k;
										}
									}								
									else if(PMTHits[k]>hitsTemp3){
										hitsTemp3=PMTHits[k];
										ktemp3=k;	
									}
								} else {trigOtherHitZero++;}
							}
						}
					}
					if(hitsTemp1+hitsTemp2+hitsTemp3==0) numTriggersOnly3Bar++;
				}
			}
					
					if(ktemp1>-1) {
						tempTime1=PMTHitTime[ktemp1];
						if(tempTime1!=1000000){ //should always be true, but as a precaution
							if(PMTHits[ktemp1]>1000) layerDelayHigh.push_back(muonTime1-tempTime1);
							else if(PMTHits[ktemp1]>100) layerDelayMid.push_back(muonTime1-tempTime1);
							else if(PMTHits[ktemp1]>0) layerDelayLow.push_back(muonTime1-tempTime1);
							layerDelay.push_back(muonTime1-tempTime1);
						}
					}
					if(ktemp2>-1) {
						tempTime2=PMTHitTime[ktemp2];
						if(tempTime2!=1000000){ //should always be true, but as a precaution
							if(PMTHits[ktemp2]>1000) layerDelayHigh.push_back(muonTime2-tempTime2);
							else if(PMTHits[ktemp2]>100) layerDelayMid.push_back(muonTime2-tempTime2);
							else if(PMTHits[ktemp2]>0) layerDelayLow.push_back(muonTime2-tempTime2);
							layerDelay.push_back(muonTime2-tempTime2);
						}
					}
					if(ktemp3>-1) {
						tempTime3=PMTHitTime[ktemp3];
						if(tempTime3!=1000000){ //should always be true, but as a precaution
							if(PMTHits[ktemp3]>1000) layerDelayHigh.push_back(muonTime3-tempTime3);
							else if(PMTHits[ktemp3]>100) layerDelayMid.push_back(muonTime3-tempTime3);
							else if(PMTHits[ktemp3]>0) layerDelayLow.push_back(muonTime3-tempTime3);
							layerDelay.push_back(muonTime3-tempTime3);
						}
					}
		peFromHit++;	
		totalNumBarHitCount.push_back(numBarHitCount);
		totalNumSlabBarHitCount.push_back(numSlabBarHitCount);
		}
		else peFromHitZero++;
	}
	//reset all the temporary variables
	for(h=0;h<31;h++) {PMTHits[h]=0; PMTHitTime[h]=1000000; anyHit[h]=false; muonHit[h]=false;}
	for(h=0;h<3;h++) {muonLayerHit[h]=0; anyLayerHit[h]=0;}
	for(h=0;h<4;h++) {muonSlabHit[h]=0; anySlabHit[h]=0;}
	noBarHit=true;
	fourSlabEvent=false;
	trig=false;
	numBarHitCount=0;
	numSlabBarHitCount=0;
	hitsTemp1=0;
	hitsTemp2=0;
	hitsTemp3=0;
	ktemp1=-1;
	ktemp2=-1;
	ktemp3=-1;
	tempTime1=1000000;
	tempTime2=1000000;
	tempTime3=1000000;
	if(i%100==0) cout << "Finished analyzing event number " << i << endl;
}

double slabAverage1;
slabAverage1=(double)std::accumulate(slabTime.begin(),slabTime.end(),0.0)/slabTime.size();
double barAverage1;
barAverage1=(double)std::accumulate(barTime.begin(),barTime.end(),0.0)/barTime.size();

cout << "============================= Event Analysis =====================================" << endl;
cout << "FOR ALL EVENTS: " << endl;
cout << "Events which had PE detections, as a percentage of events in which any particle hit the detector: " << (double)peFromHit/(peFromHit+peFromHitZero) << endl;
cout << "Number of 3 bar line triggers: " << numTriggers << endl;
cout << "Number of 4-slab muon passages: " << barHitSum+slabHitSum << endl;
cout << "Fraction of events which went through 4 slabs but no bars: " << (double)barHitSum/(barHitSum+slabHitSum) << endl;
cout << "Average hit time in first slab: " << slabAverage1 << " and sample size: " << slabTime.size() << endl;
cout << "Average hit time in first layer in bar: " << barAverage1 << " and sample size: " << barTime.size() << endl;
cout << "Typical delay time between muon hits in first slab and muon hits in first bar: " << barAverage1-slabAverage1 << endl;
cout << "=====================================================" << endl;
cout << "FOR 3 BAR LINE MUON TRIGGERS:" << endl;
cout << "Percentage of times a given non-trigger bar had a hit: " << (double)trigOtherHit/(trigOtherHit+trigOtherHitZero) << endl;
cout << "Percentage of times that NO non-trigger bar had a hit: " << (double)numTriggersOnly3Bar/numTriggers << endl;
cout << "=====================================================" << endl;
cout << "FOR 4 SLAB MUONS WHICH HIT NO BARS:" << endl;
cout << "Percentage of times a given non-trigger bar had a hit: " << (double)slabOtherHit/(slabOtherHit+slabOtherHitZero) << endl;
cout << "Percentage of times that NO non-trigger bar had a hit: " << (double)numTriggersOnly4Slab/numSlabOnlyTriggers << endl;
cout << "Num slab only triggers: " << numSlabOnlyTriggers << endl;
TFile *out = new TFile("/home/users/ryan/milliQanDemoSim/data/beammuons.root","RECREATE");

//create some datastructures to set upper bounds

int nbins=100;

//Number of times in each event a hit was registered in a distinct bar
TH1I* hNumBarsHit = new TH1I("hNumBarsHit","Number of Bars which detected photons per event",15,3,18);//tmuonHitsBack.Max()*(1+(double)1/nbins));
hNumBarsHit->GetXaxis()->SetTitle("Number of Bars hit");
hNumBarsHit->GetYaxis()->SetTitle("Bin Members");
hNumBarsHit->SetLineColor(kBlue);
for(int m=0;m<totalNumBarHitCount.size();m++) hNumBarsHit->Fill(totalNumBarHitCount[m]);

//Number of times in each event a hit was registered in a distinct bar
TH1I* hNumSlabBarsHit = new TH1I("hNumSlabBarsHit","Number of Bars which detected photons per event",18,0,18);//tmuonHitsBack.Max()*(1+(double)1/nbins));
hNumSlabBarsHit->GetXaxis()->SetTitle("Number of Bars hit");
hNumSlabBarsHit->GetYaxis()->SetTitle("Bin Members");
hNumSlabBarsHit->SetLineColor(kBlue);
for(int m=0;m<totalNumSlabBarHitCount.size();m++) hNumSlabBarsHit->Fill(totalNumSlabBarHitCount[m]);


TVectorD tslabHitSpectrum(slabHitSpectrum.size(), &slabHitSpectrum[0]);
TH1D* hSlabHit = new TH1D("hSlabHit","nPE distribution in slabs",nbins,0,2000);//tslabHitSpectrum.Max()*(1+(double)1/nbins));
hSlabHit->GetXaxis()->SetTitle("nPE distribution in slabs");
hSlabHit->GetYaxis()->SetTitle("Bin Members");
hSlabHit->SetLineColor(kBlue);
for(int m=0;m<slabHitSpectrum.size();m++) hSlabHit->Fill(slabHitSpectrum[m]);

TVectorD tbarHitSpectrum(barHitSpectrum.size(), &barHitSpectrum[0]);
TH1D* hBarHit = new TH1D("hBarHit","nPE distribution in bars",nbins,5000,160000);//tbarHitSpectrum.Max()*(1+(double)1/nbins));
hBarHit->GetXaxis()->SetTitle("nPE distribution in bars");
hBarHit->GetYaxis()->SetTitle("Bin Members");
hBarHit->SetLineColor(kBlue);
for(int m=0;m<barHitSpectrum.size();m++) hBarHit->Fill(barHitSpectrum[m]);


TVectorD totherBarHit(otherBarHit.size(), &otherBarHit[0]);

//PMT Hit spectrum, bars in the different layers as a triggering muon
TH1D* hOtherHit = new TH1D("hOtherHit","",nbins,0,totherBarHit.Max()*(1+(double)1/nbins));
hOtherHit->SetTitle("Number of PE detected in bars outside of the trigger line");
hOtherHit->GetXaxis()->SetTitle("nPE in bar");
hOtherHit->GetYaxis()->SetTitle("Bin Members");
hOtherHit->SetLineColor(kBlue);
for(int m=0;m<otherBarHit.size();m++) hOtherHit->Fill(otherBarHit[m]);

//PMT Hit spectrum, bars in the different layers as a triggering muon
TH1D* hOtherHitZoom = new TH1D("hOtherHitZoom","",nbins,0,nbins);
hOtherHitZoom->SetTitle("Number of PE detected in bars outside of the trigger line");
hOtherHitZoom->GetXaxis()->SetTitle("nPE in bar");
hOtherHitZoom->GetYaxis()->SetTitle("Bin Members");
hOtherHitZoom->SetLineColor(kBlue);
for(int m=0;m<otherBarHit.size();m++) hOtherHitZoom->Fill(otherBarHit[m]);

TVectorD totherSlabHit(otherSlabHit.size(), &otherSlabHit[0]);

//PMT Hit spectrum, bars in the different layers as a triggering muon
TH1D* hOtherSlabHit = new TH1D("hOtherSlabHit","",nbins,0,totherSlabHit.Max()*(1+(double)1/nbins));
hOtherSlabHit->SetTitle("Number of PE detected in bars for events with no muons in bars");
hOtherSlabHit->GetXaxis()->SetTitle("nPE in bar");
hOtherSlabHit->GetYaxis()->SetTitle("Bin Members");
hOtherSlabHit->SetLineColor(kBlue);
for(int m=0;m<otherSlabHit.size();m++) hOtherSlabHit->Fill(otherSlabHit[m]);

//PMT Hit spectrum, bars in the different layers as a triggering muon
TH1D* hOtherSlabHitZoom = new TH1D("hOtherSlabHitZoom","",nbins,0,nbins);
hOtherSlabHitZoom->SetTitle("Number of PE detected in bars for events with no muons in bars");
hOtherSlabHitZoom->GetXaxis()->SetTitle("nPE in bar");
hOtherSlabHitZoom->GetYaxis()->SetTitle("Bin Members");
hOtherSlabHitZoom->SetLineColor(kBlue);
for(int m=0;m<otherSlabHit.size();m++) hOtherSlabHitZoom->Fill(otherSlabHit[m]);

//plot stuff
TCanvas* c1 = new TCanvas("c1","c1",0,400,600,300);
hNumBarsHit->Draw();

TCanvas* c2 = new TCanvas("c2","c2",0,400,600,300);
hOtherHit->Draw();

TCanvas* c3 = new TCanvas("c3","c3",0,400,600,300);
hOtherHitZoom->Draw();
	
TCanvas* c8 = new TCanvas("c8","c8",0,400,600,300);
hSlabHit->Draw();

TCanvas* c9 = new TCanvas("c9","c9",0,400,600,300);
hBarHit->Draw();

TCanvas* c10 = new TCanvas("c10","c10",0,400,600,300);
hOtherSlabHit->Draw();

TCanvas* c11 = new TCanvas("c11","c11",0,400,600,300);
hOtherSlabHitZoom->Draw();

TCanvas* c12 = new TCanvas("c12","c12",0,400,600,300);
hNumSlabBarsHit->Draw();

double lowerBound=-10;
double upperBound=0;
if(layerDelay.size()>0){
	TVectorD tlayerDelay(layerDelay.size(), &layerDelay[0]);
	//PMT Hit spectrum, bars in the different layers as a triggering muon
	TH1D* hLayerDelay = new TH1D("hLayerDelay","",nbins,lowerBound,upperBound);//tlayerDelay.Min()*(1+(double)1/nbins),tlayerDelay.Max()*(1+(double)1/nbins));
	hLayerDelay->SetTitle("Delay time between muon PE detection, first secondary PE detection in layer (ns)");
	hLayerDelay->GetXaxis()->SetTitle("Time difference (ns)");
	hLayerDelay->GetYaxis()->SetTitle("Bin Members");
	hLayerDelay->SetLineColor(kBlack);
	
	for(int m=0;m<layerDelay.size();m++) hLayerDelay->Fill(layerDelay[m]);
	
	TCanvas* c4 = new TCanvas("c4","c4",0,400,600,300);
	hLayerDelay->Draw();
} else cout << "ERROR: NO HITS TO COMPARE!" << endl;

if(layerDelayLow.size()>0){
	TVectorD tlayerDelayLow(layerDelayLow.size(), &layerDelayLow[0]);
	//PMT Hit spectrum, bars in the different layers as a triggering muon
	//TH1D* hLayerDelayLow = new TH1D("hLayerDelayLow","",nbins,tlayerDelayLow.Min()*(1+(double)1/nbins),tlayerDelayLow.Max()*(1+(double)1/nbins));
	TH1D* hLayerDelayLow = new TH1D("hLayerDelayLow","",nbins,lowerBound,upperBound);//tlayerDelay.Min()*(1+(double)1/nbins),tlayerDelay.Max()*(1+(double)1/nbins));
	hLayerDelayLow->SetTitle("Delay time between muon PE detection, first secondary PE detection in layer, NPE<100 (ns)");
	hLayerDelayLow->GetXaxis()->SetTitle("Time difference (ns)");
	hLayerDelayLow->GetYaxis()->SetTitle("Bin Members");
	hLayerDelayLow->SetLineColor(kRed);

	for(int m=0;m<layerDelayLow.size();m++) hLayerDelayLow->Fill(layerDelayLow[m]);

//	TCanvas* c5 = new TCanvas("c5","c5",0,400,600,300);
	hLayerDelayLow->Draw("same");
} else cout << "ERROR: NO HITS TO COMPARE FOR NPE<100!" << endl;

if(layerDelayMid.size()>0){
	TVectorD tlayerDelayMid(layerDelayMid.size(), &layerDelayMid[0]);
	//PMT Hit spectrum, bars in the different layers as a triggering muon
	//TH1D* hLayerDelayMid = new TH1D("hLayerDelayMid","",nbins,tlayerDelayMid.Min()*(1+(double)1/nbins),tlayerDelayMid.Max()*(1+(double)1/nbins));
	TH1D* hLayerDelayMid = new TH1D("hLayerDelayMid","",nbins,lowerBound,upperBound);//tlayerDelay.Min()*(1+(double)1/nbins),tlayerDelay.Max()*(1+(double)1/nbins));
	hLayerDelayMid->SetTitle("Delay time between muon PE detection, first secondary PE detection in layer, 1000>NPE>100 (ns)");
	hLayerDelayMid->GetXaxis()->SetTitle("Time difference (ns)");
	hLayerDelayMid->GetYaxis()->SetTitle("Bin Members");
	hLayerDelayMid->SetLineColor(kGreen);

	for(int m=0;m<layerDelayMid.size();m++) hLayerDelayMid->Fill(layerDelayMid[m]);

//	TCanvas* c6 = new TCanvas("c6","c6",0,400,600,300);
	hLayerDelayMid->Draw("same");
} else cout << "ERROR: NO HITS TO COMPARE FOR 1000>NPE>100!" << endl;

if(layerDelayHigh.size()>0){
	TVectorD tlayerDelayHigh(layerDelayHigh.size(), &layerDelayHigh[0]);
	//PMT Hit spectrum, bars in the different layers as a triggering muon
	//TH1D* hLayerDelayHigh = new TH1D("hLayerDelayHigh","",nbins,tlayerDelayHigh.Min()*(1+(double)1/nbins),tlayerDelayHigh.Max()*(1+(double)1/nbins));
	TH1D* hLayerDelayHigh = new TH1D("hLayerDelayHigh","",nbins,lowerBound,upperBound);//tlayerDelay.Min()*(1+(double)1/nbins),tlayerDelay.Max()*(1+(double)1/nbins));
	hLayerDelayHigh->SetTitle("Delay time between muon PE detection, first secondary PE detection in layer, NPE>1000 (ns)");
	hLayerDelayHigh->GetXaxis()->SetTitle("Time difference (ns)");
	hLayerDelayHigh->GetYaxis()->SetTitle("Bin Members");
	hLayerDelayHigh->SetLineColor(kBlue);
	
	for(int m=0;m<layerDelayHigh.size();m++) hLayerDelayHigh->Fill(layerDelayHigh[m]);
	
//	TCanvas* c7 = new TCanvas("c7","c7",0,400,600,300);
	hLayerDelayHigh->Draw("same");
} else cout << "ERROR: NO HITS TO COMPARE FOR NPE>1000!" << endl;


//look at events which hit 4 slabs and no bars
if(slabDelay.size()>0){
	TVectorD tslabDelay(slabDelay.size(), &slabDelay[0]);
	//PMT Hit spectrum, bars in the different slabs as a triggering muon
	TH1D* hSlabDelay = new TH1D("hSlabDelay","",nbins,lowerBound,upperBound);//tslabDelay.Min()*(1+(double)1/nbins),tslabDelay.Max()*(1+(double)1/nbins));
	hSlabDelay->SetTitle("Delay time between slab muon PE detection, first secondary PE detection in bar, (ns)");
	hSlabDelay->GetXaxis()->SetTitle("Time difference (ns)");
	hSlabDelay->GetYaxis()->SetTitle("Bin Members");
	hSlabDelay->SetLineColor(kBlack);
	
	for(int m=0;m<slabDelay.size();m++) hSlabDelay->Fill(slabDelay[m]);
	
	TCanvas* s4 = new TCanvas("s4","s4",0,400,600,300);
	hSlabDelay->Draw();
} else cout << "ERROR: NO HITS TO COMPARE!" << endl;

if(slabDelayLow.size()>0){
	TVectorD tslabDelayLow(slabDelayLow.size(), &slabDelayLow[0]);
	//PMT Hit spectrum, bars in the different slabs as a triggering muon
	//TH1D* hSlabDelayLow = new TH1D("hSlabDelayLow","",nbins,tslabDelayLow.Min()*(1+(double)1/nbins),tslabDelayLow.Max()*(1+(double)1/nbins));
	TH1D* hSlabDelayLow = new TH1D("hSlabDelayLow","",nbins,lowerBound,upperBound);//tslabDelay.Min()*(1+(double)1/nbins),tslabDelay.Max()*(1+(double)1/nbins));
	hSlabDelayLow->SetTitle("Delay time between slab muon PE detection, first secondary PE detection in bar, NPE<100 (ns)");
	hSlabDelayLow->GetXaxis()->SetTitle("Time difference (ns)");
	hSlabDelayLow->GetYaxis()->SetTitle("Bin Members");
	hSlabDelayLow->SetLineColor(kRed);

	for(int m=0;m<slabDelayLow.size();m++) hSlabDelayLow->Fill(slabDelayLow[m]);

//	TCanvas* s5 = new TCanvas("s5","s5",0,400,600,300);
	hSlabDelayLow->Draw("same");
} else cout << "ERROR: NO HITS TO COMPARE FOR NPE<100!" << endl;

if(slabDelayMid.size()>0){
	TVectorD tslabDelayMid(slabDelayMid.size(), &slabDelayMid[0]);
	//PMT Hit spectrum, bars in the different slabs as a triggering muon
	//TH1D* hSlabDelayMid = new TH1D("hSlabDelayMid","",nbins,tslabDelayMid.Min()*(1+(double)1/nbins),tslabDelayMid.Max()*(1+(double)1/nbins));
	TH1D* hSlabDelayMid = new TH1D("hSlabDelayMid","",nbins,lowerBound,upperBound);//tslabDelay.Min()*(1+(double)1/nbins),tslabDelay.Max()*(1+(double)1/nbins));
	hSlabDelayMid->SetTitle("Delay time between slab muon PE detection, first secondary PE detection in bar, 1000>NPE>100 (ns)");
	hSlabDelayMid->GetXaxis()->SetTitle("Time difference (ns)");
	hSlabDelayMid->GetYaxis()->SetTitle("Bin Members");
	hSlabDelayMid->SetLineColor(kGreen);

	for(int m=0;m<slabDelayMid.size();m++) hSlabDelayMid->Fill(slabDelayMid[m]);

//	TCanvas* s6 = new TCanvas("s6","s6",0,400,600,300);
	hSlabDelayMid->Draw("same");
} else cout << "ERROR: NO HITS TO COMPARE FOR 1000>NPE>100!" << endl;

if(slabDelayHigh.size()>0){
	TVectorD tslabDelayHigh(slabDelayHigh.size(), &slabDelayHigh[0]);
	//PMT Hit spectrum, bars in the different slabs as a triggering muon
	//TH1D* hSlabDelayHigh = new TH1D("hSlabDelayHigh","",nbins,tslabDelayHigh.Min()*(1+(double)1/nbins),tslabDelayHigh.Max()*(1+(double)1/nbins));
	TH1D* hSlabDelayHigh = new TH1D("hSlabDelayHigh","",nbins,lowerBound,upperBound);//tslabDelay.Min()*(1+(double)1/nbins),tslabDelay.Max()*(1+(double)1/nbins));
	hSlabDelayHigh->SetTitle("Delay time between slab muon PE detection, first secondary PE detection in bar, NPE>1000 (ns)");
	hSlabDelayHigh->GetXaxis()->SetTitle("Time difference (ns)");
	hSlabDelayHigh->GetYaxis()->SetTitle("Bin Members");
	hSlabDelayHigh->SetLineColor(kBlue);
	
	for(int m=0;m<slabDelayHigh.size();m++) hSlabDelayHigh->Fill(slabDelayHigh[m]);
	
//	TCanvas* s7 = new TCanvas("s7","s7",0,400,600,300);
	hSlabDelayHigh->Draw("same");
} else cout << "ERROR: NO HITS TO COMPARE FOR NPE>1000!" << endl;

out->Write();

//clean up memory
otherBarHit.clear();
otherSlabHit.clear();
totalNumBarHitCount.clear();
totalNumSlabBarHitCount.clear();
layerDelay.clear();
layerDelayLow.clear();
layerDelayMid.clear();
layerDelayHigh.clear();
slabDelay.clear();
slabDelayLow.clear();
slabDelayMid.clear();
slabDelayHigh.clear();
barTime.clear();
slabTime.clear();
}

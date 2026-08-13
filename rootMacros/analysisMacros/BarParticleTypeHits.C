#include "TCanvas.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TChain.h"
#include "/media/ryan/Storage/computing/mqFullSim/include/mqROOTEvent.hh"
#include "TGraph.h"
#include "TVector.h"
#include "TVectorD.h"
#include "TVectorF.h"
#include "TH1.h"
#include "TH1F.h"
#include "TString.h"
#include "TChain.h"
#include "TMultiGraph.h"
using namespace std;


void BarNo12Hits(){

//retreive file

  TChain rootEvents("Events");
  TString fileDir = "/media/ryan/Storage/computing/mqFullSim/data/muonRockTest/";
  TString textFilepath = "/media/ryan/Storage/computing/mqFullSim/data/";
  TString xTitle = "Number of Muon-Generated Photons";
  rootEvents.Add(fileDir+"MilliQan.root");
  std::cout << "Reading file..." << std::endl;
  mqROOTEvent* myROOTEvent = new mqROOTEvent();
  rootEvents.SetBranchAddress("ROOTEvent", &myROOTEvent);
  Long64_t nentries=rootEvents.GetEntries();

//initialize variables
int numPMTHits=0;
int hitNumberScan = 0;
int count = 0;
int hitNumberCount = 0;
int bar12Hits=0;
std::ifstream barHitData;
///*

vector<int> evtNum;
vector<std::string> particleName;
vector<Double_t> particleEnergy;
vector<Double_t> evtTime;
vector<Double_t> muonHitsGamma;
vector<Double_t> muonHitsNeutron;
vector<Double_t> muonHitsElectron;
vector<Double_t> muonHitsOther;
//*/
//int evtNum[lines];
//std::string particleName[lines];
//double particleEnergy[lines];
//double evtTime[lines];
int a;
std::string b;
double c,d;
int lines=0;


barHitData.open(textFilepath+"particleHitData.txt");
while(true){
	barHitData >> a >> b >> c >> d;
	if(barHitData.eof()) break;
//	if(a>5000) break; //for debugging
	evtNum.push_back(a);
	particleName.push_back(b);
	particleEnergy.push_back(c);
	evtTime.push_back(d);
	cout << "Event read in: " << a << "	" << b << "	" << c << "	" << d << endl;
	lines++;
}
barHitData.close();

std::cout << "Number of lines in particleHitData.txt: " << lines << std::endl;
std::cout << "Last particle to hit detector is " << particleName[lines-1] << endl;

std::ofstream barWithPE;
barWithPE.open(textFilepath+"particleHitDataWithPE.txt");
//get data in file
  for (Long64_t i=0;i<nentries;i++) {
  //for (Long64_t i=0;i<5000;i++) { //for debugging
	rootEvents.GetEntry(i);
		hitNumberScan = myROOTEvent->GetBarHit();
		if(hitNumberScan > 0){
			if(hitNumberScan == 1){
			numPMTHits=myROOTEvent->GetPMTRHits()->size();
			for(int h=0;h<numPMTHits;h++){
				if(myROOTEvent->GetPMTRHits()->at(h)->GetPMTNumber()==12) bar12Hits++;
			}
//if (verbose==1)		cout << "Hit in event number " << evtNum[hitNumberCount] << " due to a " << particleName[hitNumberCount] << " of energy " << particleEnergy[hitNumberCount] << " MeV, at time " << evtTime[hitNumberCount] << " ns, with " << numPMTHits << " nPE detected total and " << bar12Hits << " detected in bar 12" << endl;
/*else */cout << evtNum[hitNumberCount] << " " << particleName[hitNumberCount] << " " << particleEnergy[hitNumberCount] << " " << evtTime[hitNumberCount] << " " << numPMTHits << " " << bar12Hits << endl;
		
		barWithPE << evtNum[hitNumberCount] << "	" << particleName[hitNumberCount] << "	" << particleEnergy[hitNumberCount] << "	" << evtTime[hitNumberCount] << "	" << numPMTHits << "	" << bar12Hits << endl;

		if(particleName[hitNumberCount]=="gamma") {
			muonHitsGamma.push_back(bar12Hits);
			muonHitsElectron.push_back(-1);
			muonHitsNeutron.push_back(-1);
			muonHitsOther.push_back(-1);
		} else if(particleName[hitNumberCount]=="e+" || particleName[hitNumberCount]=="e-"){
			muonHitsGamma.push_back(-1);
			muonHitsElectron.push_back(bar12Hits);
			muonHitsNeutron.push_back(-1);
			muonHitsOther.push_back(-1);
		} else if(particleName[hitNumberCount]=="neutron") {
			muonHitsGamma.push_back(-1);
			muonHitsElectron.push_back(-1);
			muonHitsNeutron.push_back(bar12Hits);
			muonHitsOther.push_back(-1);
		} else {
			muonHitsGamma.push_back(-1);
			muonHitsElectron.push_back(-1);
			muonHitsNeutron.push_back(-1);
			muonHitsOther.push_back(bar12Hits);
		}
			
		bar12Hits=0;
		hitNumberCount++;
			if(hitNumberCount%10==0) cout << "Finished analyzing hit number " << hitNumberCount << endl;
		
		}
			else {cout << hitNumberScan << " is more than 1 hit! Skipping for now." << endl;
				cout << "multiple particles which hit detector: ";
				for(int n=hitNumberCount;n<hitNumberCount+hitNumberScan;n++){ cout << particleName[n] << "	";
				muonHitsGamma.push_back(-1);
				muonHitsElectron.push_back(-1);
				muonHitsNeutron.push_back(-1);
				muonHitsOther.push_back(-1);
	
				cout << endl;
				}
				hitNumberCount+=hitNumberScan;
		}
//			if(i%1000==0) cout << "Finished analyzing event number " << i << endl;
   }
}

TVectorD tevtTime(evtTime.size(), &evtTime[0]);
TVectorD tmuonHitsGamma(muonHitsGamma.size(), &muonHitsGamma[0]);
TVectorD tmuonHitsElectron(muonHitsElectron.size(), &muonHitsElectron[0]);
TVectorD tmuonHitsNeutron(muonHitsNeutron.size(), &muonHitsNeutron[0]);
TVectorD tmuonHitsOther(muonHitsOther.size(), &muonHitsOther[0]);

TGraph *hGamma = new TGraph(tevtTime,tmuonHitsGamma);
TGraph *hElectron = new TGraph(tevtTime,tmuonHitsElectron);
TGraph *hNeutron = new TGraph(tevtTime,tmuonHitsNeutron);
TGraph *hOther = new TGraph(tevtTime,tmuonHitsOther);
hGamma->SetMarkerColor(8); //green
hGamma->SetMarkerStyle(kFullDotLarge);
hElectron->SetMarkerColor(9); //blue
hElectron->SetMarkerStyle(kFullDotLarge);
hNeutron->SetMarkerColor(12); //dark gray
hNeutron->SetMarkerStyle(kFullDotLarge);
hOther->SetMarkerColor(6); //pink
hOther->SetMarkerStyle(kFullDotLarge);


bool hasGamma=false;
bool hasElectron=false;
bool hasNeutron=false;
bool hasOther=false;

if(tmuonHitsGamma.Max()>-1) hasGamma=true;
if(tmuonHitsElectron.Max()>-1) hasElectron=true;
if(tmuonHitsNeutron.Max()>-1) hasNeutron=true;
if(tmuonHitsOther.Max()>-1) hasOther=true;

int x,y;
for(int m=0;m<evtTime.size();m++){
	hGamma->GetPoint(m,x,y);
	if(y==-1) hGamma->RemovePoint(m);
	hElectron->GetPoint(m,x,y);
	if(y==-1) hElectron->RemovePoint(m);
	hNeutron->GetPoint(m,x,y);
	if(y==-1) hNeutron->RemovePoint(m);
	hOther->GetPoint(m,x,y);
	if(y==-1) hOther->RemovePoint(m);
}


TMultiGraph* hnPE12 = new TMultiGraph();
if(hasGamma) hnPE12->Add(hGamma,"p");
if(hasElectron) hnPE12->Add(hElectron,"p");
if(hasNeutron) hnPE12->Add(hNeutron,"p");
if(hasOther) hnPE12->Add(hOther,"p");

TCanvas* c1 = new TCanvas("c1","c1",0,400,600,300);
hnPE12->Draw("AP");

muonHitsGamma.clear();
muonHitsElectron.clear();
muonHitsNeutron.clear();
muonHitsOther.clear();
evtNum.clear();
particleName.clear();
particleEnergy.clear();
evtTime.clear();

}

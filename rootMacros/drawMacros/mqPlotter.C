#include "TCanvas.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TFile.h"
#include <iostream>
#include "TMath.h"
#include "TChain.h"
#include "TGraph.h"
#include "TVector.h"
#include "TVectorF.h"
#include "TH1.h"
#include "TH2.h"
#include "TH2D.h"
#include "TH1F.h"
#include "TString.h"
#include "TChain.h"
using namespace std;
void mqPlotter(TString source, int nbins, double xlow, double xup, TString legpos, int chan0=0, int chan1=1, int chan2=2, int chan3=3){

TFile *tf = TFile::Open(source+".root");
TTree* Events = (TTree*)tf->Get("Events");
TCanvas* c1 = new TCanvas("c1","c1",2560,1440);
gStyle->SetOptStat(0);

TString inbins,ixlow,ixup;
TString ichan0,ichan1,ichan2,ichan3;

inbins.Form("%d",nbins);
ixlow.Form("%f",xlow);
ixup.Form("%f",xup);

ichan0.Form("%i",chan0);
ichan1.Form("%i",chan1);
ichan2.Form("%i",chan2);
ichan3.Form("%i",chan3);

//plot the vmax
Events->Draw("Max$(digitizers.waveform[0]["+ichan0+"])>>h0("+inbins+","+ixlow+","+ixup+")","","");
Events->Draw("Max$(digitizers.waveform[0]["+ichan1+"])>>h1("+inbins+","+ixlow+","+ixup+")","","");
Events->Draw("Max$(digitizers.waveform[0]["+ichan2+"])>>h2("+inbins+","+ixlow+","+ixup+")","","");
Events->Draw("Max$(digitizers.waveform[0]["+ichan3+"])>>h3("+inbins+","+ixlow+","+ixup+")","","");

TH1D* h0 = (TH1D*)gDirectory->Get("h0");
TH1D* h1 = (TH1D*)gDirectory->Get("h1");
TH1D* h2 = (TH1D*)gDirectory->Get("h2");
TH1D* h3 = (TH1D*)gDirectory->Get("h3");

//h1->Scale(1000.);
//h2->Scale(1000.);
//h0->Scale(1000.);
//h3->Scale(1000.);

double x1,y1,x2,y2;
//make legend
if(legpos=="topR"){
//top right
x1=0.67;
y1=0.6;
x2=0.87;
y2=0.8;
}
if(legpos=="midR"){
//mid right
x1=0.67;
y1=0.4;
x2=0.87;
y2=0.6;
}
if(legpos=="botR"){
//bottom right
x1=0.67;
y1=0.17;
x2=0.87;
y2=0.37;
}
if(legpos=="topL"){
//top left
x1=0.12;
y1=0.67;
x2=0.27;
y2=0.87;
}
auto legend = new TLegend(x1,y1,x2,y2);
legend->AddEntry(h0,"Ch0","l");
legend->AddEntry(h1,"Ch1","l");
legend->AddEntry(h2,"Ch2","l");
legend->AddEntry(h3,"Ch3","l");
legend->SetBorderSize(0);

h0->SetTitle(source);
h0->GetXaxis()->SetTitle("vMax (mV)");
h0->GetYaxis()->SetTitle("Bin Content");

h0->SetLineColor(kRed);
h1->SetLineColor(kBlue);
h2->SetLineColor(kGreen+1);
h3->SetLineColor(kBlack);
h0->SetLineWidth(2);
h1->SetLineWidth(2);
h2->SetLineWidth(2);
h3->SetLineWidth(2);

h0->Draw("");
h1->Draw("same");
h2->Draw("same");
h3->Draw("same");
legend->Draw("same");
//c1->SetLogy();

TFile *sv = new TFile("histlist.root","RECREATE");
h0->Write();
h1->Write();
h2->Write();
h3->Write();
}




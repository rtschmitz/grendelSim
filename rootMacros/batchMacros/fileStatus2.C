#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <algorithm>
#include <string>


using namespace std;

void fileStatus(){
  std::ifstream input,output;
  std::string mass,charge,files;
  vector<string> imass,icharge,ifiles;
  vector<string> omass,ocharge,ofiles;
  char d1('/');
  char d2(' ');
  //input.open("input.txt");
  //output.open("output.txt");
  input.open("/homes/schmitz/milliqanTrajectories/hadoop/cms/store/user/ryan/milliqan/milliq_mcgen/mcp_txt/v8_v1_save2m_full/content.txt",std::ifstream::in);
  output.open("/homes/schmitz/mcpSignal_fullsim/hadoop/cms/store/user/ryan/beammcp_fullSim_v1/mcp_v8_v1_save2m_full_fullSim_v1/content.txt",std::ifstream::in);
  	
	std::getline(input,mass,d1);
	std::getline(input,charge,d2);
	std::getline(input,files);
  while(input){
	mass.erase(0,2);
	replace(mass.begin(),mass.end(),'p','.');
	charge.erase(0,2);
	replace(charge.begin(),charge.end(),'p','.');
	files.erase(0,2);
	files.erase(files.end()-8,files.end());
  //   cout << "mass: " << mass << " charge: " << charge << " filenum: " << files << endl;
	imass.push_back(mass);
	icharge.push_back(charge);
	ifiles.push_back(files);
  	std::getline(input,mass,d1);
	std::getline(input,charge,d2);
	std::getline(input,files);
  }
  	std::getline(output,mass,d1);
	std::getline(output,charge,d2);
	std::getline(output,files);
  while(output){
	mass.erase(0,2);
	replace(mass.begin(),mass.end(),'p','.');
	charge.erase(0,2);
	replace(charge.begin(),charge.end(),'p','.');
	files.erase(0,2);
	files.erase(files.end()-8,files.end());
//     cout << "mass: " << mass << " charge: " << charge << " filenum: " << files << endl;
	omass.push_back(mass);
	ocharge.push_back(charge);
	ofiles.push_back(files);
  	std::getline(output,mass,d1);
	std::getline(output,charge,d2);
	std::getline(output,files);
  }
  
    cout << "inputvector size: " << imass.size() << endl;
    cout << "outputvector size: " << omass.size() << endl;

   Double_t gm[500], gc[500];
   Double_t ym[500], yc[500];
   Double_t rm[500], rc[500];

   
   int g=0;
   int y=0;
   int r=0;


   bool match=false;
   for(int i=0;i<imass.size();i++){
	for(int o=0;o<omass.size();o++){
		if(imass[i]==omass[o] && icharge[i]==ocharge[o]){
			if(ifiles[i]==ofiles[o]) {cout << "match for mass " << imass[i] << " charge " << icharge[i] << endl;
						  gm[g]=std::atof(imass[i].c_str()); gc[g]=std::atof(icharge[i].c_str()); g++;}
			else {cout << "no match for mass " << imass[i] << " charge " << icharge[i] << " but incomplete data" << endl;
				ym[y]=std::atof(imass[i].c_str()); yc[y]=std::atof(icharge[i].c_str()); y++;}
		match=true;
		break;
		}
	}
	if(!match) {cout << "no match at all! " << endl;
		rm[r]=std::atof(imass[i].c_str()); rc[r]=std::atof(icharge[i].c_str());
		//cout << "test double division: " << rm[r]/rc[r] << endl;
		r++;}
	match=false;
   }
  TCanvas *c1 = new TCanvas("c1","Spectrum Of Data",0,400,600,300);
  c1->SetLogx();
  c1->SetLogy();
   TGraph* green = new TGraph(g,gm,gc);
   TGraph* yellow = new TGraph(y,ym,yc);
   TGraph* red = new TGraph(r,rm,rc);
   green->SetMarkerColor(kGreen);
   yellow->SetMarkerColor(kYellow);
   red->SetMarkerColor(kRed);
   green->SetMarkerStyle(21);
   yellow->SetMarkerStyle(21);
   red->SetMarkerStyle(21);
   green->SetTitle("mCP Signal Data Coverage");
   green->GetXaxis()->SetTitle("mass (GeV)");
   green->GetYaxis()->SetTitle("charge (Q/e)");
   green->Draw("AP");
   yellow->Draw("P same");
   red->Draw("P same");

   TLegend* leg = new TLegend(0.7,0.2,0.9,0.35);
   leg->AddEntry(green,"Full Data","p");
   leg->AddEntry(yellow,"Partial Data","p");
   leg->AddEntry(red,"No Data","p");
   leg->Draw();
 // TH1F *hMEDep = new TH1F("hMECount","PMT Signal Area, CloseMid",100,0,50000);
//  hMECount->GetXaxis()->SetTitle("Pulse Area (nVs");
//  hMECount->GetYaxis()->SetTitle("Bin Members");

//  ch.Draw("area_CH1>>hMECount");
  
}

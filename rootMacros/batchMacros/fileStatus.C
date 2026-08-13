#include "TCanvas.h"
#include "TGraph.h"
#include "TTree.h"
#include "TFile.h"
#include <iostream>
#include <algorithm>
#include <string>


using namespace std;

void fileStatus(){
  std::ifstream input,fullinput,output;
  std::ofstream outfile;
  outfile.open("unfinishedmid.txt");
  std::string mass,charge,files,fullpath;
  vector<string> smass,imass,scharge,icharge,ifiles,ifullpath;
  vector<string> omass,ocharge,ofiles;
  char d1('/');
  char d2(' ');
  input.open("lowmasscontent.txt");
  fullinput.open("fulllowmasscontent.txt");
  output.open("onlycmsxoutput.txt");
  //input.open("/cms27r0/schmitz/milliqanTrajectories/hadoop/cms/store/user/ryan/milliqan/milliq_mcgen/mcp_txt/v8_v1_save2m_full/content.txt",std::ifstream::in);
  //output.open("/cms27r0/schmitz/mcpSignal_fullsim/hadoop/cms/store/user/ryan/beammcp_fullSim_v1/mcp_v8_v1_save2m_full_fullSim_v1/content.txt",std::ifstream::in);
  	std::getline(fullinput,fullpath);
  while(fullinput){
	ifullpath.push_back(fullpath);
  	std::getline(fullinput,fullpath);
	}

	std::getline(input,mass,d1);
	std::getline(input,charge,d2);
	std::getline(input,files);
  while(input){
	smass.push_back(mass);
	scharge.push_back(charge);
	mass.erase(0,2);
	replace(mass.begin(),mass.end(),'p','.');
	charge.erase(0,2);
	replace(charge.begin(),charge.end(),'p','.');
	files.erase(0,2);
	files.erase(files.end()-8,files.end());
     cout << "mass: " << mass << " charge: " << charge << " filenum: " << files << endl;
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
     cout << "mass: " << mass << " charge: " << charge << " filenum: " << files << endl;
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
						  match=true;
						  gm[g]=std::atof(imass[i].c_str()); gc[g]=std::atof(icharge[i].c_str()); g++;}
			else {cout << "no match for mass " << imass[i] << " charge " << icharge[i] << " but incomplete data" << endl;
				ym[y]=std::atof(imass[i].c_str()); yc[y]=std::atof(icharge[i].c_str()); y++;}
		break;
		}
	}
	if(!match) {cout << "no match at all! " << endl;
		rm[r]=std::atof(imass[i].c_str()); rc[r]=std::atof(icharge[i].c_str());
		//if ((rc[r]<0.011 || rm[r] > 6) && rm[r]!=3.5 && rm[r]!=4 && rm[r]>0.1 && rc[r]!=0.006 && rc[r]!=0.008){
		if ((rc[r]<=0.02 && rc[r]>=0.01 && (rm[r]>=0.3 && rm[r]<2)) || ((rc[r]>=0.02 && rc[r]<=0.07) && (rm[r]==1.6 || rm[r]==1.8))){
		std::string s2 = smass[i]+"/"+scharge[i]+"/";
		for(int p=0;p<ifullpath.size();p++){
			if (ifullpath[p].std::find(s2) != std::string::npos) outfile << imass[i] << " " << icharge[i] << " " << ifullpath[p] << endl;
		} //write to output

		//cout << "test double division: " << rm[r]/rc[r] << endl;
		r++;}
	}
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

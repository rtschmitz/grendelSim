#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <TSystem.h>
#include <iostream>
#include <fstream>



double dydeta(double eta, double pt, double mass) {
	double num = pt * std::cosh(eta);
	double denom = std::sqrt(mass * mass + pt*pt*std::cosh(eta)*std::cosh(eta));
	return num / denom;
}


double integrateDyDeta(double eta1, double eta2, double pt, double mass, int nSteps = 300) {
	double step = (eta2 - eta1)/nSteps;
	double sum = 0.5 * (dydeta(eta1, pt, mass) + dydeta(eta2, pt, mass));

	for(int i = 1; i < nSteps; i++){
		double eta_step = eta1 + i * step;
		sum += dydeta(eta_step, pt, mass);
	}
	return sum * step;
}


double correctionFactor(double pt, double mass, double I_range){
	double val = integrateDyDeta(-I_range, I_range, pt, mass);
	double denom = 2.0 * I_range * dydeta(0.11, pt, mass);
	return val/denom;
}




//void extract_values_auto(const char* mass, const char* mode, const char* nevents) {
void extract_values_auto(const char* mass, const char* mode) {
  // Set up input and output paths
  TString inpath = TString::Format("/net/cms26/cms26r0/dgimani/milliq_mcgen/propagated/c_0p1/%s/%s/", mass, mode);
  TString outpath = TString::Format("/homes/dgimani/milliQanSim/inputData/Run3_G4_mcp_inputs/c_0p1/%s/%s/", mass, mode);
  TString outputFile = TString::Format("%soutput_mcp_%s_%s.txt", outpath.Data(), mass, mode);

  // Open output file
  std::ofstream file(outputFile);
  if (!file.is_open()) {
    std::cerr << "Error opening output file: " << outputFile << std::endl;
    return;
  }

  // Open the directory and retrieve file list
  TSystemDirectory dir(inpath, inpath);
  TList *files = dir.GetListOfFiles();
  if (!files) {
    std::cerr << "Directory not found or empty: " << inpath << std::endl;
    return;
  }

  TIter next(files);
  TSystemFile *fileObj;
  int nFiles = 0;
  int entryTot = 0;

  // Loop over all files in the directory
  while ((fileObj = (TSystemFile*)next())) {
    TString fileName = fileObj->GetName();
    if (!fileObj->IsDirectory() && fileName.EndsWith(".root")) {
      TString fullFilePath = inpath + fileName;
      std::cout << "Opening file: " << fullFilePath << std::endl;

      TFile *f = TFile::Open(fullFilePath);
      if (!f || f->IsZombie()) {
        std::cerr << "Failed to open or zombie file: " << fullFilePath << std::endl;
        if (f) f->Close();
        continue;
      }

      TTree *tree = (TTree*)f->Get("Events");
      if (!tree) {
        std::cerr << "No tree named 'Events' found in file: " << fullFilePath << std::endl;
        f->Close();
        continue;
      }

      // Set up branch addresses
      Bool_t does_hit_p;
      Bool_t does_hit_m;
      Int_t decay_mode;
      TVector3 *hit_p_xyz = nullptr;
      TLorentzVector *hit_p_p4 = nullptr;
      TVector3 *hit_m_xyz = nullptr;
      TLorentzVector *hit_m_p4 = nullptr;
      Float_t xsec = 0.0f;
      Float_t BR_q1 = 0.0f;
      Float_t filter_eff = 0.0f;
      Float_t LUMI = 140000;
      Float_t weight = 0.0f;
      Float_t parent_Pt;
      Float_t parent_M;
      UInt_t n_events_total;  //The total num events generated for this decay mode and mass       //= std::stof(nevents);
      UInt_t event = 0;
      Double_t sim_q;
      double X = 0.0;
      double Y = 0.0;
      double Z = 0.0;
      double Px = 0.0;
      double Py = 0.0;
      double Pz = 0.0;
      double mass = 0.0;
      double cFactor = 1.0;

      tree->SetBranchAddress("does_hit_p", &does_hit_p);
      tree->SetBranchAddress("does_hit_m", &does_hit_m);
      tree->SetBranchAddress("hit_p_xyz", &hit_p_xyz);
      tree->SetBranchAddress("hit_p_p4", &hit_p_p4);
      tree->SetBranchAddress("hit_m_xyz", &hit_m_xyz);
      tree->SetBranchAddress("hit_m_p4", &hit_m_p4);
      tree->SetBranchAddress("parent_p4.fCoordinates.fPt", &parent_Pt);
      tree->SetBranchAddress("parent_p4.fCoordinates.fM", &parent_M);
      tree->SetBranchAddress("sim_q", &sim_q);
      tree->SetBranchAddress("xsec", &xsec);
      tree->SetBranchAddress("BR_q1", &BR_q1);
      tree->SetBranchAddress("filter_eff", &filter_eff);
      tree->SetBranchAddress("decay_mode", &decay_mode);
      tree->SetBranchAddress("n_events_total", &n_events_total);
      tree->SetBranchAddress("event", &event);
      //tree->SetBranchAddress("weight", &weight);

      int nEntries = tree->GetEntries();
      for (int j = 0; j < nEntries; j++) {
        tree->GetEntry(j);
        double charge = sim_q;
	bool hit_p = does_hit_p;
	bool hit_m = does_hit_m;
	//std::cout << "hit_p: " << hit_p << std::endl;
	//std::cout << "hit_m: " << hit_m << std::endl;
	if(hit_p){
        	mass = hit_p_p4->M();
		X = hit_p_xyz->X();
		Y = hit_p_xyz->Y();
		Z = hit_p_xyz->Z();
		Px = hit_p_p4->Px();
		Py = hit_p_p4->Py();
		Pz = hit_p_p4->Pz();
	}
	else if(hit_m){
        	mass = hit_m_p4->M();
		X = hit_m_xyz->X();
		Y = hit_m_xyz->Y();
		Z = hit_m_xyz->Z();
		Px = hit_m_p4->Px();
		Py = hit_m_p4->Py();
		Pz = hit_m_p4->Pz();
	}

	if(decay_mode >= 11 && decay_mode <=15){
		cFactor = correctionFactor(parent_Pt, parent_M, 1.2);
	}
	else {
		cFactor = 1.0;
	}
        weight = xsec * BR_q1 * filter_eff * LUMI * cFactor/ n_events_total;
	if(weight > 200){
		std::cout << "Event: " << event << std::endl;
		std::cout << "Decay mode: " << decay_mode << std::endl;
		std::cout << "xsec: " << xsec << std::endl;
		std::cout << "BR_q1: " << BR_q1 << std::endl;
		std::cout << "filter_eff: " << filter_eff << std::endl;
		std::cout << "LUMI: " << LUMI << std::endl;
		std::cout << "n_events_total: " << n_events_total << std::endl;
		std::cout << "weight: " << weight << std::endl;
		std::cout << "decay mode: " << decay_mode << std::endl;
		std::cout << "parent_M: " << parent_M << std::endl;
		std::cout << "parent_Pt: " << parent_Pt << std::endl;
		std::cout << "cFactor: " << cFactor << std::endl;
	} 
	file << event << " " << decay_mode << " " << charge << " " << mass << " " << Z << " " << Y << " " << -1 * X << " " << Pz << " " << Py << " " << -1 * Px << " " << weight << std::endl;
      }

      std::cout << "File had " << nEntries << " entries" << std::endl;
      entryTot += nEntries;
      nFiles++;

      f->Close();
    }
  }

  // Close output file
  file.close();
  std::cout << "Extraction complete, " << nFiles << " files converted with " << entryTot << " total entries. Results saved to " << outputFile << std::endl;
}




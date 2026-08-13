#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <iostream>
#include <fstream>

void extract_values() {
  //TString mode="dy";
  TString mode="eta";
  TString mass="m_0p1";
  //TString path = "/net/cms26/cms26r0/hmei/milliqan/muons/05252023/"+mode+"/postsim_v1_save2m/";
  TString inpath = "/net/cms26/cms26r0/dgimani/milliq_mcgen/propagated/" + mass + "/" + mode + "/";

  //TString/ outputFile = "output_"+mode+"_muons.txt";
  TString outpath = "/homes/dgimani/milliQanSim/inputData/Run3_mcp_inputs/";
  TString outputFile = outpath+mass+"/"+mode+"/output_mcp_"+mass+"_"+mode+".txt";

  std::ofstream file(outputFile);
  int nFiles=0;
  int entryTot=0;
  for (int i = 1; i <= 48; i++) { // change if the number of files you want to loop over changes
    TString fileName = Form("%spropagated_output_%d.root", inpath.Data(),i);
    //TString fileName = "propagated_output_1.root";
    std::cout << "Opening file: " << inpath + fileName << std::endl;
    TFile *f = TFile::Open(fileName);
    if (!f) continue;
    if (f->IsZombie()) {f->Close(); std::cout << "is zombie!" << std::endl; continue;}
    if ( !f->Get("Events")) {std::cout << "file is null! No event content found" << std::endl; continue;}

    TTree *tree = (TTree *)f->Get("Events"); // Change to the name of your tree

    TVector3 *hit_p_xyz = nullptr;
    TLorentzVector *hit_p_p4 = nullptr;
    Float_t weight;
    Double_t sim_q;

    tree->SetBranchAddress("hit_p_xyz", &hit_p_xyz);
    tree->SetBranchAddress("hit_p_p4", &hit_p_p4);
    tree->SetBranchAddress("sim_q", &sim_q);
    tree->SetBranchAddress("weight", &weight);

    int nEntries = tree->GetEntries();

    for (int j = 0; j < nEntries; j++) {
      tree->GetEntry(j);
      double charge = sim_q;
      double mass = hit_p_p4->M();
      double X = hit_p_xyz->X();
      double Y = hit_p_xyz->Y();
      double Z = hit_p_xyz->Z();
      double Px = hit_p_p4->Px();
      double Py = hit_p_p4->Py();
      double Pz = hit_p_p4->Pz();

      file << charge << " " << mass << " " << Z << " " << Y << " " << -1*X << " " << Pz << " " << Py << " " << -1*Px << " " << weight << std::endl;
      //file << charge << " " << mass << " " << X << " " << Y << " " << Z << " " << Px << " " << Py << " " << Pz << " " << weight << std::endl;
    }
    std::cout << "File had " << nEntries << " entries" << std::endl;    
    nFiles++;
    entryTot+=nEntries;
    f->Close();
  }

  file.close();
  std::cout << "Extraction complete, " << nFiles << " files converted with " << entryTot << " total entries. Results saved to " << outputFile << std::endl;
}


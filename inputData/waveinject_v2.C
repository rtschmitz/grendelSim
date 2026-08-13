#include "TCanvas.h"
#include "TTree.h"
#include "TGaxis.h"
#include "TStyle.h"
#include "TFile.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TChain.h"
#include "/homes/dgimani/milliQanSim/include/mqROOTEvent.hh"
#include "/homes/dgimani/milliQanSim/include/mqPMTRHit.hh"
//#include "milliQanSim/include/mqROOTEvent.hh"
//#include "milliQanSim/include/mqPMTRHit.hh"
#include "TGraph.h"
#include "TVector.h"
#include "TVectorD.h"
#include "TVectorF.h"
#include "TH1.h"
#include "TH1F.h"
#include "TString.h"
#include "TChain.h"
#include "TMultiGraph.h"
#include <vector>
#include <map>
#include <algorithm>
R__LOAD_LIBRARY(/homes/dgimani/milliQanSim/build/libMilliQanCore.so)
#include "TSystem.h"
using namespace std;


int simToDataPMT(int simChannel) {
    if (simChannel == 77) return 68;
    else if (simChannel == 78) return 70;
    else if (simChannel == 79) return 72;
    else if (simChannel == 81) return 69;
    else if (simChannel == 82) return 71;
    else if (simChannel == 83) return 73;
    else if (simChannel == 97) return 74;
    else if (simChannel == 96) return 75;

    int layerNumber = simChannel / 216;
    simChannel = simChannel % 216;

    if (simChannel <= 4) {
        return (simChannel + 11) + layerNumber * 16;
    } else if (simChannel <= 12) {
        return simChannel - 1 + layerNumber * 16;
    } else if (simChannel <= 16) {
        return simChannel - 13 + layerNumber * 16;
    } else {
        std::cerr << "Error: simChannel out of range" << std::endl;
        return -1;
    }
}

void waveinject_v2(TString inputFile, TString outputFile, TString waveformFile) {

   std::cout << "Injecting file " << inputFile << std::endl;
   std::cout << "Outputting file " << outputFile << std::endl;
   std::cout << "Using waveform template " << waveformFile << std::endl;

   TChain rootEvents("Events");
   rootEvents.Add(inputFile);
   mqROOTEvent* myROOTEvent = new mqROOTEvent();
   rootEvents.SetBranchAddress("ROOTEvent", &myROOTEvent);
   TFile* outfile = new TFile(outputFile, "RECREATE");
   
   const int nDigitizers = 5;
   const int nChannelsPerDigitizer = 16;
   const int nBins = 1024;
   double binWidth = 2.5;
   double rms_noise = 1;

   Float_t waveform[nDigitizers][nChannelsPerDigitizer][nBins] = {{{0}}};
   Double_t eventWeight = -1;

   TTree* injectedTree = new TTree("Events", "Tree with digitizer waveform data");
   injectedTree->Branch("waveform", waveform, Form("waveform[%d][%d][%d]/F", nDigitizers, nChannelsPerDigitizer, nBins));
   injectedTree->Branch("eventWeight", &eventWeight, "eventWeight/D");

   TF1 *fit = new TF1("fit", "gaus(0)", 0, 5000);
   fit->SetParameter(0, 7.23967e-02);
   fit->SetParameter(1, 1.48539e+03);
   fit->SetParameter(2, 2.90976e+02);

   TFile* f = new TFile(waveformFile);
   TH1F* pulse_shape = (TH1F*)f->Get("average_waveform");

   // Calibration array (scaled by dividing by 11)
   std::vector<double> cali = {9.7, 8.4, 5.0, 3.6, 8.2, 8.2, 6.8, 4.6, 8.7, 7.1, 6.4, 7.5, 8.7, 11.8, 9.7, 8.1,
                               8.6, 9.1, 6.4, 7.5, 9.8, 8.8, 11, 6.6, 8.6, 11, 7.3, 8.3, 2.6, 6.2, 7.0, 7.2,
                               8.7, 7.7, 6.8, 7.8, 8.8, 8.5, 3.5, 8.2, 7.6, 6.8, 9.5, 8.5, 6.3, 6.7, 7.6, 7.9,
                               9.0, 8.3, 8.3, 6.9, 6.0, 8.0, 5.5, 5.5, 6.6, 8.1, 8.4, 8.7, 9.2, 7.6, 5.9, 8.2};
   for (auto& cal : cali) cal /= 11.0; // Divide each value by 11

std::vector<double> maxValues = {
    1250, 1250, 1250, 1250, 1025, 1075, 1070, 980, 1250, 1250,
    1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1250, 1080,
    1070, 1060, 1080, 1075, 1250, 1180, 1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250, 1250, 1250, 1250, 1085, 1075, 1080,
    1250, 1250, 1075, 1250, 1250, 1250, 1250, 1250, 1250, 1250,
    1250, 1250, 1080, 1250, 1080, 1080, 1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
    1250, 1250, 1250, 1250,
};

   TRandom3 randGen(2004);
   Long64_t nentries = rootEvents.GetEntries();
   //std::cout << "Entries: " << nentries << std::endl;

   for (Long64_t i = 0; i < nentries; i++) {
      if (i % (nentries / 100) == 0) //std::cout << "Processing Event " << i << "..." << std::endl;
      rootEvents.GetEntry(i);
      memset(waveform, 0, sizeof(waveform));

      eventWeight = myROOTEvent->GetEventWeight();

      std::map<int, std::vector<mqPMTRHit*>> pmtHitsMap;

      for (int j = 0; j < myROOTEvent->GetPMTRHits()->size(); j++) {
         mqPMTRHit* PMTRHit = myROOTEvent->GetPMTRHits()->at(j);
         int PMT_number = PMTRHit->GetPMTNumber();
         pmtHitsMap[PMT_number].push_back(PMTRHit);
      }

      for (const auto& [PMT_number, hits] : pmtHitsMap) {
         int remappedPMT = simToDataPMT(PMT_number);
         if (remappedPMT == -1) continue;
         if (remappedPMT == 24) remappedPMT = 78;
         if (remappedPMT == 25) remappedPMT = 79;
         if ((remappedPMT == 24) || (remappedPMT == 25)) cout << remappedPMT << endl;

         int digitizer = remappedPMT / nChannelsPerDigitizer;
         int channel = remappedPMT % nChannelsPerDigitizer;

         // Extract and sort hit times
         std::vector<double> hitTimes;
         for (const auto& hit : hits) {
            if(hit->GetFirstHitTime() > 500.0 ) continue;
	    hitTimes.push_back(hit->GetFirstHitTime());
         }


	 if(hitTimes.size()==0) continue;
         std::sort(hitTimes.begin(), hitTimes.end());

         // Calculate median hit time
/*
	 double median_hit_time;
         size_t size = hitTimes.size();
         if (size % 2 == 0) {
            median_hit_time = 0.5 * (hitTimes[size / 2 - 1] + hitTimes[size / 2]);
         } else {
            median_hit_time = hitTimes[size / 2];
         }
*/
         double initial_hit_time = hitTimes[0];
	 double calibration = (remappedPMT < 64) ? cali[remappedPMT] : 0.682;
	 if (hits.size() > 5000) {
            double areaSum = 0.0;
            for (size_t k = 0; k < hits.size(); ++k) {
                if(randGen.Uniform() <= calibration) areaSum += fit->GetRandom();
            }

            TH1F* new_waveform = (TH1F*)pulse_shape->Clone();
            //new_waveform->Scale(areaSum * (1077.24 / 828.03) / new_waveform->Integral(480, 640));
            
	    // Scale only the bins within [500, 660]
            double scaleFactor = areaSum / (828.03) * (44.8573/59.0878); //sample area, then scale by the pulse height for SPEs in data
            for (int bin = 500; bin <= 660; ++bin) {
                double binContent = new_waveform->GetBinContent(bin);
                new_waveform->SetBinContent(bin, binContent * scaleFactor);
            }

	    int integer_shift = static_cast<int>(initial_hit_time / binWidth);
            double fractional_shift = fmod(initial_hit_time, binWidth) / binWidth;

	    //uses median time rather than first for large hits
	    //int integer_shift = static_cast<int>(median_hit_time / binWidth);
            //double fractional_shift = fmod(median_hit_time, binWidth) / binWidth;

            for (int bin = 0; bin < nBins; ++bin) waveform[digitizer][channel][bin] = 0.0;

            for (int bin = 0; bin < nBins; ++bin) {
                int shifted_bin = bin - integer_shift;
                if (shifted_bin >= nBins || shifted_bin < 1) continue;

                double value = (1.0 - fractional_shift) * new_waveform->GetBinContent(shifted_bin) +
                               fractional_shift * new_waveform->GetBinContent(shifted_bin - 1);
                double noise = randGen.Gaus(0, rms_noise);
                waveform[digitizer][channel][bin] += (value + noise);

                if (waveform[digitizer][channel][bin] > maxValues[remappedPMT]) waveform[digitizer][channel][bin] = maxValues[remappedPMT];
                if (waveform[digitizer][channel][bin] < -50) waveform[digitizer][channel][bin] = -50;
            }

            for (int bin = 0; bin < integer_shift; ++bin) waveform[digitizer][channel][bin] = 0.0;

            delete new_waveform;
         } else {
            for (mqPMTRHit* PMTRHit : hits) {
               if(randGen.Uniform() > calibration) continue;
               double initial_hit_time = PMTRHit->GetFirstHitTime();
	       if(initial_hit_time>500) {continue;}
               TH1F* new_waveform = (TH1F*)pulse_shape->Clone();
               double event_area = fit->GetRandom();

               //new_waveform->Scale(event_area * (1077.24 / 828.03) / new_waveform->Integral(480, 640));
	       
	       // Scale only the bins within [500, 660]
               double scaleFactor = event_area / (828.03) * (44.8573/59.0878);
               for (int bin = 500; bin <= 660; ++bin) {
                   double binContent = new_waveform->GetBinContent(bin);
                   new_waveform->SetBinContent(bin, binContent * scaleFactor);
               }

               int integer_shift = static_cast<int>(initial_hit_time / binWidth);
	       double fractional_shift = fmod(initial_hit_time, binWidth) / binWidth;

               //for (int bin = 0; bin < nBins; ++bin) waveform[digitizer][channel][bin] = 0.0;

               //for (int bin = 500; bin < 660; ++bin) {
               for (int bin = 0; bin < nBins; ++bin) {
                  int shifted_bin = bin - integer_shift;
                  if (shifted_bin >= nBins || shifted_bin < 1) continue;

                  double value = (1.0 - fractional_shift) * new_waveform->GetBinContent(shifted_bin) +
                                 fractional_shift * new_waveform->GetBinContent(shifted_bin - 1);
		  waveform[digitizer][channel][bin] += (value);

               }

               for (int bin = 0; bin < integer_shift; ++bin) waveform[digitizer][channel][bin] = 0.0;

               delete new_waveform;
            }
                  for (int bin = 0; bin < nBins; bin++) {
		        if(bin<500 || bin>660) waveform[digitizer][channel][bin]=0;
                        double noise = randGen.Gaus(0, rms_noise);
			waveform[digitizer][channel][bin] += noise;
                  	if (waveform[digitizer][channel][bin] > maxValues[remappedPMT]) waveform[digitizer][channel][bin] = maxValues[remappedPMT];
                  	if (waveform[digitizer][channel][bin] < -50) waveform[digitizer][channel][bin] = -50;
		  }

	   }
      }
      injectedTree->Fill();
   }

      cout << "final" << endl;
   outfile->cd();
   injectedTree->Write();
   f->Close();
   outfile->Close();
}


/*
 * mqHistoManager.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "mqHistoManager.hh"

#include "G4UnitsTable.hh"
#include "G4ThreeVector.hh"
#include "G4EventManager.hh"
//#ifdef G4ANALYSIS_USE_ROOT
#include "TFile.h"
#include "TTree.h"
//#include "Cintex/Cintex.h"
#include "TSystem.h"

//#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqHistoManager::mqHistoManager()
: rootFile(0),ntuplEvent(0), myROOTEvent(0)
{

  // ntuple

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqHistoManager::~mqHistoManager()
{
//#ifdef G4ANALYSIS_USE
    if ( rootFile ) delete rootFile;
//#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqHistoManager::book(G4String outfile)
{
//#ifdef G4ANALYSIS_USE_ROOT

 // Creating a tree container to handle histograms and ntuples.
 // This tree is associated to an output file.
 //

 G4cout << "MilliQan> Create ROOT file : MilliQan.root" << G4endl;

 G4String fileName = outfile + "MilliQan.root";
 rootFile = new TFile(fileName,"RECREATE");
 if(!rootFile) {
   G4cout << " mqHistoManager::book :"
          << " problem creating the ROOT TFile "
          << G4endl;
   return;
 }


 //info about event statistics
 ntuplEvent = new TTree("Events", "MilliQan event statistics");


 myROOTEvent = new mqROOTEvent();
 ntuplEvent->Branch("ROOTEvent", &myROOTEvent);



 G4cout << "\n----> Histogram file is opened in " << fileName << G4endl;
//#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqHistoManager::save()
{
//#ifdef G4ANALYSIS_USE_ROOT
  if (rootFile) {
    rootFile->Write();       // Writing the histograms to the file
    rootFile->Close();        // and closing the tree (and the file)
    G4cout << "\n----> Histogram Tree is saved \n" << G4endl;
  }
//#endif
}



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqHistoManager::FillEventNtuple(mqROOTEvent* rootEvent)
{

	myROOTEvent = rootEvent;

//#ifdef G4ANALYSIS_USE_ROOT
  if (ntuplEvent) ntuplEvent->Fill();
//#endif
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......








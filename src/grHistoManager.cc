/*
 * grHistoManager.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "grHistoManager.hh"

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

grHistoManager::grHistoManager()
: rootFile(0),ntuplEvent(0), myROOTEvent(0)
{

  // ntuple

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

grHistoManager::~grHistoManager()
{
//#ifdef G4ANALYSIS_USE
    if ( rootFile ) delete rootFile;
//#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void grHistoManager::book(G4String outfile)
{
//#ifdef G4ANALYSIS_USE_ROOT

 // Creating a tree container to handle histograms and ntuples.
 // This tree is associated to an output file.
 //

 G4cout << "GRENDEL> Create ROOT file : grendelSim.root" << G4endl;

 G4String fileName = outfile + "grendelSim.root";
 rootFile = new TFile(fileName,"RECREATE");
 if(!rootFile) {
   G4cout << " grHistoManager::book :"
          << " problem creating the ROOT TFile "
          << G4endl;
   return;
 }


 //info about event statistics
 ntuplEvent = new TTree("Events", "GRENDEL event statistics");


 myROOTEvent = new grROOTEvent();
 ntuplEvent->Branch("ROOTEvent", &myROOTEvent);



 G4cout << "\n----> Histogram file is opened in " << fileName << G4endl;
//#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void grHistoManager::save()
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

void grHistoManager::FillEventNtuple(grROOTEvent* rootEvent)
{

  myROOTEvent = rootEvent;

//#ifdef G4ANALYSIS_USE_ROOT
  if (ntuplEvent) ntuplEvent->Fill();
//#endif
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......








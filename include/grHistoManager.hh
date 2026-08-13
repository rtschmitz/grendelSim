/*
 * grHistoManager.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQHISTOMANAGER_HH_
#define MQHISTOMANAGER_HH_


#include "globals.hh"
#include "G4ThreeVector.hh"
#include "TROOT.h"
#include "TFile.h"
#include "TSystem.h"

#include "grROOTEvent.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

 class TFile;
 class TTree;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class grHistoManager
{
  public:

    grHistoManager();
   ~grHistoManager();

    void book(G4String);
    void save();


    void FillEventNtuple(grROOTEvent* rootEvent);



  private:

    TFile*   rootFile;
    TTree*   ntuplEvent;

    grROOTEvent* myROOTEvent;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif /* MQHISTOMANAGER_HH_ */

/*
 * mqHistoManager.hh
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

#include "mqROOTEvent.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

 class TFile;
 class TTree;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class mqHistoManager
{
  public:

    mqHistoManager();
   ~mqHistoManager();

    void book(G4String);
    void save();


    void FillEventNtuple(mqROOTEvent* rootEvent);



  private:

    TFile*   rootFile;
    TTree*   ntuplEvent;

    mqROOTEvent* myROOTEvent;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


#endif /* MQHISTOMANAGER_HH_ */

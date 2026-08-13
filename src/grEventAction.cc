/*
 * grEventAction.cc
 *
 *  Created on: 18.09.2012
 *      Author: schmitz
 */
#include <iostream>
#include "grEventAction.hh"
#include "grPMTHit.hh"
#include "grScintHit.hh"
#include "grUserEventInformation.hh"
#include "grHistoManager.hh"
#include "grROOTEvent.hh"

#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4UImanager.hh"

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grEventAction::grEventAction(grHistoManager* histo, G4int eventOffset, G4double eventWeight, G4int processID):
  histoManager(histo),fEventOffset(eventOffset),saveThreshold(0),pmtCollID(-1), fProcessID(processID),
  scintCollID(-1),verbose(0),pmtThreshold(1),forcedrawphotons(false),forcenophotons(false), pmtnb(0),
   storePMTHit(false),storeScintHit(true)
{
/* //should be unnecessary now, since we read in event weight from an argument
//read in event weight (and everything else). Might be able to do in PrimaryGenAction, but this is a workaround
  std::ifstream infile;

  std::cout << std::endl << "opening " << fPathname << fFilename << " for event weights" << std::endl;

  infile.open(fPathname.append(fFilename).c_str());
  G4String line;
  G4double fe, fq, fm, fx, fy, fz, fpx, fpy, fpz, fw;
  while (std::getline(infile, line)) {

    std::istringstream iss(line);
    iss.clear();
    iss.str(line);

    if (!(iss >> fq >> fm >> fx >> fy >> fz >> fpx >> fpy >> fpz >> fw))
      break;
    eventWeight.push_back( fw );
  }

  std::cout << std::endl << "found " << eventWeight.size() << " events" << std::endl;
*/
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grEventAction::~grEventAction(){


}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grEventAction::BeginOfEventAction(const G4Event* anEvent){


grUserEventInformation* eventInformation =
    (grUserEventInformation*)anEvent->GetUserInformation();
if (!eventInformation) {
    eventInformation = new grUserEventInformation();
    G4EventManager::GetEventManager()->SetUserInformation(eventInformation);
}

  // Find the sensitive detectors by name
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  if( pmtCollID < 0 ){

    //pmtCollID=SDman->GetCollectionID("pmtHitCollection");

  }

  if( scintCollID < 0 ){

      scintCollID=SDman->GetCollectionID("scintCollection");
  }

G4int eventID = -1;


  //Get event ID
  if (anEvent != NULL) {
    eventID = anEvent->GetEventID();
  } else {
    eventID = -1;
    G4cerr << "!> BeginOfEvent run is NULL!" << G4endl;
  }

  // get User event information and reset it as it is the beginning of the event

        G4double tempW = eventInformation->GetEventWeight();
  eventInformation->Reset();
  eventInformation->SetEventID(eventID);
  eventInformation->SetPhotonLastTrackID(-1);
  eventInformation->SetGammaLastTrackID(-1);
  eventInformation->SetNeutronLastTrackID(-1);
  eventInformation->SetMuonLastTrackID(-1);
  eventInformation->SetElectronLastTrackID(-1);
  eventInformation->SetMCPLastTrackID(-1);
  eventInformation->SetEventWeight(tempW);
  eventInformation->SetProcessID(fProcessID);
  // get event weight and assign it
//  eventInformation->SetEventWeight(eventWeight[eventID+fEventOffset]);

  //can also assign process names if contained/read in
  //eventInformation->SetProcessName(eventWeight[eventID+fEventOffset]);

  if (verbose >= 1) {
    G4cout << ">> Enter event #" << eventID << G4endl;
  }

  //if(recorder)recorder->RecordBeginOfEvent(anEvent);
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grEventAction::EndOfEventAction(const G4Event* anEvent){
  //G4cout << "End of Event Action" << G4endl;
  grUserEventInformation* eventInformation
    =(grUserEventInformation*)anEvent->GetUserInformation();

  //=======================================================================================
  //Hit collections
  //=======================================================================================

  grPMTHitsCollection* PHC = 0; //PMT hit collection
  grScintHitsCollection* THC = 0; // Scintillator hit collection
  G4HCofThisEvent* HCE = anEvent->GetHCofThisEvent();

  //
  //Get the hit collections
  //
  if(HCE){

    if(pmtCollID>=0) PHC = (grPMTHitsCollection*)(HCE->GetHC(pmtCollID));
    if(scintCollID>=0) THC = (grScintHitsCollection*)(HCE->GetHC(scintCollID));


  }

  //=======================================================================================
  //Scint hit collection
  //=======================================================================================


  if(THC){
    //
    //add scint hit vector to the eventInformation
    //
    if (storeScintHit){
      grScintHitVector myScintHitsVec = *(THC->GetVector());

      for (unsigned int j = 0; j < myScintHitsVec.size(); j++) {
              eventInformation->AddScintHit(myScintHitsVec[j]);
      }
    }

  }

  //=======================================================================================
  // PMT hit collection
  //=======================================================================================
  if(PHC){
    //
    //add PMT hit vector to the eventInformation
    //
    if (storePMTHit){
      grPMTHitVector myPMTHitsVec = *(PHC->GetVector());

      for (unsigned int j = 0; j < myPMTHitsVec.size(); j++) {
                eventInformation->AddPMTHit(myPMTHitsVec[j]);
      }
    }


    G4int pmts=PHC->entries();

    //
    //Gather info from all PMTs
    //
    for(G4int i=0;i<pmts;i++){

      eventInformation->IncPECountPMT((*PHC)[i]->GetPhotonCount());

      //pmtnb = (*PHC)[i]->GetPMTNumber();

      if((*PHC)[i]->GetPhotonCount() >= pmtThreshold){
        eventInformation->IncPMTSAboveThreshold();
      }
      else{//wasnt above the threshold, turn it back off, but not now
        //(*PHC)[i]->SetDrawit(false);
      }
    }


    //PHC->DrawAllHits(); not now
  }



  eventInformation->Finalize();


//  if(anEvent->GetEventID() % 1000 == 0){ G4cout << "Event ID: "
//         << anEvent->GetEventID() << G4endl;}
  if(verbose>0){ // TODO put into Print()
  //
    //End of event output. later to be controlled by a verbose level
  //
    G4cout << "\tNumber of PE in PMTs in this event : "
     << eventInformation->GetPECountPMT() << G4endl;

    G4cout << "\tNumber of PMTs above threshold("<<pmtThreshold<<") : "
     << eventInformation->GetPMTSAboveThreshold() << G4endl;
    G4cout << "\tNumber of photons produced  in this event : "
     << eventInformation->GetPhotonCount() << G4endl;

    G4cout << "\tNumber of photons produced by scintillation in this event : "
     << eventInformation->GetPhotonCount_Scint() << G4endl;
    G4cout << "\tNumber of photons produced by cherenkov in this event : "
     << eventInformation->GetPhotonCount_Cheren() << G4endl;
    G4cout << "\tNumber of photons absorbed (OpAbsorption) in this event : "
     << eventInformation->GetAbsorptionCount() << G4endl;
    G4cout << "\tNumber of photons absorbed at boundaries (OpBoundary) in "
     << "this event : " << eventInformation->GetBoundaryAbsorptionCount()
     << G4endl;
    G4cout << "\tUnacounted for photons in this event : "
     << (
         eventInformation->GetPhotonCount_Scint()+
         eventInformation->GetPhotonCount_Cheren()-
         eventInformation->GetAbsorptionCount() -
         eventInformation->GetPECountPMT() -
         eventInformation->GetBoundaryAbsorptionCount())
     << G4endl;


    G4cout << "\tNumber of neutrons in this event: "
        << eventInformation->GetNeutronTracks()->size()
        << G4endl;
    if (verbose > 1){
        for (G4int n  = 0; n < eventInformation->GetNeutronTracks()->size(); n++){
    G4cout << "\tNeutron creation process: "
           << eventInformation->GetNeutronTracks()->at(n)->GetFirstProcessName()
           << G4endl;
        }
    }
  }



  //
  //If we have set the flag to save 'special' events, save here
  //
  if(saveThreshold&&eventInformation->GetPhotonCount_Scint() <= saveThreshold)
    G4RunManager::GetRunManager()->rndmSaveThisEvent();

  //if(recorder)recorder->RecordEndOfEvent(anEvent);
  //




  //
  //fill ntuple via HistoManager for chosen characteristics of the event
  //
    grROOTEvent* rootEvent = eventInformation->ConvertToROOTEvent();

    rootEvent->Finalize();

// if(eventInformation->GetPMTHits()->size()){
    histoManager->FillEventNtuple(rootEvent);
// }
 //   delete rootEvent;
    rootEvent->Reset();
  eventInformation->Reset();
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grEventAction::SetSaveThreshold(G4int save){
/*Sets the save threshold for the random number seed. If the number of photons
generated in an event is lower than this, then save the seed for this event
in a file called run###evt###.rndm
*/
  saveThreshold=save;
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("random/");
  //  G4UImanager::GetUIpointer()->ApplyCommand("/random/setSavingFlag 1");
}

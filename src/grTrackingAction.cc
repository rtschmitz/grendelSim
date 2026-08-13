/*
 * grTrackingAction.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */



#include "grTrackingAction.hh"
#include "grUserTrackInformation.hh"
#include "grDetectorConstruction.hh"
#include "grHistoManager.hh"
#include "grUserEventInformation.hh"
#include "G4ThreeVector.hh"
#include "G4VTouchable.hh"
#include "G4NavigationHistory.hh"

#include "G4TrackingManager.hh"
#include "G4EventManager.hh"
#include "G4Track.hh"
#include "G4ParticleTypes.hh"
// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grTrackingAction::grTrackingAction(grHistoManager* histo):
  histoManager(histo), verbose(0),
  gammaTrackStorage(false),
  neutronTrackStorage(false),
  muonTrackStorage(true),
  electronTrackStorage(false),
  mcpTrackStorage(true),
  trackID(-1),parentID(-1),
  particleName(""),
  initialVolumeName(""),
  initialCopyNo(-1),
  initialProcessName(""),
  initialPosition(0.,0.,0.),
  initialEnergy(0.),
  initialTime(0.)
{

}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grTrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
  ////Let this be up to the user via vis.mac
  //fpTrackingManager->SetStoreTrajectory(true);
  //
  ////Use custom trajectory class, but not now
  //fpTrackingManager->SetTrajectory(new grTrajectory(aTrack));

  //New track, add the user information object
  //This user track information is only relevant to the photons

  // Check if the track already has track information
  if(aTrack->GetUserInformation()!=0) return;


  grUserEventInformation* eventInformation
  =(grUserEventInformation*)G4EventManager::GetEventManager()->GetUserInformation();

  //grGammaTrack *gammaTrack = new grGammaTrack();
  //grNeutronTrack *neutronTrack = new grNeutronTrack();
  //grMuonTrack *muonTrack = new grMuonTrack();
  //grElectronTrack *electronTrack = new grElectronTrack();
  //grMCPTrack *mcpTrack = new grMCPTrack();


  fpTrackingManager->SetUserTrackInformation(new grUserTrackInformation);

  if (aTrack != NULL) {
    trackID = aTrack->GetTrackID();
  } else {
    trackID = -1;
    G4cerr
        << "!>  BeginOfTrack: G4Track is NULL!"
        << G4endl;
  }

  //Get general information about this track
  parentID = aTrack->GetParentID();
  initialVolumeName = aTrack->GetVolume()->GetName();
  initialCopyNo = aTrack->GetTouchable()->GetCopyNumber();
  const G4VTouchable* touch = aTrack->GetTouchable();


  if(initialVolumeName.contains("plScin")){

    //get bar copy num
    G4int barCopyNo = touch->GetCopyNumber(2);
/*
    //get nBarPerLayer
      grDetectorConstruction* detector = (grDetectorConstruction*)G4RunManager::GetRunManager()
                                        ->GetUserDetectorConstruction();
      G4int nBarPerLayer = detector->GetNBarPerLayer();
*/
    G4int nBarPerLayer = 6;

    //get layer number
    char layerNumberChar = touch->GetVolume(2)->GetName().back();

    //ASCII conversion to num
    G4int layerNumber = layerNumberChar-48;

    //set copy num
    initialCopyNo = nBarPerLayer*layerNumber + barCopyNo;
  //  G4cout << "copy num is " << initialCopyNo << G4endl;
  //  G4cout << "volume is " << touch->GetVolume(2)->GetName() << G4endl;
  }
  else if(initialVolumeName.contains("airgap_physic")){

    //get bar copy num
    G4int barCopyNo = touch->GetCopyNumber(1);
/*
    //get nBarPerLayer
      grDetectorConstruction* detector = (grDetectorConstruction*)G4RunManager::GetRunManager()
                                        ->GetUserDetectorConstruction();
      G4int nBarPerLayer = detector->GetNBarPerLayer();
*/
    G4int nBarPerLayer = 6;

    //get layer number
    char layerNumberChar = touch->GetVolume(1)->GetName().back();

    //ASCII conversion to num
    G4int layerNumber = layerNumberChar-48;

    //set copy num
    initialCopyNo = nBarPerLayer*layerNumber + barCopyNo;
  //  G4cout << "copy num is " << initialCopyNo << G4endl;
  //  G4cout << "volume is " << touch->GetVolume(1)->GetName() << G4endl;
  }
  else if(initialVolumeName.contains("barParam")){

    //get bar copy num
    G4int barCopyNo = touch->GetCopyNumber(0);
/*
    //get nBarPerLayer
      grDetectorConstruction* detector = (grDetectorConstruction*)G4RunManager::GetRunManager()
                                        ->GetUserDetectorConstruction();
      G4int nBarPerLayer = detector->GetNBarPerLayer();
*/
    G4int nBarPerLayer = 6;

    //get layer number
    char layerNumberChar = touch->GetVolume(0)->GetName().back();

    //ASCII conversion to num
    G4int layerNumber = layerNumberChar-48;

    //set copy num
    initialCopyNo = nBarPerLayer*layerNumber + barCopyNo;
  //  G4cout << "copy num is " << initialCopyNo << G4endl;
  //  G4cout << "volume is " << touch->GetVolume(1)->GetName() << G4endl;
  }

  else if(initialVolumeName.contains("slab_physic")
    || initialVolumeName.contains("panel_physic")){
    initialCopyNo = touch->GetCopyNumber(2);
    }
  else if(initialVolumeName.contains("airGapSlab_physic")
    || initialVolumeName.contains("airGapPanel_physic")){
    initialCopyNo = touch->GetCopyNumber(1);
    }

  //Primary track has no creator process, so ensure that it exists before you try to access it.
  const G4VProcess* myProcess = aTrack->GetCreatorProcess();
  if (myProcess != NULL) {
    initialProcessName = myProcess->GetProcessName();
  } else {
    initialProcessName = "(Primary particle)";
  }

  initialPosition = aTrack->GetVertexPosition();
  initialEnergy = aTrack->GetVertexKineticEnergy();


  initialTime = aTrack->GetGlobalTime();

  particleName = aTrack->GetDefinition()->GetParticleName();

  if( particleName.contains("gamma") //&& (eventInformation->GetGammaLastTrackID() != trackID)
        && gammaTrackStorage){


      eventInformation->SetGammaLastTrackID(trackID);

        grGammaTrack *gammaTrack = new grGammaTrack();
      gammaTrack->SetTrackID(trackID);
      gammaTrack->SetPDGID(aTrack->GetDefinition()->GetPDGEncoding());
      gammaTrack->SetParentID(parentID);

      //begin of track information
      gammaTrack->SetTimeOfFirstProcess(initialTime/ns);
      gammaTrack->SetInitialEnergy(initialEnergy/MeV);
      gammaTrack->SetFirstPositionX(initialPosition.getX()/m);
      gammaTrack->SetFirstPositionY(initialPosition.getY()/m);
      gammaTrack->SetFirstPositionZ(initialPosition.getZ()/m);
      gammaTrack->SetFirstVolume(initialVolumeName);
      gammaTrack->SetFirstProcessName(initialProcessName);
      gammaTrack->SetFirstCopyNo(initialCopyNo);

      eventInformation->AddGammaTrack(gammaTrack);
//      delete gammaTrack;
  }

  //
  //Store information about new neutron tracks;
  //
  if( particleName.contains("neutron") //&& (eventInformation->GetNeutronLastTrackID() != trackID)
      && neutronTrackStorage){
    eventInformation->SetNeutronLastTrackID(trackID);


      grNeutronTrack *neutronTrack = new grNeutronTrack();
    neutronTrack->SetTrackID(trackID);
    neutronTrack->SetPDGID(aTrack->GetDefinition()->GetPDGEncoding());
    neutronTrack->SetParentID(parentID);

    //begin of track information
    neutronTrack->SetTimeOfFirstProcess(initialTime/s);
    neutronTrack->SetInitialEnergy(initialEnergy/MeV);
    neutronTrack->SetFirstPositionX(initialPosition.getX()/m);
    neutronTrack->SetFirstPositionY(initialPosition.getY()/m);
    neutronTrack->SetFirstPositionZ(initialPosition.getZ()/m);
    neutronTrack->SetFirstVolume(initialVolumeName);
    neutronTrack->SetFirstProcessName(initialProcessName);
    neutronTrack->SetFirstCopyNo(initialCopyNo);

    eventInformation->AddNeutronTrack(neutronTrack);
//    delete neutronTrack;
  }


  //
  //Store info about muons
  //
  if( (particleName.contains("mu")) //&& (eventInformation->GetMuonLastTrackID() != trackID)
        && muonTrackStorage){
  //G4cout << "Initial momentum X: " << initialMomentum.getX() << G4endl;
  //G4cout << "Initial momentum Y: " << initialMomentum.getY() << G4endl;
  //G4cout << "Initial momentum Z: " << initialMomentum.getZ() << G4endl;

    eventInformation->SetMuonLastTrackID(trackID);


      grMuonTrack *muonTrack = new grMuonTrack();
    muonTrack->SetTrackID(trackID);
    muonTrack->SetPDGID(aTrack->GetDefinition()->GetPDGEncoding());
    muonTrack->SetParentID(parentID);

    //begin of track information
    muonTrack->SetTimeOfFirstProcess(initialTime/s);
    muonTrack->SetInitialEnergy(initialEnergy/MeV);
    muonTrack->SetFirstPositionX(initialPosition.getX()/m);
    muonTrack->SetFirstPositionY(initialPosition.getY()/m);
    muonTrack->SetFirstPositionZ(initialPosition.getZ()/m);
    muonTrack->SetFirstVolume(initialVolumeName);
    muonTrack->SetFirstProcessName(initialProcessName);
    muonTrack->SetFirstCopyNo(initialCopyNo);

    eventInformation->AddMuonTrack(muonTrack);
    //delete muonTrack;
  }


  //
  //Store info about electrons
  //
  if( (particleName.contains("e+") || particleName.contains("e-")) //&& (eventInformation->GetElectronLastTrackID() != trackID)
        && electronTrackStorage){
    eventInformation->SetElectronLastTrackID(trackID);


      grElectronTrack *electronTrack = new grElectronTrack();
    electronTrack->SetTrackID(trackID);
    electronTrack->SetPDGID(aTrack->GetDefinition()->GetPDGEncoding());
    electronTrack->SetParentID(parentID);

    //begin of track information
    electronTrack->SetTimeOfFirstProcess(initialTime/s);
    electronTrack->SetInitialEnergy(initialEnergy/MeV);
    electronTrack->SetFirstPositionX(initialPosition.getX()/m);
    electronTrack->SetFirstPositionY(initialPosition.getY()/m);
    electronTrack->SetFirstPositionZ(initialPosition.getZ()/m);
    electronTrack->SetFirstVolume(initialVolumeName);
    electronTrack->SetFirstProcessName(initialProcessName);
    electronTrack->SetFirstCopyNo(initialCopyNo);

    eventInformation->AddElectronTrack(electronTrack);
    //delete electronTrack;
  }


  //
  //Store info about mcps
  //
  if( (particleName.contains("kaon")) //&& (eventInformation->GetMCPLastTrackID() != trackID)
        && mcpTrackStorage){
    eventInformation->SetMCPLastTrackID(trackID);


      grMCPTrack *mcpTrack = new grMCPTrack();
    mcpTrack->SetTrackID(trackID);
    mcpTrack->SetPDGID(aTrack->GetDefinition()->GetPDGEncoding());
    mcpTrack->SetParentID(parentID);

    //begin of track information
    mcpTrack->SetTimeOfFirstProcess(initialTime/s);
    mcpTrack->SetInitialEnergy(initialEnergy/MeV);
    mcpTrack->SetFirstPositionX(initialPosition.getX()/m);
    mcpTrack->SetFirstPositionY(initialPosition.getY()/m);
    mcpTrack->SetFirstPositionZ(initialPosition.getZ()/m);
    mcpTrack->SetFirstVolume(initialVolumeName);
    mcpTrack->SetFirstProcessName(initialProcessName);
    mcpTrack->SetFirstCopyNo(initialCopyNo);

    eventInformation->AddMCPTrack(mcpTrack);
  //  delete mcpTrack;
  }
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grTrackingAction::PostUserTrackingAction(const G4Track* aTrack){

  //grTrajectory* trajectory=(grTrajectory*)fpTrackingManager->GimmeTrajectory();

    grUserEventInformation* eventInformation
    =(grUserEventInformation*)G4EventManager::GetEventManager()->GetUserInformation();


    //
  // Get general information about this track
  // Get the last process of the track via the last stepPoint of the track
    //
  G4StepPoint *finalStepPoint = aTrack->GetStep()->GetPostStepPoint();
  //G4StepPoint *prefinalStepPoint = aTrack->GetStep()->GetPreStepPoint();
  G4String finalVolumeName="";
  G4int finalCopyNo=0;
  G4ThreeVector finalPosition = aTrack->GetPosition();
  G4double finalEnergy = aTrack->GetKineticEnergy();
//  G4double energyDeposit= aTrack->GetEnergyDeposit(); //this is the implicit deposited energy
//  if we want a better deposited energy figure, use an initial-final approach
  G4double finalTime = aTrack->GetGlobalTime();
  G4double finalTrackLength = aTrack->GetTrackLength();
  G4int trackID = aTrack->GetTrackID();
      G4String myEndProcessName = "";

        if (finalStepPoint != NULL) {
                        if (finalStepPoint->GetPhysicalVolume() != NULL) {
                                finalVolumeName = finalStepPoint->GetPhysicalVolume()->GetName();
        finalCopyNo = finalStepPoint->GetPhysicalVolume()->GetCopyNo();
                        }

                        if (finalStepPoint->GetProcessDefinedStep() != NULL) {
                                myEndProcessName = finalStepPoint->GetProcessDefinedStep()->GetProcessName();
                                        }
                }

  //
  //
  if( particleName.contains("gamma")// && (eventInformation->GetGammaLastTrackID() != trackID)
      && gammaTrackStorage){


    //end of track information
      eventInformation->GetGammaTrack(trackID)->SetLastPositionX(finalPosition.getX()/m);
      eventInformation->GetGammaTrack(trackID)->SetLastPositionY(finalPosition.getY()/m);
      eventInformation->GetGammaTrack(trackID)->SetLastPositionZ(finalPosition.getZ()/m);
      eventInformation->GetGammaTrack(trackID)->SetFinalEnergy(finalEnergy/MeV);
      eventInformation->GetGammaTrack(trackID)->SetTotalTrackLength(finalTrackLength/m);
      eventInformation->GetGammaTrack(trackID)->SetLastVolume(finalVolumeName);
      eventInformation->GetGammaTrack(trackID)->SetLastProcessName(myEndProcessName);
      eventInformation->GetGammaTrack(trackID)->SetTimeOfLastProcess(finalTime/ns);
      eventInformation->GetGammaTrack(trackID)->SetLastCopyNo(finalCopyNo);

  }
  //
  //Store information about new neutron tracks;
  //
  if( particleName.contains("neutron") //&& (eventInformation->GetNeutronLastTrackID() != trackID)
      && neutronTrackStorage){

    //end of track information
    eventInformation->GetNeutronTrack(trackID)->SetLastPositionX(finalPosition.getX()/m);
    eventInformation->GetNeutronTrack(trackID)->SetLastPositionY(finalPosition.getY()/m);
    eventInformation->GetNeutronTrack(trackID)->SetLastPositionZ(finalPosition.getZ()/m);
    eventInformation->GetNeutronTrack(trackID)->SetFinalEnergy(finalEnergy/MeV);
          eventInformation->GetNeutronTrack(trackID)->SetTotalTrackLength(finalTrackLength/m);
          eventInformation->GetNeutronTrack(trackID)->SetLastVolume(finalVolumeName);
          eventInformation->GetNeutronTrack(trackID)->SetLastProcessName(myEndProcessName);
          eventInformation->GetNeutronTrack(trackID)->SetTimeOfLastProcess(finalTime/s);
    eventInformation->GetNeutronTrack(trackID)->SetLastCopyNo(finalCopyNo);

    //G4cout << "Total energy neutron: " << totalEnergy/MeV << G4endl;
    //G4cout << "Final energy neutron: " << finalEnergy/MeV << G4endl;

  }

  //
  //Store information about muon tracks
  //
  if( (particleName.contains("mu")) //&& (eventInformation->GetMuonLastTrackID() != trackID)
      && muonTrackStorage){

    //end of track information
    eventInformation->GetMuonTrack(trackID)->SetLastPositionX(finalPosition.getX()/m);
    eventInformation->GetMuonTrack(trackID)->SetLastPositionY(finalPosition.getY()/m);
    eventInformation->GetMuonTrack(trackID)->SetLastPositionZ(finalPosition.getZ()/m);
    eventInformation->GetMuonTrack(trackID)->SetFinalEnergy(finalEnergy/MeV);
    eventInformation->GetMuonTrack(trackID)->SetTotalTrackLength(finalTrackLength/m);
    eventInformation->GetMuonTrack(trackID)->SetLastVolume(finalVolumeName);
        eventInformation->GetMuonTrack(trackID)->SetLastProcessName(myEndProcessName);
          eventInformation->GetMuonTrack(trackID)->SetTimeOfLastProcess(finalTime/s);
    eventInformation->GetMuonTrack(trackID)->SetLastCopyNo(finalCopyNo);

  }


  //
  //Store information about electron tracks
  //
  if( (particleName.contains("e+") || particleName.contains("e-")) //&& (eventInformation->GetElectronLastTrackID() != trackID)
      && electronTrackStorage){

    //end of track information
//    eventInformation->GetElectronTrack(trackID)->SetLastPositionX(finalPosition.getX()/m);
//    eventInformation->GetElectronTrack(trackID)->SetLastPositionY(finalPosition.getY()/m);
//    eventInformation->GetElectronTrack(trackID)->SetLastPositionZ(finalPosition.getZ()/m);
    eventInformation->GetElectronTrack(trackID)->SetFinalEnergy(finalEnergy/MeV);
    eventInformation->GetElectronTrack(trackID)->SetTotalTrackLength(finalTrackLength/m);
    eventInformation->GetElectronTrack(trackID)->SetLastVolume(finalVolumeName);
        eventInformation->GetElectronTrack(trackID)->SetLastProcessName(myEndProcessName);
          eventInformation->GetElectronTrack(trackID)->SetTimeOfLastProcess(finalTime/s);
    eventInformation->GetElectronTrack(trackID)->SetLastCopyNo(finalCopyNo);

  }


  //
  //Store information about mcp tracks
  //
  if( (particleName.contains("kaon")) //&& (eventInformation->GetMCPLastTrackID() != trackID)
      && mcpTrackStorage){
    //end of track information
    eventInformation->GetMCPTrack(trackID)->SetLastPositionX(finalPosition.getX()/m);
    eventInformation->GetMCPTrack(trackID)->SetLastPositionY(finalPosition.getY()/m);
    eventInformation->GetMCPTrack(trackID)->SetLastPositionZ(finalPosition.getZ()/m);
    eventInformation->GetMCPTrack(trackID)->SetFinalEnergy(finalEnergy/MeV);
    eventInformation->GetMCPTrack(trackID)->SetTotalTrackLength(finalTrackLength/m);
    eventInformation->GetMCPTrack(trackID)->SetLastVolume(finalVolumeName);
        eventInformation->GetMCPTrack(trackID)->SetLastProcessName(myEndProcessName);
          eventInformation->GetMCPTrack(trackID)->SetTimeOfLastProcess(finalTime/s);
    eventInformation->GetMCPTrack(trackID)->SetLastCopyNo(finalCopyNo);

  }



    //========================================================================================================================
     //========================================================================================================================

     //char gNucl[10];
     //strcpy(gNucl, "");
     //std::vector<G4double> gKineticEnergy;
     //gKineticEnergy.clear();



  //else //draw all other trajectories
  //  trajectory->SetDrawTrajectory(true);

  //if(recorder)recorder->Record)Track(aTrack);



}

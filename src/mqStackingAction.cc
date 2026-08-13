/*
 * mqStackingAction.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "mqStackingAction.hh"
#include "mqUserEventInformation.hh"
#include "mqSteppingAction.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqStackingAction::mqStackingAction()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqStackingAction::~mqStackingAction()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4ClassificationOfNewTrack
mqStackingAction::ClassifyNewTrack(const G4Track * aTrack){

  mqUserEventInformation* eventInformation=
    (mqUserEventInformation*)G4EventManager::GetEventManager()
    ->GetConstCurrentEvent()->GetUserInformation();

  //Count what process generated the optical photons
  if(aTrack->GetDefinition()==G4OpticalPhoton::OpticalPhotonDefinition()){
	  eventInformation->IncPhotonCount();
    // particle is optical photon
    if(aTrack->GetParentID() > 0){
   //   // particle is secondary
      if(aTrack->GetCreatorProcess()->GetProcessName()=="Scintillation")
    	  eventInformation->IncPhotonCount_Scint();
      if(aTrack->GetCreatorProcess()->GetProcessName()=="Cerenkov")
    	  eventInformation->IncPhotonCount_Cheren();
      }

    }
  return fUrgent;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqStackingAction::NewStage(){
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqStackingAction::PrepareNewEvent(){
}


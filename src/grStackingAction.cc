/*
 * grStackingAction.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "grStackingAction.hh"
#include "grUserEventInformation.hh"
#include "grSteppingAction.hh"

#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grStackingAction::grStackingAction()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grStackingAction::~grStackingAction()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4ClassificationOfNewTrack
grStackingAction::ClassifyNewTrack(const G4Track * aTrack){

  grUserEventInformation* eventInformation=
    (grUserEventInformation*)G4EventManager::GetEventManager()
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
void grStackingAction::NewStage(){
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void grStackingAction::PrepareNewEvent(){
}


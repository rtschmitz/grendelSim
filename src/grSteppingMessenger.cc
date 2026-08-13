/*
 * grSteppingMessenger.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "grSteppingMessenger.hh"
#include "grSteppingAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithABool.hh"

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grSteppingMessenger::grSteppingMessenger(grSteppingAction* step)
:stepping(step)
{
  oneStepPrimariesCmd = new G4UIcmdWithABool("/grendelSim/WaterTank/oneStepPrimaries",this);
  oneStepPrimariesCmd->SetGuidance("Only allows primaries to go one step in the scintillator volume before being killed.");
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grSteppingMessenger::~grSteppingMessenger(){
  delete oneStepPrimariesCmd;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void
grSteppingMessenger::SetNewValue(G4UIcommand* command,G4String newValue){
  if( command == oneStepPrimariesCmd ){
    stepping->SetOneStepPrimaries(oneStepPrimariesCmd
          ->GetNewBoolValue(newValue));
  }
}

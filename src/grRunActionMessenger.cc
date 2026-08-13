// ------------------------------------------------
//
//  grRunActionMessenger.cc
//
// ------------------------------------------------

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

#include "G4RunManager.hh"

#include "grRunActionMessenger.hh"
#include "grRunAction.hh"

// ------------------------------------------------

grRunActionMessenger::grRunActionMessenger(grRunAction* grRun):pRunAction(grRun)
{
  // Create the run directory
  grRunDir = new G4UIdirectory("/grendelSim/");
  grRunDir->SetGuidance("grendelSim specific run controls.");
  //

  //  run directory already exists

  // Set file name
  setRunFileName = new G4UIcmdWithAString("/run/fname",this);
  setRunFileName->SetGuidance("Set the name of the output files.");
  setRunFileName->SetParameterName("fname",true);
  setRunFileName->SetDefaultValue("Sim");
  setRunFileName->AvailableForStates(G4State_PreInit,G4State_Idle);

}

// ------------------------------------------------

grRunActionMessenger::~grRunActionMessenger()
{
  delete setRunFileName;
}

// ------------------------------------------------

void grRunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{

  if( command == setRunFileName ) {
    pRunAction->SetDataFileNamePrefix(newValue);
  }

}

// ------------------------------------------------


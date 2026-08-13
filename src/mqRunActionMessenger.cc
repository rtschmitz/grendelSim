// ------------------------------------------------
//
//  mqRunActionMessenger.cc
//
// ------------------------------------------------

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

#include "G4RunManager.hh"

#include "mqRunActionMessenger.hh"
#include "mqRunAction.hh"

// ------------------------------------------------

mqRunActionMessenger::mqRunActionMessenger(mqRunAction* mqRun):pRunAction(mqRun)
{ 
  // Create the run directory
  mqRunDir = new G4UIdirectory("/MilliQan/");
  mqRunDir->SetGuidance("MilliQan specific run controls.");
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

mqRunActionMessenger::~mqRunActionMessenger()
{
  delete setRunFileName;     
}

// ------------------------------------------------

void mqRunActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 

  if( command == setRunFileName ) {
    pRunAction->SetDataFileNamePrefix(newValue);
  }

}

// ------------------------------------------------


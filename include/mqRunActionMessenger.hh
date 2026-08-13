// ------------------------------------------------
//
//  mqRunActionMessenger.hh
//
// ------------------------------------------------

#ifndef mqRunActionMessenger_h
#define mqRunActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

// ------------------------------------------------

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class mqRunAction;

// ------------------------------------------------

class mqRunActionMessenger: public G4UImessenger
{
public:
  mqRunActionMessenger(mqRunAction* );
  ~mqRunActionMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  mqRunAction*      	  pRunAction;
  G4UIdirectory*          mqRunDir;

  // Run Element Activation

  G4UIcmdWithAString*     setRunFileName;
};

#endif

// ------------------------------------------------
//
//  grRunActionMessenger.hh
//
// ------------------------------------------------

#ifndef grRunActionMessenger_h
#define grRunActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

// ------------------------------------------------

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class grRunAction;

// ------------------------------------------------

class grRunActionMessenger: public G4UImessenger
{
public:
  grRunActionMessenger(grRunAction* );
  ~grRunActionMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  grRunAction*          pRunAction;
  G4UIdirectory*          grRunDir;

  // Run Element Activation

  G4UIcmdWithAString*     setRunFileName;
};

#endif

#ifndef grSessionMessenger_h
#define grSessionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class grSession;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class grSessionMessenger: public G4UImessenger {
public:
  grSessionMessenger(grSession* Sess);
  ~grSessionMessenger();

  void SetNewValue(G4UIcommand*, G4String);
private:
  G4UIdirectory* dirLogging;
  grSession* mySession;
  G4UIcmdWithABool* cmdLogOn;
  G4UIcmdWithAString* cmdLogFileName;
};

#endif

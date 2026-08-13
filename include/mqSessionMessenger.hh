#ifndef mqSessionMessenger_h
#define mqSessionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class mqSession;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithABool;

class mqSessionMessenger: public G4UImessenger {
public:
	mqSessionMessenger(mqSession* Sess);
	~mqSessionMessenger();

	void SetNewValue(G4UIcommand*, G4String);
private:
	G4UIdirectory* dirLogging;
	mqSession* mySession;
	G4UIcmdWithABool* cmdLogOn;
	G4UIcmdWithAString* cmdLogFileName;
};

#endif

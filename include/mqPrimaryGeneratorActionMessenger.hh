#ifndef mqPrimaryGeneratorActionMessenger_h
#define mqPrimaryGeneratorActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class mqPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;

class mqPrimaryGeneratorActionMessenger: public G4UImessenger {
public:
	mqPrimaryGeneratorActionMessenger(mqPrimaryGeneratorAction* PGA);
	~mqPrimaryGeneratorActionMessenger();

	void SetNewValue(G4UIcommand*, G4String);
private:
	mqPrimaryGeneratorAction* myPGA;
	G4UIdirectory* dirPGA;

	G4UIcmdWithABool *cmdUseGeantinos;


	G4UIcmdWithAnInteger *cmdSeed0;
	G4UIcmdWithAnInteger *cmdSeed1;
	G4UIcmdWithAnInteger *cmdSeed2;
	G4UIcmdWithoutParameter *cmdSetSeeds;
	unsigned long Seed0;
	unsigned long Seed1;
	unsigned long Seed2;

};

#endif


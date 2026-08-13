#ifndef grPrimaryGeneratorActionMessenger_h
#define grPrimaryGeneratorActionMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class grPrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithABool;
class G4UIcmdWithoutParameter;

class grPrimaryGeneratorActionMessenger: public G4UImessenger {
public:
  grPrimaryGeneratorActionMessenger(grPrimaryGeneratorAction* PGA);
  ~grPrimaryGeneratorActionMessenger();

  void SetNewValue(G4UIcommand*, G4String);
private:
  grPrimaryGeneratorAction* myPGA;
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


#ifndef grRunAction_h
#define grRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Run.hh"

class grRunActionMessenger;
class grHistoManager;
class grRunAction: public G4UserRunAction {
public:
  grRunAction(grHistoManager*);
  ~grRunAction();

  G4Run* GenerateRun();
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void SetDataFileNamePrefix(G4String nPrefix)  {DataFileNamePrefix = nPrefix;}

private:
    grHistoManager* histoManager;
    grRunActionMessenger* runMessenger;

    G4String DataFileNamePrefix;
};

#endif


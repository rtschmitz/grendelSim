#ifndef GREVENTACTION_HH_
#define GREVENTACTION_HH_

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;
class grHistoManager;

class grEventAction : public G4UserEventAction {
public:
  grEventAction(grHistoManager*, G4int eventOffset, G4double eventWeight, G4int processID);
  ~grEventAction();
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void SetEventVerbose(G4int value) { verbose = value; }
private:
  grHistoManager* histoManager;
  G4int scintCollID;
  G4int verbose;
  G4double fEventWeight;
  G4int fProcessID;
};

#endif

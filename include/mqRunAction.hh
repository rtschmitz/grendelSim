#ifndef mqRunAction_h
#define mqRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4Run.hh"

class mqRunActionMessenger;
class mqHistoManager;
class mqRunAction: public G4UserRunAction {
public:
	mqRunAction(mqHistoManager*);
	~mqRunAction();

	G4Run* GenerateRun();
	void BeginOfRunAction(const G4Run*);
	void EndOfRunAction(const G4Run*);

	void SetDataFileNamePrefix(G4String nPrefix)	{DataFileNamePrefix = nPrefix;}

private:
	  mqHistoManager* histoManager;
	  mqRunActionMessenger* runMessenger;

	  G4String DataFileNamePrefix;
};

#endif


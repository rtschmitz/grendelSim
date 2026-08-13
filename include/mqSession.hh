#ifndef mqSession_h
#define mqSession_h 1

#include "G4UIsession.hh"
#include "mqSessionMessenger.hh"
#include <iostream>
#include <fstream>

class mqSession: public G4UIsession {
public:
	mqSession();
	~mqSession();

	G4int ReceiveG4cout(G4String coutString);
	G4int ReceiveG4cerr(G4String cerrString);
	void logOn(G4bool myOnOff);
	void SetLogFileName(G4String filename);
private:
	void reinitFile();
	std::ofstream logFile;
	mqSessionMessenger* mySessionMessenger;
	G4String logFileName;
	G4bool logOnOff;
};

#endif

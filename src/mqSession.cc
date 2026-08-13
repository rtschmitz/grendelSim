#include "mqSession.hh"
#include "mqSessionMessenger.hh"

mqSession::mqSession():
logOnOff(false){
	mySessionMessenger = new mqSessionMessenger(this);
}

mqSession::~mqSession() {
	delete mySessionMessenger;
	logFile.close();
}

G4int mqSession::ReceiveG4cout(G4String coutString) {
	if (logOnOff) {
		logFile << coutString << std::flush;
	}
	std::cout << coutString;
	return 0;
}

G4int mqSession::ReceiveG4cerr(G4String cerrString) {
	if (logOnOff) {
		logFile << "Error!: " << cerrString << std::flush;
	}
	std::cerr << cerrString;
	return 0;
}

void mqSession::logOn(G4bool myOnOff) {
	logOnOff = myOnOff;
	reinitFile();
}

void mqSession::SetLogFileName(G4String filename) {
	logFileName = filename;
	reinitFile();
}

void mqSession::reinitFile() {
	if (logOnOff) {
		logFile.close();
		logFile.open(logFileName);
		// 		G4cout << "   Turn Logfile '" << logFileName << "' on." << G4endl;
	} else {
		logFile.close();
		// 		G4cout << "   Turn Logfile '" << logFileName << "' off." << G4endl;
	}
}


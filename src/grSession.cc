#include "grSession.hh"
#include "grSessionMessenger.hh"

grSession::grSession():
logOnOff(false){
  mySessionMessenger = new grSessionMessenger(this);
}

grSession::~grSession() {
  delete mySessionMessenger;
  logFile.close();
}

G4int grSession::ReceiveG4cout(G4String coutString) {
  if (logOnOff) {
    logFile << coutString << std::flush;
  }
  std::cout << coutString;
  return 0;
}

G4int grSession::ReceiveG4cerr(G4String cerrString) {
  if (logOnOff) {
    logFile << "Error!: " << cerrString << std::flush;
  }
  std::cerr << cerrString;
  return 0;
}

void grSession::logOn(G4bool myOnOff) {
  logOnOff = myOnOff;
  reinitFile();
}

void grSession::SetLogFileName(G4String filename) {
  logFileName = filename;
  reinitFile();
}

void grSession::reinitFile() {
  if (logOnOff) {
    logFile.close();
    logFile.open(logFileName);
    //     G4cout << "   Turn Logfile '" << logFileName << "' on." << G4endl;
  } else {
    logFile.close();
    //     G4cout << "   Turn Logfile '" << logFileName << "' off." << G4endl;
  }
}

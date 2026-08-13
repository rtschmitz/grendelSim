#ifndef grSession_h
#define grSession_h 1

#include "G4UIsession.hh"
#include "grSessionMessenger.hh"
#include <iostream>
#include <fstream>

class grSession: public G4UIsession {
public:
  grSession();
  ~grSession();

  G4int ReceiveG4cout(G4String coutString);
  G4int ReceiveG4cerr(G4String cerrString);
  void logOn(G4bool myOnOff);
  void SetLogFileName(G4String filename);
private:
  void reinitFile();
  std::ofstream logFile;
  grSessionMessenger* mySessionMessenger;
  G4String logFileName;
  G4bool logOnOff;
};

#endif

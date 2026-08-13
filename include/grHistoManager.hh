#ifndef GRHISTOMANAGER_HH_
#define GRHISTOMANAGER_HH_

#include "globals.hh"

class TFile;
class TTree;
class grUserEventInformation;
struct grOutputBuffer;

class grHistoManager {
public:
  grHistoManager();
  ~grHistoManager();
  void book(G4String outputDirectory);
  void save();
  void FillEventNtuple(grUserEventInformation& event);
private:
  TFile* rootFile;
  TTree* eventTree;
  grOutputBuffer* output;
};

#endif

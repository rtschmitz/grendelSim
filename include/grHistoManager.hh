#ifndef GRHISTOMANAGER_HH_
#define GRHISTOMANAGER_HH_

#include "globals.hh"

class TFile;
class TTree;
class grUserEventInformation;
struct grEventOutput;
struct grTrackOutput;
struct grHitOutput;

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
  TTree* trackTree;
  TTree* hitTree;
  grEventOutput* eventOutput;
  grTrackOutput* trackOutput;
  grHitOutput* hitOutput;
};

#endif

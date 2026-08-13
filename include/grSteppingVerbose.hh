#ifndef grSteppingVerbose_h
#define grSteppingVerbose_h 1

#include "G4SteppingVerbose.hh"
class grSteppingVerbose;

class grSteppingVerbose: public G4SteppingVerbose {
public:
  grSteppingVerbose();
  ~grSteppingVerbose();

  void StepInfo();
  void TrackingStarted();
};

#endif

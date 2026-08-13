#ifndef mqSteppingVerbose_h
#define mqSteppingVerbose_h 1

#include "G4SteppingVerbose.hh"
class mqSteppingVerbose;

class mqSteppingVerbose: public G4SteppingVerbose {
public:
	mqSteppingVerbose();
	~mqSteppingVerbose();

	void StepInfo();
	void TrackingStarted();
};

#endif

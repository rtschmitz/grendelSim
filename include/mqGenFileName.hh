#ifndef mqGenFileName_h
#define mqGenFileName_h 1

#include "G4String.hh"

class mqGenFileName {
public:
	mqGenFileName();
	~mqGenFileName();
	static G4String genFileName(char postfix[6]);
private:
	static char prefix[24];
};

#endif

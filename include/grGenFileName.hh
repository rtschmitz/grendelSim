#ifndef grGenFileName_h
#define grGenFileName_h 1

#include "G4String.hh"

class grGenFileName {
public:
  grGenFileName();
  ~grGenFileName();
  static G4String genFileName(char postfix[6]);
private:
  static char prefix[24];
};

#endif

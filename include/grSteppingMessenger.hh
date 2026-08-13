/*
 * grSteppingMessenger.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSTEPPINGMESSENGER_HH_
#define MQSTEPPINGMESSENGER_HH_

#include "G4UImessenger.hh"
#include "globals.hh"

class grSteppingAction;
class G4UIcmdWithABool;

class grSteppingMessenger: public G4UImessenger
{
public:
  grSteppingMessenger(grSteppingAction*);
  ~grSteppingMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  grSteppingAction*        stepping;
  G4UIcmdWithABool*  oneStepPrimariesCmd;

};



#endif /* MQSTEPPINGMESSENGER_HH_ */

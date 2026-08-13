/*
 * mqSteppingMessenger.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSTEPPINGMESSENGER_HH_
#define MQSTEPPINGMESSENGER_HH_

#include "G4UImessenger.hh"
#include "globals.hh"

class mqSteppingAction;
class G4UIcmdWithABool;

class mqSteppingMessenger: public G4UImessenger
{
public:
  mqSteppingMessenger(mqSteppingAction*);
  ~mqSteppingMessenger();

  void SetNewValue(G4UIcommand*, G4String);

private:
  mqSteppingAction*        stepping;
  G4UIcmdWithABool*  oneStepPrimariesCmd;

};



#endif /* MQSTEPPINGMESSENGER_HH_ */

/*
 * mqSteppingAction.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSTEPPINGACTION_HH_
#define MQSTEPPINGACTION_HH_

#include "globals.hh"
#include "G4UserSteppingAction.hh"


#include  "mqEventAction.hh"
#include  "mqTrackingAction.hh"
#include  "mqSteppingMessenger.hh"
#include  "mqHistoManager.hh"

class mqSteppingAction : public G4UserSteppingAction
{
public:
  mqSteppingAction(mqHistoManager*);
  ~mqSteppingAction();
  virtual void UserSteppingAction(const G4Step*);

  void SetOneStepPrimaries(G4bool b){oneStepPrimaries=b;}
  G4bool GetOneStepPrimaries(){return oneStepPrimaries;}

private:
  G4bool oneStepPrimaries;
  mqSteppingMessenger* steppingMessenger;
  mqHistoManager* histoManager;
  G4bool killPhoton;
};



#endif /* MQSTEPPINGACTION_HH_ */

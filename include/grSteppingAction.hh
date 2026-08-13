/*
 * grSteppingAction.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSTEPPINGACTION_HH_
#define MQSTEPPINGACTION_HH_

#include "globals.hh"
#include "G4UserSteppingAction.hh"


#include  "grEventAction.hh"
#include  "grTrackingAction.hh"
#include  "grSteppingMessenger.hh"
#include  "grHistoManager.hh"

class grSteppingAction : public G4UserSteppingAction
{
public:
  grSteppingAction(grHistoManager*);
  ~grSteppingAction();
  virtual void UserSteppingAction(const G4Step*);

  void SetOneStepPrimaries(G4bool b){oneStepPrimaries=b;}
  G4bool GetOneStepPrimaries(){return oneStepPrimaries;}

private:
  G4bool oneStepPrimaries;
  grSteppingMessenger* steppingMessenger;
  grHistoManager* histoManager;
  G4bool killPhoton;
};



#endif /* MQSTEPPINGACTION_HH_ */

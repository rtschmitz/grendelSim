/*
 * mqStackingAction.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSTACKINGACTION_HH_
#define MQSTACKINGACTION_HH_

#include "globals.hh"
#include "G4UserStackingAction.hh"

class mqStackingAction : public G4UserStackingAction
{
public:
  mqStackingAction();
  ~mqStackingAction();

  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track* aTrack);
  virtual void NewStage();
  virtual void PrepareNewEvent();

private:
};




#endif /* MQSTACKINGACTION_HH_ */

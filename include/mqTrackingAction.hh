/*
 * mqTrackingAction.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQTRACKINGACTION_HH_
#define MQTRACKINGACTION_HH_

#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
class mqHistoManager;

class mqTrackingAction : public G4UserTrackingAction {

public:
  mqTrackingAction(mqHistoManager*);
  ~mqTrackingAction() {};//delete histoManager;};

  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*);

private:
  mqHistoManager* histoManager;
  G4int verbose;
  G4bool photonTrackStorage;
  G4bool gammaTrackStorage;
  G4bool neutronTrackStorage;
  G4bool muonTrackStorage;
  G4bool electronTrackStorage;
  G4bool mcpTrackStorage;
  G4int trackID;
  G4int parentID;
  G4String particleName;
  G4ThreeVector initialPosition;
  G4ThreeVector initialMomentum;
  G4String initialVolumeName;
  G4int initialCopyNo;
  G4String initialProcessName;
  G4double initialEnergy;
  G4double initialTime;

};



#endif /* MQTRACKINGACTION_HH_ */

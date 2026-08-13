/*
 * mqEventAction.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQEVENTACTION_HH_
#define MQEVENTACTION_HH_


#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4Event;
class mqHistoManager;
//const G4int PMTnb = 72;
class mqEventAction : public G4UserEventAction
{
public:
  mqEventAction(mqHistoManager*,G4int,G4double,G4int);
  ~mqEventAction();

public:
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void SetSaveThreshold(G4int save);

  void SetEventVerbose(G4int v){verbose=v;}

  void SetPMTThreshold(G4int t){pmtThreshold=t;}

  void SetForceDrawPhotons(G4bool b){forcedrawphotons=b;}

  void SetForceDrawNoPhotons(G4bool b){forcenophotons=b;}

private:

  mqHistoManager* histoManager;


  G4int  saveThreshold;

  G4int  pmtCollID;

  G4int  scintCollID;

  G4int  verbose;

  G4int  pmtThreshold;

  G4bool forcedrawphotons;

  G4bool forcenophotons;

  G4int pmtnb;

//  G4int scintnb;

  G4bool storePMTHit;

  G4bool storeScintHit;

  G4int fEventOffset;

  G4double fEventWeight;

  G4int fProcessID;

//  std::vector<G4double> eventWeight;
};


#endif /* MQEVENTACTION_HH_ */

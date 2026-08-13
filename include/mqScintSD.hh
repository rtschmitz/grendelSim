/*
 * mqScintSD.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSCINTSD_HH_
#define MQSCINTSD_HH_

#include "G4VSensitiveDetector.hh"
#include "mqScintHit.hh"
#include <G4StepPoint.hh>
#include <fstream>
#include <iostream>

//#include "mqScintHitCollection.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class mqScintSD : public G4VSensitiveDetector
{
  public:
      mqScintSD(G4String);
     ~mqScintSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      G4bool ProcessHitsEnter(const G4Step*, G4TouchableHistory*);
      G4bool ProcessHitsExit(const G4Step*, G4TouchableHistory*);

    void SetHitEnergy(G4double ed){ hitEnergy    = ed;    };
    G4double GetHitEnergy()const{ return hitEnergy; };

    void SetHitTime(G4double tm){ hitTime    = tm;    };
    G4double GetHitTime()const{ return hitTime; };

    void SetHitPosition     (G4ThreeVector xyz){ hitPosition     = xyz;   };
    G4ThreeVector GetHitPosition()const{ return hitPosition; };      
      
      void EndOfEvent(G4HCofThisEvent*);
      G4int HCID;

  	  private:
      mqScintHitsCollection* scintCollection;
      G4int verbose;
      G4ThreeVector hitPosition;
      G4double hitTime;
      G4double hitEnergy;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif /* MQSCINTSD_HH_ */

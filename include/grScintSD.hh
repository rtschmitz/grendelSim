/*
 * grScintSD.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef GRSCINTSD_HH_
#define GRSCINTSD_HH_

#include "G4VSensitiveDetector.hh"
#include "grScintHit.hh"
#include <G4StepPoint.hh>
#include <fstream>
#include <iostream>

//#include "grScintHitCollection.hh"

class G4Step;
class G4HCofThisEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class grScintSD : public G4VSensitiveDetector
{
  public:
      grScintSD(G4String);
     ~grScintSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      G4bool ProcessHitsEnter(const G4Step*, G4TouchableHistory*);

      void EndOfEvent(G4HCofThisEvent*);
      G4int HCID;

      private:
      grScintHitsCollection* scintCollection;
      G4int verbose;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif

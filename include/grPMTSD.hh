/*
 * grPMTSD.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQPMTSD_HH_
#define MQPMTSD_HH_

#include "G4DataVector.hh"
#include "G4PhysicsVector.hh"
#include "G4VSensitiveDetector.hh"
#include "grPMTHit.hh"
class G4Step;
class G4HCofThisEvent;

class grPMTSD : public G4VSensitiveDetector
{

public:
  grPMTSD(G4String name);
  ~grPMTSD();

  void Initialize(G4HCofThisEvent* HCE);
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory* ROhist);

  //A version of processHits that keeps aStep constant
  G4bool ProcessHits_constStep(const G4Step* aStep,
             G4TouchableHistory* ROhist);
  void EndOfEvent(G4HCofThisEvent* HCE);
  void clear();
  void DrawAll();
  void PrintAll();

  void SetR878_QE(const G4PhysicsVector& effVec) { r878QE = effVec; }
  void SetR7725_QE(const G4PhysicsVector& effVec) { r7725QE = effVec; }
  void SetET9814B_QE(const G4PhysicsVector& effVec) { et9814bQE = effVec; }

  const G4PhysicsVector& GetR878_QE() const { return r878QE; }
  const G4PhysicsVector& GetR7725_QE() const { return r7725QE; }
  const G4PhysicsVector& GetET9814B_QE() const { return et9814bQE; }


  //Initialize the arrays to store pmt positions
//  inline void InitPMTs(G4int nPMTs){
//    if(pmtPositionsX)delete pmtPositionsX;
//    if(pmtPositionsY)delete pmtPositionsY;
//    if(pmtPositionsZ)delete pmtPositionsZ;
//    pmtPositionsX=new G4DataVector(nPMTs);
//    pmtPositionsY=new G4DataVector(nPMTs);
//    pmtPositionsZ=new G4DataVector(nPMTs);

//  }

  //Store a pmt position
//  inline void SetPMTPos(G4int n,G4double x,G4double y,G4double z){
//    if(pmtPositionsX)pmtPositionsX->insertAt(n,x);
//    if(pmtPositionsY)pmtPositionsY->insertAt(n,y);
//    if(pmtPositionsZ)pmtPositionsZ->insertAt(n,z);
//  }

private:
  grPMTHitsCollection* pmtHitCollection;

//  G4DataVector* pmtPositionsX;
//  G4DataVector* pmtPositionsY;
//  G4DataVector* pmtPositionsZ;
  G4PhysicsVector r878QE;
  G4PhysicsVector r7725QE;
  G4PhysicsVector et9814bQE;
  G4int verbose;
};


#endif /* MQPMTSD_HH_ */

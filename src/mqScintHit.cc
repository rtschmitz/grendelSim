/*
 * mqScintHit.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "mqScintHit.hh"
// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4THitsCollection.hh"

G4Allocator<mqScintHit> mqScintHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintHit::mqScintHit():
	trackID(-1), parentID(-1),
	hitEnergy(0.),
	exitEnergy(0.),
//	trackLength(0.),
        copyNo(0),
	particleName(0),
	procName(""),
	creatorVolName(""),
	hitTime(0.),
	exitTime(0.),
	hitPosition(0.,0.,0.),
	exitPosition(0.,0.,0.)

{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintHit::~mqScintHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintHit::mqScintHit(const mqScintHit& right)
  : G4VHit()
{
    trackID         = right.trackID        ;
    parentID        = right.parentID       ;
    hitEnergy            = right.hitEnergy           ;
    exitEnergy            = right.exitEnergy           ;
//    trackLength            = right.trackLength   ;
    copyNo          = right.copyNo         ;
    particleName    = right.particleName   ;
    creatorVolName    = right.creatorVolName   ;
    hitTime  = right.hitTime ;
    exitTime  = right.exitTime ;
    hitPosition     = right.hitPosition    ;
    exitPosition     = right.exitPosition    ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const mqScintHit& mqScintHit::operator=(const mqScintHit& right)
{
    trackID         = right.trackID        ;
    parentID        = right.parentID       ;
    hitEnergy            = right.hitEnergy           ;
    exitEnergy            = right.exitEnergy           ;
//    trackLength            = right.trackLength           ;
    copyNo          = right.copyNo         ;
    particleName    = right.particleName   ;
    creatorVolName    = right.creatorVolName   ;
    hitTime  = right.hitTime ;
    exitTime  = right.exitTime ;
    hitPosition     = right.hitPosition    ;
    exitPosition     = right.exitPosition    ;

    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int mqScintHit::operator==(const mqScintHit& right) const
{
  return (this==&right) ? 1 : 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqScintHit::Draw()
{
  //G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  //if(pVVisManager)
  //{
  //  G4Circle circle(pos);
  //  circle.SetScreenSize(2.);
  //  circle.SetFillStyle(G4Circle::filled);
  //  G4Colour colour(1.,0.,0.);
  //  G4VisAttributes attribs(colour);
  //  circle.SetVisAttributes(attribs);
  //  pVVisManager->Draw(circle);
  //}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqScintHit::Print()
{
	//if(pName.contains("Ge")){ //only print info if it is a Ge
		G4cout  << "  trackID  : " << trackID
//				<< "  Particle : " << particleName
				<< "  Parent ID: " << parentID
				<< "  Energy   : " << G4BestUnit(hitEnergy-exitEnergy,"Energy")
				<< "  Position : " << G4BestUnit(hitPosition,"Length") << G4endl;
	//}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqScintRHit* mqScintHit::ConvertToROOTHit() const{

  mqScintRHit *myROOTHit = new mqScintRHit();

  myROOTHit->SetEDep(this->GetHitEnergy()/MeV-this->GetExitEnergy()/MeV);
  
//  G4ThreeVector hitVec = this->GetHitPosition();
//  G4ThreeVector exitVec = this->GetExitPosition();

//  G4ThreeVector tLength = hitVec-exitVec;
//  myROOTHit->SetTrackLength(tLength.mag()/cm);
  myROOTHit->SetCopyNo(this->GetCopyNo());
  myROOTHit->SetHitPositionX(this->GetHitPosition().getX()/m);
  myROOTHit->SetHitPositionY(this->GetHitPosition().getY()/m);
  myROOTHit->SetHitPositionZ(this->GetHitPosition().getZ()/m);
  myROOTHit->SetExitPositionX(this->GetExitPosition().getX()/m);
  myROOTHit->SetExitPositionY(this->GetExitPosition().getY()/m);
  myROOTHit->SetExitPositionZ(this->GetExitPosition().getZ()/m);
  myROOTHit->SetHitTime(this->GetHitTime()/ns);
  myROOTHit->SetExitTime(this->GetExitTime()/ns);
  myROOTHit->SetParentID(this->GetParentID());
  myROOTHit->SetParticleName(this->GetParticleName());
  myROOTHit->SetProcName(this->GetProcName());
  myROOTHit->SetCreatorVolName(this->GetCreatorVolName());
  myROOTHit->SetTrackID(this->GetTrackID());


  return myROOTHit;

}



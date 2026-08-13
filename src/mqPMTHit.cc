/*
 * mqPMTHit.cc
 *
 * PMT hit class. A PMT hit is equivalent to a detected photo electron.
 * This class allows to store all necessary information about
 * PMT hits such as the number of PE, total energy deposit ... .
 * Hit informations are assigned in mqPMTSD (= sensetive detector).
 * It inherits from G4VHit which is required by the SD.
 *
 * Created on: 22.4.2019
 * Author: schmitz
 */


#include "mqPMTHit.hh"
#include "mqPMTRHit.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
//#include "G4VPhysicalVolume.hh"
// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <vector>
#include "TVector3.h"
#include "TObject.h"
#include "TMath.h"
#include <iomanip>


G4Allocator<mqPMTHit> mqPMTHitAllocator;

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqPMTHit::mqPMTHit()
  :eventID(-1), parentID(-1),
   pmtNumber(-1),photons(0),//physVol(0),
   drawit(false),energy(0.),
   hitTime(0.), finalHitTime(0.),
   position(), trackID(-1),
   totE(0.), scintToPMT(false)

{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqPMTHit::~mqPMTHit()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqPMTHit::mqPMTHit(const mqPMTHit &right)
  : G4VHit()
{
  pmtNumber = right.pmtNumber;
  photons   = right.photons;
  //physVol   = right.physVol;
  drawit    = right.drawit;
  energy    = right.energy;
  hitTime   = right.hitTime;
  finalHitTime  = right.finalHitTime;
  position  = right.position;
  totE	    = right.totE;
  trackID   = right.trackID;
  parentID  = right.parentID;
  eventID   = right.eventID;
  scintToPMT = right.scintToPMT;

}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
const mqPMTHit& mqPMTHit::operator=(const mqPMTHit &right){
  pmtNumber = right.pmtNumber;
  photons   = right.photons;
  //physVol   = right.physVol;
  drawit    = right.drawit;
  energy    = right.energy;
  hitTime   = right.hitTime;
  finalHitTime  = right.finalHitTime;
  position  = right.position;
  totE      = right.totE;
  trackID   = right.trackID;
  parentID  = right.parentID;
  eventID   = right.eventID;
  scintToPMT = right.scintToPMT;
  return *this;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4int mqPMTHit::operator==(const mqPMTHit &right) const{
  return (pmtNumber == right.pmtNumber);

}



//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTHit::Draw(){
	//ReDraw only the PMTs that have hit counts > 0
	//if(drawit&&physVol){
    //
	//	//Also need a physical volume to be able to draw anything
	//	G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    //
	//	//Make sure that the VisManager exists
	//	if(pVVisManager){
    //
	//		G4VisAttributes attribs(G4Colour(1.,0.,0.));
	//		attribs.SetForceSolid(true);
	//		G4RotationMatrix rot;
    //
	//		//If a rotation is defined use it
	//		if(physVol->GetRotation()){
    //
	//			rot=*(physVol->GetRotation());
	//		}
	//		//Create transform
	//		G4Transform3D trans(rot,physVol->GetTranslation());
	//		//Draw it
	//		pVVisManager->Draw(*physVol,attribs,trans);
	//	}
	//}

}


mqPMTRHit* mqPMTHit::ConvertToROOTHit() const{

  mqPMTRHit *myROOTHit = new mqPMTRHit();

  myROOTHit->SetTrackID(this->GetTrackID());
//  myROOTHit->SetPhotonCount(this->GetPhotonCount());
  myROOTHit->SetFirstHitTime(this->GetHitTime()/ns);
//  G4ThreeVector pos = this->GetHitPosition();
//  Double_t xPos = pos.getX();
//  Double_t yPos = pos.getY();
//  Double_t zPos = pos.getZ();
  ///////////////////////////////////////
//  TVector3 rPos;
//  rPos.SetXYZ(xPos,yPos,zPos);
//  rPos.SetX(xPos);
  ///////////////////////////////////////
//  myROOTHit->SetScintToPMT(this->GetScintToPMT());
//  myROOTHit->SetHitPosition(rPos);
//  myROOTHit->SetInitialEDep(this->GetInitialEDep()/eV);
//  myROOTHit->SetLastHitTime(this->GetLastHitTime()/ns);
  myROOTHit->SetPMTNumber(this->GetPMTNumber());
  myROOTHit->SetParentID(this->GetParentID());
//  myROOTHit->SetTotalEDep(this->GetEDep()/eV);

/*G4cout << "X-Position (G4): " << xPos << G4endl;
G4cout << "Y-Position: " << yPos << G4endl;
G4cout << "Z-Position: " << zPos << G4endl;

G4cout << "X-Position (ROOT): " << rPos.X() << G4endl;
G4cout << "Y-Position: " << rPos.Y() << G4endl;
G4cout << "Z-Position: " << rPos.Z() << G4endl;
*/
return myROOTHit;

}
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
bool mqPMTHit::compareHits(mqPMTHit* const &a, mqPMTHit* const &b){
	G4double aTime = a->GetLastHitTime();//GetHitTime();
	G4double bTime = b->GetLastHitTime();//GetHitTime();
	return (aTime < bTime);
}


//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTHit::Print(){

	G4cout
	     << "  trackID: " << trackID << " pmtNumber: " << pmtNumber
	     << " Number of photons: "
	     << std::setw(7) << photons
	     << " Parent ID: "
	     << std::setw(7) << parentID
	     << " Energy: "
	     << std::setw(7) << G4BestUnit(energy,"Energy")
	     << " Hit time: "
	     << std::setw(7) << G4BestUnit( hitTime,"Time")
	     << "ETot: "
	     << std::setw(7) << G4BestUnit(totE,"Energy")
	     << G4endl;

}



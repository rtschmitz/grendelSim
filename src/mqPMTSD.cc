/*
 * mqPMTSD.cc
 *
 *  Created on: 22-4-2019
 *      Author: schmitz
 */
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "mqPMTSD.hh"
#include "mqPMTHit.hh"
#include "mqDetectorConstruction.hh"

//#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4ParticleDefinition.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleDefinition.hh"
#include "G4RunManager.hh"
#include <math.h>
#include <string>
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqPMTSD::mqPMTSD(G4String name)
  :G4VSensitiveDetector(name),pmtHitCollection(0),
//	pmtPositionsX(0),
//	pmtPositionsY(0),
//	pmtPositionsZ(0),
	verbose(0)
{

	collectionName.insert("pmtHitCollection");
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqPMTSD::~mqPMTSD()

{
//	if(pmtHitCollection) delete pmtHitCollection;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTSD::Initialize(G4HCofThisEvent* HCE){
  pmtHitCollection = new mqPMTHitsCollection
                      (SensitiveDetectorName,collectionName[0]);
  //Store collection with event and keep ID
  static G4int HCID = -1;
  if(HCID<0){
    HCID = GetCollectionID(0);
  }
  HCE->AddHitsCollection( HCID, pmtHitCollection );
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4bool mqPMTSD::ProcessHits(G4Step* ,G4TouchableHistory* ){
  return false;
}

//Generates a hit and uses the postStepPoint's mother volume replica number
//PostStepPoint because the hit is generated manually when the photon is
//absorbed by the photocathode
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
G4bool mqPMTSD::ProcessHits_constStep(const G4Step* aStep,
				       G4TouchableHistory* ){

  //need to know if this is an optical photon
  if(aStep->GetTrack()->GetDefinition() != G4OpticalPhoton::OpticalPhotonDefinition()) return false;

//  const G4VTouchable* aStep->GetPostStepPoint()->GetTouchable() = aStep->GetPostStepPoint()->GetTouchable();

//  const G4VProcess* process = aStep->GetPostStepPoint()->GetProcessDefinedStep();

  G4int volCopyNo = aStep->GetPostStepPoint()->GetTouchable()->GetCopyNumber();
  G4int copyNo=0;
  if(volCopyNo<6){

  //get the detector to retrieve layer information
//  mqDetectorConstruction* detector = (mqDetectorConstruction*)G4RunManager::GetRunManager()
//                                            ->GetUserDetectorConstruction();
  //get number of bars per layer
//  G4int nBarPerLayer = detector->GetNBarPerLayer();

  G4int nBarPerLayer = 9*6*4;

  char layerNumberChar = aStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName().back();
  G4int layerNumber = layerNumberChar-48;
  G4int subStackCopyNo = aStep->GetPostStepPoint()->GetTouchable()->GetCopyNumber(2);
//  G4String subStackName = aStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName();
//  G4cout << "substackCopyNoName : " << subStackName << G4endl;
  copyNo = nBarPerLayer*layerNumber + volCopyNo + 4*subStackCopyNo;
//  G4cout << "copyNo parts: " << layerNumber << " " << aStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName() << " " << volCopyNo << " " << subStackCopyNo << G4endl;
  } else {copyNo = volCopyNo;}

  G4double particleEnergy=aStep->GetTrack()->GetTotalEnergy();
  G4double particleWavelength=(1240/particleEnergy)*eV; //wavelength in nm
  //G4cout << "particle wavelength is: " << particleWavelength << G4endl;
  G4double detProb=0;
  G4bool det=false;


// this is where we assign different QEs based on PMT. Just using ET universally for now
/*  
  if(pmtNumber==1|pmtNumber==3|pmtNumber==4|pmtNumber==10){
  	detProb = GetET9814B_QE().Value(particleWavelength);
//	G4cout << "Hit ET9814B, detProb is: " << detProb << G4endl;
  } else if(pmtNumber==13|pmtNumber==15){
  	detProb = GetR7725_QE().Value(particleWavelength);
//	G4cout << "Hit R7725, detProb is: " << detProb << G4endl;
  } else {
  	detProb = GetR878_QE().Value(particleWavelength);
//	G4cout << "Hit R878, detProb is: " << detProb << G4endl;
  }	
*/

  	detProb = GetR878_QE().Value(particleWavelength);
  	//if(copyNo==1000) detProb = GetR878_QE().Value(particleWavelength);
	//else detProb = GetET9814B_QE().Value(particleWavelength);
  
	det=(G4UniformRand()<detProb);
  if(det){
//	G4cout << "successfully detected in PMT: " << pmtNumber << G4endl;
  //Get information from the optical photon which hit the pmt
  G4double energyDeposit = aStep->GetTotalEnergyDeposit();
  G4double globalTime = aStep->GetPostStepPoint()->GetGlobalTime(); //time since the event was created

  G4ThreeVector hitPos = aStep->GetPostStepPoint()->GetPosition();
  G4double xy_mag = sqrt(pow(hitPos.x(),2)+pow(hitPos.y(),2));
  G4int pid = aStep->GetTrack()->GetParentID();

  //Find the correct hit collection

  G4int n = pmtHitCollection->entries();
  //mqPMTHit* hit=NULL;
  //for(G4int i=0; i < n; i++){
  //  if((*pmtHitCollection)[i]->GetPMTNumber() == pmtNumber){
  //    hit=(*pmtHitCollection)[i];
  //    break;
  //  }
  //}

  //if(hit==NULL){//this pmt wasnt previously hit in this event
  //
	mqPMTHit*
	hit = new mqPMTHit(); //so create new hit

    hit->SetPMTNumber(copyNo);
    //hit->SetPMTPhysVol(physVol);
    hit->SetParentID(pid);
    hit->SetTrackID(aStep->GetTrack()->GetTrackID());
    hit->SetInitialEDep(energyDeposit); // TODO this is EDep of first hit
    //hit->IncEDep(energyDeposit);

    hit->SetScintToPMT(true);
    hit->SetHitTime(globalTime);// this is hit time of first hit
    hit->SetHitPosition(hitPos);//this is only hit position of first hit
    //hit->SetTrackLength(aStep->GetTrack()->GetTrackLength());
    pmtHitCollection->insert(hit);
    //hit->SetPMTPos((*pmtPositionsX)[pmtNumber],(*pmtPositionsY)[pmtNumber],
	//	   (*pmtPositionsZ)[pmtNumber]);

  // }
  //increment hit for the selected pmt
  //
    //hit->IncEDep(energyDeposit);
  	//hit->SetLastHitTime(globalTime);
  	//hit->IncPhotonCount();
//////////////////////////
//G4cout << "X-Position: " << hitPos.getX() << G4endl;
//G4cout << "Y-Position: " << hitPos.getY() << G4endl;
//G4cout << "Z-Position: " << hitPos.getZ() << G4endl;
/////////////////////////
  // delete hit;
  } //else {G4cout << "not detected" << G4endl;}
return true;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTSD::EndOfEvent(G4HCofThisEvent* ){
	G4String collName = pmtHitCollection->GetName() ;
	G4int NbHits = pmtHitCollection->entries();
	if (verbose > 0){
			G4cout << "\n-------->Hits Collection "<< collName << ": in this event they are " << NbHits
					<< " hits in the pmts: " << G4endl;

			for (G4int i=0;i<NbHits;i++){
				(*pmtHitCollection)[i]->Print();
				//G4cout <<"Hit time : " << (*pmtHitCollection)[i]->GetHitTime() << G4endl;;

			}

	}



}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTSD::clear(){
delete pmtHitCollection;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTSD::DrawAll(){
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
void mqPMTSD::PrintAll(){
}




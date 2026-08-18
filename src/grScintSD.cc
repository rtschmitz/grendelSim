/*
 * grScintSD.cc
 *
 *  Created on: 18.04.2017
 *      Author: schmitz
 */


#include "grScintSD.hh"
#include "grScintHit.hh"
#include "grDetectorConstruction.hh"

#include "grVolumeID.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include <G4StepPoint.hh>
#include "G4TouchableHistory.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4VProcess.hh"
#include "G4ios.hh"
#include <fstream>
#include <iostream>
#include "Randomize.hh"

using namespace::std;


namespace {

G4bool IsGargoyleActiveVolume(const G4String& volumeName)
{
  return volumeName.contains("gargoyle_scint_phys") ||
         volumeName.contains("gargoyle_si_layer1") ||
         volumeName.contains("gargoyle_si_layer2") ||
         volumeName.contains("gargoyle_si_layer3") ||
         volumeName.contains("gargoyle_si_layer5") ||
         volumeName.contains("gargoyle_si_layer1_z_phys") ||
         volumeName.contains("gargoyle_si_layer2_z_phys") ||
         volumeName.contains("gargoyle_si_layer3_z_phys") ||
         volumeName.contains("gargoyle_si_layer5_z_phys");
}

G4int GetGargoyleCopyNo(const G4Step* aStep, G4bool usePostPoint)
{
  const G4StepPoint* point = usePostPoint
      ? aStep->GetPostStepPoint() : aStep->GetPreStepPoint();
  if (!point || !point->GetPhysicalVolume() || !point->GetTouchable() ||
      !IsGargoyleActiveVolume(point->GetPhysicalVolume()->GetName())) return -1;
  return grVolumeID::FromTouchable(point->GetTouchable());
}

G4int GetOriginVolumeID(const G4Step* aStep)
{
  return (aStep && aStep->GetTrack())
      ? grVolumeID::FromTouchable(aStep->GetTrack()->GetOriginTouchable())
      : grVolumeID::Unknown;
}

} // namespace


grScintSD::grScintSD(G4String name)
:G4VSensitiveDetector(name),scintCollection(0), verbose(0)
{
  G4String HCname;
  collectionName.insert(HCname = "scintCollection");

  //HCID= -1;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

grScintSD::~grScintSD(){

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void grScintSD::Initialize(G4HCofThisEvent* HCE)
{
  scintCollection = new grScintHitsCollection
                          (SensitiveDetectorName,collectionName[0]);


static G4int HCID = -1;
  if(HCID < 0)
  { HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]); }
  HCE->AddHitsCollection( HCID, scintCollection );

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool grScintSD::ProcessHits(G4Step* ,G4TouchableHistory* ){
  return false;
}

G4bool grScintSD::ProcessHitsEnter(const G4Step* aStep,G4TouchableHistory*)
{


//  G4int copyNo=volCopyNo;

  // GARGOYLE active layers are simple placements and do not have the old
  // GRENDEL replica depth used by GetCopyNumber(2)/GetCopyNumber(4).  Handle
  // them first using only the depth-0 physical-volume copy number.
  G4int copyNo = GetGargoyleCopyNo(aStep, true);

  if (copyNo < 0) {
    G4int volCopyNo = aStep->GetPostStepPoint()->GetTouchable()->GetCopyNumber(2);
    copyNo=0;
    if(volCopyNo<6){

    //get the detector to retrieve layer information
  //  grDetectorConstruction* detector = (grDetectorConstruction*)G4RunManager::GetRunManager()
  //                                            ->GetUserDetectorConstruction();
    //get number of bars per layer
  //  G4int nBarPerLayer = detector->GetNBarPerLayer();

    G4int nBarPerLayer = 9*6*4;

    char layerNumberChar = aStep->GetPostStepPoint()->GetTouchable()->GetVolume(2)->GetName().back();
    G4int layerNumber = layerNumberChar-48;
    G4int subStackCopyNo = aStep->GetPostStepPoint()->GetTouchable()->GetCopyNumber(4);

    copyNo = nBarPerLayer*layerNumber + volCopyNo + 4*subStackCopyNo;
    } else {copyNo = volCopyNo;}
  }

  //G4cout << "Exiting scint! Copy Number is: " << copyNo << G4endl;





  const G4VProcess* creaProc= aStep->GetTrack()->GetCreatorProcess();
  G4String creaProcName;
  if (creaProc) creaProcName = creaProc->GetProcessName();
  else creaProcName = "0";

  const G4int originVolumeID = GetOriginVolumeID(aStep);
//  G4cout << vertexVolume << " " << G4endl;
  const G4double kineticEnergy = aStep->GetPostStepPoint()->GetKineticEnergy();
  grScintHit* hit = new grScintHit();
    hit->SetTrackID  (aStep->GetTrack()->GetTrackID());          // trackID
    hit->SetParentID (aStep->GetTrack()->GetParentID());         // parentID
    hit->SetKineticEnergy(kineticEnergy);
    hit->SetHitTime(aStep->GetPostStepPoint()->GetGlobalTime());
    hit->SetHitPosition(aStep->GetPostStepPoint()->GetPosition());
    hit->SetDirection(aStep->GetPostStepPoint()->GetMomentumDirection());
    //hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetParticleName() );
    hit->SetParticleName( aStep->GetTrack()->GetDefinition()->GetPDGEncoding() );
    hit->SetCopyNo(copyNo); // sensitive-volume identifier
    hit->SetProcName(creaProcName); //creation process of the particle that caused the hit
    hit->SetOriginVolumeID(originVolumeID); //creation volume of the particle that caused the hit
    hit->SetEntering(true);
    scintCollection->insert(hit);
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool grScintSD::ProcessHitsExit(const G4Step* aStep, G4TouchableHistory*)
{
  G4int copyNo = GetGargoyleCopyNo(aStep, false);
  if (copyNo < 0) {
    const G4StepPoint* pre = aStep->GetPreStepPoint();
    G4int volCopyNo = pre->GetTouchable()->GetCopyNumber(2);
    if (volCopyNo < 6) {
      const G4int nBarPerLayer = 9*6*4;
      const G4int layerNumber = pre->GetTouchable()->GetVolume(2)->GetName().back()-'0';
      const G4int subStackCopyNo = pre->GetTouchable()->GetCopyNumber(4);
      copyNo = nBarPerLayer*layerNumber + volCopyNo + 4*subStackCopyNo;
    } else {
      copyNo = volCopyNo;
    }
  }

  const G4Track* track = aStep->GetTrack();
  const G4StepPoint* boundary = aStep->GetPostStepPoint();
  const G4VProcess* creator = track->GetCreatorProcess();
  grScintHit* hit = new grScintHit();
  hit->SetTrackID(track->GetTrackID());
  hit->SetParentID(track->GetParentID());
  hit->SetKineticEnergy(boundary->GetKineticEnergy());
  hit->SetHitTime(boundary->GetGlobalTime());
  hit->SetHitPosition(boundary->GetPosition());
  hit->SetDirection(boundary->GetMomentumDirection());
  hit->SetParticleName(track->GetDefinition()->GetPDGEncoding());
  hit->SetCopyNo(copyNo);
  hit->SetProcName(creator ? creator->GetProcessName() : "0");
  hit->SetOriginVolumeID(GetOriginVolumeID(aStep));
  hit->SetEntering(false);
  scintCollection->insert(hit);
  return true;
}

void grScintSD::EndOfEvent(G4HCofThisEvent*)
{
  if (verbose>0) {

     G4int NbHits = scintCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are " << NbHits
            << " hits in the scint : " << G4endl;
     for (G4int i=0;i<NbHits;i++){
       //if ((*scintCollection)[i]->GetCopyNo() > 0)
       (*scintCollection)[i]->Print();
     }

   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

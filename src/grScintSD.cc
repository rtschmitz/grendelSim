/*
 * grScintSD.cc
 *
 *  Created on: 18.04.2017
 *      Author: schmitz
 */


#include "grScintSD.hh"
#include "grScintHit.hh"
#include "grDetectorConstruction.hh"

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
         volumeName.contains("gargoyle_si_layer1_phys") ||
         volumeName.contains("gargoyle_si_layer2_phys") ||
         volumeName.contains("gargoyle_si_layer3_phys") ||
         volumeName.contains("gargoyle_si_layer5_phys") ||
         volumeName.contains("gargoyle_si_layer1_z_phys") ||
         volumeName.contains("gargoyle_si_layer2_z_phys") ||
         volumeName.contains("gargoyle_si_layer3_z_phys") ||
         volumeName.contains("gargoyle_si_layer5_z_phys");
}

G4int GargoyleLayerBase(const G4String& volumeName)
{
  // Keep the layer numbering explicit and separated from the old GRENDEL
  // bar/stacks scheme.  The GARGOYLE geometry places each segment with these
  // copy-number bases, but this fallback also protects us if the placement copy
  // numbers are later changed back to local 0..N segment IDs.
  if (volumeName.contains("gargoyle_scint_phys"))    return 0;
  if (volumeName.contains("gargoyle_si_layer1_z_phys")) return 10000;
  if (volumeName.contains("gargoyle_si_layer2_z_phys")) return 20000;
  if (volumeName.contains("gargoyle_si_layer3_z_phys")) return 30000;
  if (volumeName.contains("gargoyle_si_layer5_z_phys")) return 50000;
  if (volumeName.contains("gargoyle_si_layer1_phys")) return 1000;
  if (volumeName.contains("gargoyle_si_layer2_phys")) return 2000;
  if (volumeName.contains("gargoyle_si_layer3_phys")) return 3000;
  if (volumeName.contains("gargoyle_si_layer5_phys")) return 5000;
  return -1;
}

G4int GetGargoyleCopyNo(const G4Step* aStep, G4bool usePostPoint)
{
  const G4StepPoint* stepPoint = usePostPoint ?
      aStep->GetPostStepPoint() : aStep->GetPreStepPoint();

  if (!stepPoint || !stepPoint->GetPhysicalVolume() || !stepPoint->GetTouchable()) {
    return -1;
  }

  const G4String volumeName = stepPoint->GetPhysicalVolume()->GetName();
  if (!IsGargoyleActiveVolume(volumeName)) {
    return -1;
  }

  const G4int layerBase = GargoyleLayerBase(volumeName);
  const G4int localCopyNo = stepPoint->GetTouchable()->GetCopyNumber(); // depth 0 only

  // Alternating z strips are daughters of replicated two-strip cells.
  // Reconstruct the original sequential bin number for stable output IDs.
  if (volumeName.contains("_z_phys_") &&
      stepPoint->GetTouchable()->GetHistoryDepth() >= 1) {
    const G4int cellCopyNo = stepPoint->GetTouchable()->GetCopyNumber(1);
    return layerBase + 2 * cellCopyNo + localCopyNo;
  }

  // In the generated GARGOYLE geometry, placement copy numbers are already
  // global within their layer-specific ranges; preserve them exactly.
  if (localCopyNo >= layerBase) {
    return localCopyNo;
  }

  // If a future geometry uses local segment copy numbers 0..N for every layer,
  // add the layer offset here.
  return layerBase + localCopyNo;
}

G4String GetVertexVolumeNameSafe(const G4Step* aStep)
{
  if (!aStep || !aStep->GetTrack() || !aStep->GetTrack()->GetLogicalVolumeAtVertex()) {
    return "0";
  }
  return aStep->GetTrack()->GetLogicalVolumeAtVertex()->GetName();
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

  G4String vertexVolume = GetVertexVolumeNameSafe(aStep);
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
    hit->SetCreatorVolName(vertexVolume); //creation volume of the particle that caused the hit
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
  hit->SetCreatorVolName(GetVertexVolumeNameSafe(aStep));
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

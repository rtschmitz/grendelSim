/*
 * grUserEventInformation.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "grUserEventInformation.hh"
#include "grPhotonTrack.hh"
#include "grGammaTrack.hh"
#include "grNeutronTrack.hh"
#include "grMuonTrack.hh"
#include "grElectronTrack.hh"
#include "grMCPTrack.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <vector>

template<class SequenceType>
void DeleteAll(SequenceType& sequence) {
  for (typename SequenceType::iterator it = sequence.begin(); it != sequence.end(); ++it) {
    delete (*it);
  }
  sequence.clear();
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grUserEventInformation::grUserEventInformation()
  :GammaTracks(0),NbOfGammaTracks(0),
   NeutronTracks(0),NbOfNeutronTracks(0),
   PhotonTracks(0),NbOfPhotonTracks(0),
   MuonTracks(0),NbOfMuonTracks(0),
   ElectronTracks(0),NbOfElectronTracks(0),
   MCPTracks(0),NbOfMCPTracks(0),
   PMTHits(0),NbOfPMTHits(0),
   ScintHits(0), NbOfScintHits(0),
   peCountPMT(0),absorptionCount(0),boundaryAbsorptionCount(0),
   pmtsAboveThreshold(0),
   photonCount_Scint(0),
   photonCount(0),
   photonCount_Cheren(0),
   //primMutrackLength(0.),
   Edep_MeV(0.),
   runID(-1), eventID(-1),
   photonLastTrackID(-1),
  gammaLastTrackID(-1),
  neutronLastTrackID(-1),
  muonLastTrackID(-1),
  electronLastTrackID(-1),
  mcpLastTrackID(-1),
   gammaOutScintillator(false),
   muonTrig(false),
   trident(false),
   tridentGamma(false),
   kaonCavern(false),
   scintToPMT(false),
   barHit(0),
   panelHit(0),
   slabHit(0),
   eventWeight(0),
   processID(0)
{
}


//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grUserEventInformation::~grUserEventInformation()
{
  this->Reset();
}
void grUserEventInformation::Initialize() {

}
void grUserEventInformation::Finalize() {

/*
  // Sort the track collection, so that time(track_n)<time(track_n+1), because this ordering is needed in the offline analysis
  std::sort(PhotonTracks.begin(), PhotonTracks.end(),
        grPhotonTrack::compareHits);

  std::sort(GammaTracks.begin(), GammaTracks.end(),
        grGammaTrack::compareHits);

  std::sort(NeutronTracks.begin(), NeutronTracks.end(),
        grNeutronTrack::compareHits);

  std::sort(MuonTracks.begin(), MuonTracks.end(),
        grMuonTrack::compareHits);

  std::sort(ElectronTracks.begin(), ElectronTracks.end(),
        grElectronTrack::compareHits);

  std::sort(MCPTracks.begin(), MCPTracks.end(),
        grMCPTrack::compareHits);

  // Sort the hit collection, so that time(hit_n)<time(hit_n+1), because this ordering is needed in the offline analysis
  std::sort(PMTHits.begin(), PMTHits.end(),
      grPMTHit::compareHits);

  // Sort the hit collection, so that time(hit_n)<time(hit_n+1), because this ordering is needed in the offline analysis
//  std::sort(ScintHits.begin(), ScintHits.end(),
//      grScintHit::compareHits);
*/
  NbOfNeutronTracks = NeutronTracks.size();
  NbOfGammaTracks = GammaTracks.size();
  NbOfPhotonTracks = PhotonTracks.size();
  NbOfMuonTracks = MuonTracks.size();
  NbOfElectronTracks = ElectronTracks.size();
  NbOfMCPTracks = MCPTracks.size();
  NbOfPMTHits = PMTHits.size();
  NbOfScintHits = ScintHits.size();
}

void grUserEventInformation::Reset() {

//        if(!GammaTracks.empty()) DeleteAll(GammaTracks);
//        if(!NeutronTracks.empty()) DeleteAll(NeutronTracks);
//        if(!PhotonTracks.empty()) DeleteAll(PhotonTracks);
//        if(!MuonTracks.empty()) DeleteAll(MuonTracks);
//        if(!ElectronTracks.empty()) DeleteAll(ElectronTracks);
//        if(!MCPTracks.empty()) {DeleteAll(MCPTracks);}
//        if(!PMTHits.empty()) {std::cout << "deleting PMTHits" << std::endl; DeleteAll(PMTHits);}
//        if(!ScintHits.empty()) DeleteAll(ScintHits);

  PMTHits.clear(); // G4VHit takes care of the delete
  ScintHits.clear();// G4VHit takes care of the delete

  NbOfNeutronTracks = 0;
  NbOfGammaTracks   = 0;
  NbOfPhotonTracks = 0;
  NbOfMuonTracks   = 0;
  NbOfElectronTracks   = 0;
  NbOfMCPTracks   = 0;
  NbOfPMTHits      = 0;
  NbOfScintHits     = 0;
}

void grUserEventInformation::SetEventID(G4int EventID) {
  this->eventID = EventID;
}

G4int grUserEventInformation::GetEventID()const{
  return this->eventID;
}

void grUserEventInformation::SetRunID(G4int RunID) {
  this->runID = RunID;
}

G4int grUserEventInformation::GetRunID()const{
  return this->runID;
}

//////////////////////gamma//////////////////////////////////

void grUserEventInformation::AddGammaTrack(grGammaTrack *mtrack){
  this->GammaTracks.push_back(mtrack);
}

grGammaTrack* grUserEventInformation::GetGammaTrack(G4int trackID){

  G4int ID = -1;

  for (G4int i = 0; i < GammaTracks.size(); i++){
     if (this->GammaTracks.at(i)->GetTrackID() == trackID)

       ID = i;

  }
  return this->GammaTracks.at(ID);

}


grGammaTrackVector* grUserEventInformation::GetGammaTracks(){
  return &GammaTracks;
}

///////////////////////Neutron/////////////////////////////

void grUserEventInformation::AddNeutronTrack(grNeutronTrack *ntrack) {

  this->NeutronTracks.push_back(ntrack);
}

grNeutronTrack* grUserEventInformation::GetNeutronTrack(G4int trackID){

  G4int ID = -1;
  for (G4int i = 0; i < NeutronTracks.size(); i++){
     if (this->NeutronTracks.at(i)->GetTrackID() == trackID)

       ID = i;

  }
  return this->NeutronTracks.at(ID);

}

grNeutronTrackVector* grUserEventInformation::GetNeutronTracks(){
  return &NeutronTracks;
}

////////////////////////muon///////////////////////////

void grUserEventInformation::AddMuonTrack(grMuonTrack *ntrack) {

  this->MuonTracks.push_back(ntrack);
}

grMuonTrack* grUserEventInformation::GetMuonTrack(G4int trackID){

  G4int ID = -1;
  for (G4int i = 0; i < MuonTracks.size(); i++){
     if (this->MuonTracks.at(i)->GetTrackID() == trackID)

       ID = i;

  }
  return this->MuonTracks.at(ID);

}

grMuonTrackVector* grUserEventInformation::GetMuonTracks(){
  return &MuonTracks;
}


////////////////////////electron///////////////////////////

void grUserEventInformation::AddElectronTrack(grElectronTrack *ntrack) {

  this->ElectronTracks.push_back(ntrack);
}

grElectronTrack* grUserEventInformation::GetElectronTrack(G4int trackID){

  G4int ID = -1;
  for (G4int i = 0; i < ElectronTracks.size(); i++){
     if (this->ElectronTracks.at(i)->GetTrackID() == trackID)

       ID = i;

  }
  return this->ElectronTracks.at(ID);

}

grElectronTrackVector* grUserEventInformation::GetElectronTracks(){
  return &ElectronTracks;
}


////////////////////////mCP///////////////////////////

void grUserEventInformation::AddMCPTrack(grMCPTrack *ntrack) {

  this->MCPTracks.push_back(ntrack);
}

grMCPTrack* grUserEventInformation::GetMCPTrack(G4int trackID){
  G4int ID = -1;
    for (G4int i = 0; i < MCPTracks.size(); i++){
      if (this->MCPTracks.at(i)->GetTrackID() == trackID)

        ID = i;
  }
  return this->MCPTracks.at(ID);

}

grMCPTrackVector* grUserEventInformation::GetMCPTracks(){
  return &MCPTracks;
}


////////////////////photon////////////////////////////

void grUserEventInformation::AddPhotonTrack(grPhotonTrack *ptrack) {

  this->PhotonTracks.push_back(ptrack);

}

grPhotonTrack* grUserEventInformation::GetPhotonTrack(G4int trackID){

  G4int ID = -1;
  for (G4int i = 0; i < PhotonTracks.size(); i++){
     if (this->PhotonTracks.at(i)->GetTrackID() == trackID)

       ID = i;

  }
  return this->PhotonTracks.at(ID);

}


grPhotonTrackVector* grUserEventInformation::GetPhotonTracks(){
  return &PhotonTracks;
}

//////////////////////PMTHit////////////////////////

void grUserEventInformation::AddPMTHit(grPMTHit *hit) {
  this->PMTHits.push_back(hit);
}

grPMTHitVector* grUserEventInformation::GetPMTHits(){
  return &PMTHits;
}

//////////////////ScintHit////////////////////////////

void grUserEventInformation::AddScintHit(grScintHit *hit) {
  this->ScintHits.push_back(hit);
}

grScintHitVector* grUserEventInformation::GetScintHits(){
  return &ScintHits;
}

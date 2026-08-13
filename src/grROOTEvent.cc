/*
 * grROOTEvent.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "grROOTEvent.hh"
#include "grPhotonTrack.hh"
#include "grGammaTrack.hh"
#include "grNeutronTrack.hh"
#include "grMuonTrack.hh"
#include "grElectronTrack.hh"
#include "grMCPTrack.hh"
#include "grPMTRHit.hh"
#include "grScintRHit.hh"
#include <algorithm>
#include <vector>

template<class SequenceType>
void DeleteAll(SequenceType& sequence) {
  for (typename SequenceType::iterator it = sequence.begin(); it != sequence.end(); ++it) {
    delete (*it);
  }
  sequence.clear();
}
ClassImp(grROOTEvent)
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grROOTEvent::grROOTEvent()
  : eventID(-1),
   GammaTracks(0),NbOfGammaTracks(0),
   NeutronTracks(0),NbOfNeutronTracks(0),
   PhotonTracks(0),NbOfPhotonTracks(0),
   MuonTracks(0),NbOfMuonTracks(0),
   ElectronTracks(0),NbOfElectronTracks(0),
   MCPTracks(0),NbOfMCPTracks(0),
   PMTHits(0),NbOfPMTHits(0),
   ScintRHits(0), NbOfScintRHits(0),
   nbOfCerenkovPhotons(0),nbOfScintillationPhotons(0),absorptionCount(0),
   boundaryAbsorptionCount(0),
   pmtsAboveTrigger(0),
   Edep_MeV(0.),
//   gammaOutScintillator(false),
// scintToPMT(false),
   trident(false),
   tridentGamma(false),
   kaonCavern(false),
   muonTrig(false),
   barHit(0),
   slabHit(0),
   panelHit(0),
   eventWeight(0.),
   processID(0)
{
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grROOTEvent::~grROOTEvent()
{
  this->Reset();
}
void grROOTEvent::Initialize() {

}
void grROOTEvent::Finalize() {

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
      grPMTRHit::compareHits);
*/
//  //Sort the hit collection, so that time(hit_n)<time(hit_n+1), because this ordering is needed in the offline analysis
//  std::sort(ScintRHits.begin(), ScintRHits.end(),
//      grScintRHit::compareHits);

  NbOfNeutronTracks = NeutronTracks.size();
  NbOfGammaTracks = GammaTracks.size();
  NbOfPhotonTracks = PhotonTracks.size();
  NbOfMuonTracks = MuonTracks.size();
  NbOfElectronTracks = ElectronTracks.size();
  NbOfMCPTracks = MCPTracks.size();
  NbOfPMTHits = PMTHits.size();
  NbOfScintRHits = ScintRHits.size();
}

void grROOTEvent::Reset() {
  //Calls the destructor of the elements of the vector. Afterwards the elements are removed from the vector

  if(!GammaTracks.empty()) DeleteAll(GammaTracks);
  if(!NeutronTracks.empty()) DeleteAll(NeutronTracks);
  if(!PhotonTracks.empty()) DeleteAll(PhotonTracks);
  if(!MuonTracks.empty()) DeleteAll(MuonTracks);
  if(!ElectronTracks.empty()) DeleteAll(ElectronTracks);
  if(!MCPTracks.empty()) DeleteAll(MCPTracks);
  if(!PMTHits.empty()) DeleteAll(PMTHits);
  if(!ScintRHits.empty()) DeleteAll(ScintRHits);


  NbOfNeutronTracks = 0;
  NbOfGammaTracks    = 0;
  NbOfPhotonTracks  = 0;
  NbOfMuonTracks    = 0;
  NbOfElectronTracks    = 0;
  NbOfMCPTracks    = 0;
  NbOfPMTHits       = 0;
  NbOfScintRHits   = 0;
}


//////////////////////gamma/////////////////////////////////////

void grROOTEvent::AddGammaTrack(grGammaTrack *mtrack) {

  this->GammaTracks.push_back(mtrack);
}

void grROOTEvent::SetGammaTracks(grGammaTrackVector *trackVec){
  this->GammaTracks = *trackVec;
}


grGammaTrack* grROOTEvent::GetGammaTrack(Int_t trackID) {

    Int_t ID = -1;
    for (Int_t i = 0; i < GammaTracks.size(); i++){
        if (this->GammaTracks.at(i)->GetTrackID() == trackID){
            ID = i;
            break;
        }
    }
    if(ID<0)
        return NULL;
    else
        return this->GammaTracks.at(ID);

}


grGammaTrackVector* grROOTEvent::GetGammaTracks() {
  return &GammaTracks;
}

/////////////////////////Neutron/////////////////////////////////////////////

void grROOTEvent::SetNeutronTracks(grNeutronTrackVector *trackVec){
  this->NeutronTracks = *trackVec;
}



void grROOTEvent::AddNeutronTrack(grNeutronTrack *ntrack) {

  this->NeutronTracks.push_back(ntrack);
}

grNeutronTrack* grROOTEvent::GetNeutronTrack(Int_t trackID) {

    Int_t ID = -1;
    for (Int_t i = 0; i < NeutronTracks.size(); i++){
        if (this->NeutronTracks.at(i)->GetTrackID() == trackID){
            ID = i;
            break;
        }

    }
    if(ID<0)
        return NULL;
    else
  return this->NeutronTracks.at(ID);

}


grNeutronTrackVector* grROOTEvent::GetNeutronTracks() {
  return &NeutronTracks;
}

//////////////////Muon////////////////////////


void grROOTEvent::SetMuonTracks(grMuonTrackVector *trackVec){
  this->MuonTracks = *trackVec;
}



void grROOTEvent::AddMuonTrack(grMuonTrack *ntrack) {

  this->MuonTracks.push_back(ntrack);
}

grMuonTrack* grROOTEvent::GetMuonTrack(Int_t trackID) {

    Int_t ID = -1;
    for (Int_t i = 0; i < MuonTracks.size(); i++){
        if (this->MuonTracks.at(i)->GetTrackID() == trackID){
            ID = i;
            break;
        }

    }
    if(ID<0)
        return NULL;
    else
        return this->MuonTracks.at(ID);

}


grMuonTrackVector* grROOTEvent::GetMuonTracks() {
  return &MuonTracks;
}

//////////////////Electron////////////////////////


void grROOTEvent::SetElectronTracks(grElectronTrackVector *trackVec){
  this->ElectronTracks = *trackVec;
}



void grROOTEvent::AddElectronTrack(grElectronTrack *ntrack) {

  this->ElectronTracks.push_back(ntrack);
}

grElectronTrack* grROOTEvent::GetElectronTrack(Int_t trackID) {

    Int_t ID = -1;
    for (Int_t i = 0; i < ElectronTracks.size(); i++){
        if (this->ElectronTracks.at(i)->GetTrackID() == trackID){
            ID = i;
            break;
        }
    }
    if(ID<0)
        return NULL;
    else
        return this->ElectronTracks.at(ID);

}


grElectronTrackVector* grROOTEvent::GetElectronTracks() {
  return &ElectronTracks;
}

//////////////////MCP////////////////////////


void grROOTEvent::SetMCPTracks(grMCPTrackVector *trackVec){
  this->MCPTracks = *trackVec;
}



void grROOTEvent::AddMCPTrack(grMCPTrack *ntrack) {

  this->MCPTracks.push_back(ntrack);
}

grMCPTrack* grROOTEvent::GetMCPTrack(Int_t trackID) {

    Int_t ID = -1;
    for (Int_t i = 0; i < MCPTracks.size(); i++){
        if (this->MCPTracks.at(i)->GetTrackID() == trackID){
            ID = i;
            break;
        }
    }
    if(ID<0)
        return NULL;
    else
        return this->MCPTracks.at(ID);
}


grMCPTrackVector* grROOTEvent::GetMCPTracks() {
  return &MCPTracks;
}

////////////////Photon/////////////////////////

void grROOTEvent::AddPhotonTrack(grPhotonTrack *ptrack) {

  this->PhotonTracks.push_back(ptrack);

}

grPhotonTrack* grROOTEvent::GetPhotonTrack(Int_t trackID) {

    Int_t ID = -1;
    for (Int_t i = 0; i < PhotonTracks.size(); i++){
        if (this->PhotonTracks.at(i)->GetTrackID() == trackID){
            ID = i;
            break;
        }
    }
    if(ID<0)
        return NULL;
    else
        return this->PhotonTracks.at(ID);

}

void grROOTEvent::SetPhotonTracks(grPhotonTrackVector *trackVec){
  this->PhotonTracks = *trackVec;
}


grPhotonTrackVector* grROOTEvent::GetPhotonTracks() {
  return &PhotonTracks;
}

////////////////PMTHit//////////////////////////////////

void grROOTEvent::AddPMTRHit(grPMTRHit *hit) {
  this->PMTHits.push_back(hit);
}

void grROOTEvent::SetPMTRHits(grPMTRHitVector *hitVec){
  this->PMTHits = *hitVec;
}

grPMTRHitVector* grROOTEvent::GetPMTRHits() {
  return &PMTHits;
}

//////////////////ScintHit///////////////////////////////

void grROOTEvent::AddScintRHit(grScintRHit *hit) {
  this->ScintRHits.push_back(hit);
}

grScintRHitVector* grROOTEvent::GetScintRHits() {
  return &ScintRHits;
}

/*
 * mqROOTEvent.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "mqROOTEvent.hh"
#include "mqPhotonTrack.hh"
#include "mqGammaTrack.hh"
#include "mqNeutronTrack.hh"
#include "mqMuonTrack.hh"
#include "mqElectronTrack.hh"
#include "mqMCPTrack.hh"
#include "mqPMTRHit.hh"
#include "mqScintRHit.hh"
#include <algorithm>
#include <vector>

template<class SequenceType>
void DeleteAll(SequenceType& sequence) {
	for (typename SequenceType::iterator it = sequence.begin(); it != sequence.end(); ++it) {
		delete (*it);
	}
	sequence.clear();
}
ClassImp(mqROOTEvent)
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
mqROOTEvent::mqROOTEvent()
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
mqROOTEvent::~mqROOTEvent()
{
	this->Reset();
}
void mqROOTEvent::Initialize() {

}
void mqROOTEvent::Finalize() {

/*
	// Sort the track collection, so that time(track_n)<time(track_n+1), because this ordering is needed in the offline analysis
	std::sort(PhotonTracks.begin(), PhotonTracks.end(),
				mqPhotonTrack::compareHits);

	std::sort(GammaTracks.begin(), GammaTracks.end(),
				mqGammaTrack::compareHits);

	std::sort(NeutronTracks.begin(), NeutronTracks.end(),
				mqNeutronTrack::compareHits);

	std::sort(MuonTracks.begin(), MuonTracks.end(),
				mqMuonTrack::compareHits);
	
	std::sort(ElectronTracks.begin(), ElectronTracks.end(),
				mqElectronTrack::compareHits);
	
	std::sort(MCPTracks.begin(), MCPTracks.end(),
				mqMCPTrack::compareHits);
	
	// Sort the hit collection, so that time(hit_n)<time(hit_n+1), because this ordering is needed in the offline analysis
	std::sort(PMTHits.begin(), PMTHits.end(),
			mqPMTRHit::compareHits);
*/
//	//Sort the hit collection, so that time(hit_n)<time(hit_n+1), because this ordering is needed in the offline analysis
//	std::sort(ScintRHits.begin(), ScintRHits.end(),
//			mqScintRHit::compareHits);

	NbOfNeutronTracks = NeutronTracks.size();
	NbOfGammaTracks = GammaTracks.size();
	NbOfPhotonTracks = PhotonTracks.size();
	NbOfMuonTracks = MuonTracks.size();
	NbOfElectronTracks = ElectronTracks.size();
	NbOfMCPTracks = MCPTracks.size();
	NbOfPMTHits = PMTHits.size();
	NbOfScintRHits = ScintRHits.size();
}

void mqROOTEvent::Reset() {
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

void mqROOTEvent::AddGammaTrack(mqGammaTrack *mtrack) {

	this->GammaTracks.push_back(mtrack);
}

void mqROOTEvent::SetGammaTracks(mqGammaTrackVector *trackVec){
	this->GammaTracks = *trackVec;
}


mqGammaTrack* mqROOTEvent::GetGammaTrack(Int_t trackID) {

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


mqGammaTrackVector* mqROOTEvent::GetGammaTracks() {
	return &GammaTracks;
}

/////////////////////////Neutron/////////////////////////////////////////////

void mqROOTEvent::SetNeutronTracks(mqNeutronTrackVector *trackVec){
	this->NeutronTracks = *trackVec;
}



void mqROOTEvent::AddNeutronTrack(mqNeutronTrack *ntrack) {

	this->NeutronTracks.push_back(ntrack);
}

mqNeutronTrack* mqROOTEvent::GetNeutronTrack(Int_t trackID) {

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


mqNeutronTrackVector* mqROOTEvent::GetNeutronTracks() {
	return &NeutronTracks;
}

//////////////////Muon////////////////////////


void mqROOTEvent::SetMuonTracks(mqMuonTrackVector *trackVec){
	this->MuonTracks = *trackVec;
}



void mqROOTEvent::AddMuonTrack(mqMuonTrack *ntrack) {

	this->MuonTracks.push_back(ntrack);
}

mqMuonTrack* mqROOTEvent::GetMuonTrack(Int_t trackID) {

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


mqMuonTrackVector* mqROOTEvent::GetMuonTracks() {
	return &MuonTracks;
}

//////////////////Electron////////////////////////


void mqROOTEvent::SetElectronTracks(mqElectronTrackVector *trackVec){
	this->ElectronTracks = *trackVec;
}



void mqROOTEvent::AddElectronTrack(mqElectronTrack *ntrack) {

	this->ElectronTracks.push_back(ntrack);
}

mqElectronTrack* mqROOTEvent::GetElectronTrack(Int_t trackID) {

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


mqElectronTrackVector* mqROOTEvent::GetElectronTracks() {
	return &ElectronTracks;
}

//////////////////MCP////////////////////////


void mqROOTEvent::SetMCPTracks(mqMCPTrackVector *trackVec){
	this->MCPTracks = *trackVec;
}



void mqROOTEvent::AddMCPTrack(mqMCPTrack *ntrack) {

	this->MCPTracks.push_back(ntrack);
}

mqMCPTrack* mqROOTEvent::GetMCPTrack(Int_t trackID) {

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


mqMCPTrackVector* mqROOTEvent::GetMCPTracks() {
	return &MCPTracks;
}

////////////////Photon/////////////////////////

void mqROOTEvent::AddPhotonTrack(mqPhotonTrack *ptrack) {

	this->PhotonTracks.push_back(ptrack);

}

mqPhotonTrack* mqROOTEvent::GetPhotonTrack(Int_t trackID) {

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

void mqROOTEvent::SetPhotonTracks(mqPhotonTrackVector *trackVec){
	this->PhotonTracks = *trackVec;
}


mqPhotonTrackVector* mqROOTEvent::GetPhotonTracks() {
	return &PhotonTracks;
}

////////////////PMTHit//////////////////////////////////

void mqROOTEvent::AddPMTRHit(mqPMTRHit *hit) {
	this->PMTHits.push_back(hit);
}

void mqROOTEvent::SetPMTRHits(mqPMTRHitVector *hitVec){
	this->PMTHits = *hitVec;
}

mqPMTRHitVector* mqROOTEvent::GetPMTRHits() {
	return &PMTHits;
}

//////////////////ScintHit///////////////////////////////

void mqROOTEvent::AddScintRHit(mqScintRHit *hit) {
	this->ScintRHits.push_back(hit);
}

mqScintRHitVector* mqROOTEvent::GetScintRHits() {
	return &ScintRHits;
}



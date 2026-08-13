/*
 * mqUserEventInformation.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#include "mqUserEventInformation.hh"
#include "mqPhotonTrack.hh"
#include "mqGammaTrack.hh"
#include "mqNeutronTrack.hh"
#include "mqMuonTrack.hh"
#include "mqElectronTrack.hh"
#include "mqMCPTrack.hh"
#include "mqROOTEvent.hh"
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
mqUserEventInformation::mqUserEventInformation()
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
mqUserEventInformation::~mqUserEventInformation()
{
	this->Reset();
}
void mqUserEventInformation::Initialize() {

}
void mqUserEventInformation::Finalize() {

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
			mqPMTHit::compareHits);

	// Sort the hit collection, so that time(hit_n)<time(hit_n+1), because this ordering is needed in the offline analysis
//	std::sort(ScintHits.begin(), ScintHits.end(),
//			mqScintHit::compareHits);
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

void mqUserEventInformation::Reset() {

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

void mqUserEventInformation::SetEventID(G4int EventID) {
	this->eventID = EventID;
}

G4int mqUserEventInformation::GetEventID()const{
	return this->eventID;
}

void mqUserEventInformation::SetRunID(G4int RunID) {
	this->runID = RunID;
}

G4int mqUserEventInformation::GetRunID()const{
	return this->runID;
}

//////////////////////gamma//////////////////////////////////

void mqUserEventInformation::AddGammaTrack(mqGammaTrack *mtrack){
	this->GammaTracks.push_back(mtrack);
}

mqGammaTrack* mqUserEventInformation::GetGammaTrack(G4int trackID){

	G4int ID = -1;

	for (G4int i = 0; i < GammaTracks.size(); i++){
		 if (this->GammaTracks.at(i)->GetTrackID() == trackID)

			 ID = i;

	}
	return this->GammaTracks.at(ID);

}


mqGammaTrackVector* mqUserEventInformation::GetGammaTracks(){
	return &GammaTracks;
}

///////////////////////Neutron/////////////////////////////

void mqUserEventInformation::AddNeutronTrack(mqNeutronTrack *ntrack) {

	this->NeutronTracks.push_back(ntrack);
}

mqNeutronTrack* mqUserEventInformation::GetNeutronTrack(G4int trackID){

	G4int ID = -1;
	for (G4int i = 0; i < NeutronTracks.size(); i++){
		 if (this->NeutronTracks.at(i)->GetTrackID() == trackID)

			 ID = i;

	}
	return this->NeutronTracks.at(ID);

}

mqNeutronTrackVector* mqUserEventInformation::GetNeutronTracks(){
	return &NeutronTracks;
}

////////////////////////muon///////////////////////////

void mqUserEventInformation::AddMuonTrack(mqMuonTrack *ntrack) {

	this->MuonTracks.push_back(ntrack);
}

mqMuonTrack* mqUserEventInformation::GetMuonTrack(G4int trackID){

	G4int ID = -1;
	for (G4int i = 0; i < MuonTracks.size(); i++){
		 if (this->MuonTracks.at(i)->GetTrackID() == trackID)

			 ID = i;

	}
	return this->MuonTracks.at(ID);

}

mqMuonTrackVector* mqUserEventInformation::GetMuonTracks(){
	return &MuonTracks;
}


////////////////////////electron///////////////////////////

void mqUserEventInformation::AddElectronTrack(mqElectronTrack *ntrack) {

	this->ElectronTracks.push_back(ntrack);
}

mqElectronTrack* mqUserEventInformation::GetElectronTrack(G4int trackID){

	G4int ID = -1;
	for (G4int i = 0; i < ElectronTracks.size(); i++){
		 if (this->ElectronTracks.at(i)->GetTrackID() == trackID)

			 ID = i;

	}
	return this->ElectronTracks.at(ID);

}

mqElectronTrackVector* mqUserEventInformation::GetElectronTracks(){
	return &ElectronTracks;
}


////////////////////////mCP///////////////////////////

void mqUserEventInformation::AddMCPTrack(mqMCPTrack *ntrack) {

	this->MCPTracks.push_back(ntrack);
}

mqMCPTrack* mqUserEventInformation::GetMCPTrack(G4int trackID){
	G4int ID = -1;
		for (G4int i = 0; i < MCPTracks.size(); i++){
			if (this->MCPTracks.at(i)->GetTrackID() == trackID)

				ID = i;
	}
	return this->MCPTracks.at(ID);

}

mqMCPTrackVector* mqUserEventInformation::GetMCPTracks(){
	return &MCPTracks;
}


////////////////////photon////////////////////////////

void mqUserEventInformation::AddPhotonTrack(mqPhotonTrack *ptrack) {

	this->PhotonTracks.push_back(ptrack);

}

mqPhotonTrack* mqUserEventInformation::GetPhotonTrack(G4int trackID){

	G4int ID = -1;
	for (G4int i = 0; i < PhotonTracks.size(); i++){
		 if (this->PhotonTracks.at(i)->GetTrackID() == trackID)

			 ID = i;

	}
	return this->PhotonTracks.at(ID);

}


mqPhotonTrackVector* mqUserEventInformation::GetPhotonTracks(){
	return &PhotonTracks;
}

//////////////////////PMTHit////////////////////////

void mqUserEventInformation::AddPMTHit(mqPMTHit *hit) {
	this->PMTHits.push_back(hit);
}

mqPMTHitVector* mqUserEventInformation::GetPMTHits(){
	return &PMTHits;
}

//////////////////ScintHit////////////////////////////

void mqUserEventInformation::AddScintHit(mqScintHit *hit) {
	this->ScintHits.push_back(hit);
}

mqScintHitVector* mqUserEventInformation::GetScintHits(){
	return &ScintHits;
}







////////////////////Store Data in ROOT//////////////////////

mqROOTEvent* mqUserEventInformation::ConvertToROOTEvent(){

	//convert information stored in UserEventInformation into TObject

	mqROOTEvent *myROOTEvent = new mqROOTEvent();
	myROOTEvent->SetAbsorptionCount(this->GetAbsorptionCount());
	myROOTEvent->SetBoundaryAbsorption(this->GetBoundaryAbsorptionCount());
	myROOTEvent->SetEventID(this->GetEventID());
	myROOTEvent->SetGammaTracks(this->GetGammaTracks());
	myROOTEvent->SetNeutronTracks(this->GetNeutronTracks());
	myROOTEvent->SetMuonTracks(this->GetMuonTracks());
	myROOTEvent->SetElectronTracks(this->GetElectronTracks());
	myROOTEvent->SetMCPTracks(this->GetMCPTracks());
//	myROOTEvent->SetPMTHitCount(this->GetPECountPMT());
	//convert G4hits into ROOT hits:

	for (Int_t i = 0; i < this->PMTHits.size(); i++){
		 myROOTEvent->AddPMTRHit(this->PMTHits.at(i)->ConvertToROOTHit());
	//	 mqPMTRHit* hit = this->PMTHits.at(i)->ConvertToROOTHit();
	//	 myROOTEvent->AddPMTRHit(hit);
	//	 delete hit;
	}


	//convert G4hits into ROOT hits:

	for (Int_t i = 0; i < this->ScintHits.size(); i++){
		 myROOTEvent->AddScintRHit(this->ScintHits.at(i)->ConvertToROOTHit()); 
		 //mqScintRHit* scinthit = this->ScintHits.at(i)->ConvertToROOTHit();
		 //myROOTEvent->AddScintRHit(scinthit);
		 //delete scinthit;
	}
	myROOTEvent->SetPMTSAboveThreshold(this->GetPMTSAboveThreshold());
	myROOTEvent->SetPhotonCountCeren(this->GetPhotonCount_Cheren());
	myROOTEvent->SetPhotonCountScint(this->GetPhotonCount_Scint());
	myROOTEvent->SetPhotonTracks(this->GetPhotonTracks());
//	myROOTEvent->SetGammaOutScintillator(this->GetGammaOutScintillator());
	myROOTEvent->SetMuonTrigger(this->GetMuonTrigger());
//	myROOTEvent->SetScintToPMT(this->GetScintToPMT());
	myROOTEvent->SetEventEnergyDeposit(this->GetEventEnergyDeposit());
	myROOTEvent->SetBarHit(this->GetBarHit());
	myROOTEvent->SetSlabHit(this->GetSlabHit());
	myROOTEvent->SetPanelHit(this->GetPanelHit());
	myROOTEvent->SetEventWeight(this->GetEventWeight());
	//process name is empty; fill by adding process names to read in files, setting in mqEventAction
	myROOTEvent->SetProcessID(this->GetProcessID());
  return myROOTEvent;

}




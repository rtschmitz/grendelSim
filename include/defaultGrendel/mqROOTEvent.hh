/*
 * mqROOTEvent.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQROOTEVENT_HH_
#define MQROOTEVENT_HH_

// only include classes known to ROOT
#include "mqPhotonTrack.hh"
#include "mqGammaTrack.hh"
#include "mqNeutronTrack.hh"
#include "mqMuonTrack.hh"
#include "mqElectronTrack.hh"
#include "mqMCPTrack.hh"
#include "mqPMTRHit.hh"
#include "mqScintRHit.hh"



#include <TObject.h>

class mqROOTEvent :  public TObject
{
public:
  mqROOTEvent();
  //virtual
  ~mqROOTEvent();

  inline void Print()const{};

  void Initialize();
  void Reset();
  void Finalize();

  void SetEventID (Int_t eID){ eventID = eID; }
  Int_t GetEventID()const{ return eventID; }

  //virtual void Clear(Option_t* /*option*/ = "");

  //info about Cherenkov photons and PMT hits
  void SetPhotonCountCeren(Int_t photons_Ceren){ this->nbOfCerenkovPhotons = photons_Ceren; }
  void SetPhotonCountScint(Int_t photons_Scint){ this->nbOfScintillationPhotons = photons_Scint; }
  void SetAbsorptionCount(Int_t absCount){ this->absorptionCount = absCount; }
  void SetBoundaryAbsorption(Int_t boundaryAbs){ this->boundaryAbsorptionCount = boundaryAbs; }
//  void SetPMTHitCount(Int_t hits){ this->totalNbOfPEPMT = hits; }
  void SetPMTSAboveThreshold(Int_t pmts){this->pmtsAboveTrigger = pmts; }

  Int_t GetPhotonCountCeren()const { return nbOfCerenkovPhotons; }
  Int_t GetPhotonCountScint()const { return nbOfScintillationPhotons; }
//  Int_t GetPMTHitCount()const { return totalNbOfPEPMT; }
  Int_t GetAbsorptionCount()const { return absorptionCount; }
  Int_t GetBoundaryAbsorptionCount() const { return boundaryAbsorptionCount; }
  Int_t GetPMTSAboveThreshold()const {return pmtsAboveTrigger;}


  // info about crystal hits
//  void SetTotalEDepInCrystals(Double_t edep){this->totalEDepInCrytals = edep;}
//  void SetTotalNREDepInCrystals(Double_t edepNR){this->totalNREDepInCrytals = edepNR;}


//  Double_t GetTotalEDepInCrystals()const {return totalEDepInCrytals;}
//  Double_t GetTotalNREDepInCrystals()const {return totalNREDepInCrytals;}

  //gamma tracking
  void AddGammaTrack (mqGammaTrack *track);
  void SetGammaTracks(mqGammaTrackVector *trackVec);
  mqGammaTrack* GetGammaTrack(Int_t trackID);
  mqGammaTrackVector* GetGammaTracks();

  //neutron tracking
  void AddNeutronTrack (mqNeutronTrack *track);
  void SetNeutronTracks(mqNeutronTrackVector *trackVec);
  mqNeutronTrack* GetNeutronTrack(Int_t trackID);
  mqNeutronTrackVector* GetNeutronTracks();

  //muon tracking 
  void AddMuonTrack (mqMuonTrack *track);
  void SetMuonTracks(mqMuonTrackVector *trackVec);
  mqMuonTrack* GetMuonTrack(Int_t trackID);
  mqMuonTrackVector* GetMuonTracks();

  //electron tracking 
  void AddElectronTrack (mqElectronTrack *track);
  void SetElectronTracks(mqElectronTrackVector *trackVec);
  mqElectronTrack* GetElectronTrack(Int_t trackID);
  mqElectronTrackVector* GetElectronTracks();

  //mCP tracking 
  void AddMCPTrack (mqMCPTrack *track);
  void SetMCPTracks(mqMCPTrackVector *trackVec);
  mqMCPTrack* GetMCPTrack(Int_t trackID);
  mqMCPTrackVector* GetMCPTracks();
  
//  void SetGammaOutScintillator (bool in) {gammaOutScintillator = in;}
//  bool GetGammaOutScintillator() const{return gammaOutScintillator;}

  void SetMuonTrigger (bool in) {muonTrig = in;}
  bool GetMuonTrigger() const{return muonTrig;}

  void SetTrident (bool in) {trident = in;}
  bool GetTrident() const{return trident;}
  
  void SetTridentGamma (bool in) {tridentGamma = in;}
  bool GetTridentGamma() const{return tridentGamma;}
  
  void SetKaonCavern(bool in) {kaonCavern = in;}
  bool GetKaonCavern() const{return kaonCavern;}
//	void SetScintToPMT(bool in) {scintToPMT = in;}
//	bool GetScintToPMT()const{return scintToPMT;}


  void SetBarHit (Int_t hit){ this->barHit = hit;}
  Int_t GetBarHit()const{ return barHit;}

  void SetSlabHit (Int_t hit){ this->slabHit = hit;}
  Int_t GetSlabHit()const{ return slabHit;}
  
  void SetPanelHit (Int_t hit){ this->panelHit = hit;}
  Int_t GetPanelHit()const{ return panelHit;}
  
  void SetEventEnergyDeposit(Double_t edep){ this->Edep_MeV = edep;}
  Double_t GetEventEnergyDeposit()const{ return Edep_MeV;}

  void SetEventWeight(Double_t ew){eventWeight = ew;}
  Double_t GetEventWeight()const{ return eventWeight;}

  void SetProcessID(Int_t pn){processID = pn;}
  Int_t GetProcessID()const{ return processID;}

  //photon tracking
  void AddPhotonTrack (mqPhotonTrack *track);
  void SetPhotonTracks(mqPhotonTrackVector *trackVec);
  mqPhotonTrack* GetPhotonTrack(Int_t trackID);
  mqPhotonTrackVector* GetPhotonTracks();

  //PMT Hits
  void AddPMTRHit(mqPMTRHit *hit);
  void SetPMTRHits(mqPMTRHitVector *hitVec);
  mqPMTRHitVector* GetPMTRHits();

  //Scint Hits
  void AddScintRHit(mqScintRHit *hit);
  mqScintRHitVector* GetScintRHits();

  friend std::ostream & operator<<(std::ostream &o, mqROOTEvent const &event);

  //std::vector<unsigned long> Seeds;//Seeds of the random number generator for the current event
private:


  Int_t eventID;

  mqGammaTrackVector GammaTracks;
  Int_t NbOfGammaTracks;

  mqNeutronTrackVector NeutronTracks;
  Int_t NbOfNeutronTracks;

  mqPhotonTrackVector PhotonTracks;
  Int_t NbOfPhotonTracks;

  mqMuonTrackVector MuonTracks;
  Int_t NbOfMuonTracks;

  mqElectronTrackVector ElectronTracks;
  Int_t NbOfElectronTracks;

  mqMCPTrackVector MCPTracks;
  Int_t NbOfMCPTracks;

  mqPMTRHitVector PMTHits;
  Int_t NbOfPMTHits;

  mqScintRHitVector ScintRHits;
  Int_t NbOfScintRHits;

  Int_t nbOfCerenkovPhotons;
  Int_t nbOfScintillationPhotons;
  Int_t absorptionCount;
  Int_t boundaryAbsorptionCount;

//  Int_t totalNbOfPEPMT;
  Int_t pmtsAboveTrigger;

//  Double_t totalEDepInCrytals;
//  Double_t totalNREDepInCrytals;

  Double_t Edep_MeV;
  Int_t barHit;
  Int_t slabHit;
  Int_t panelHit;

//  bool gammaOutScintillator;
  bool muonTrig;
  bool trident;
  bool tridentGamma;
  bool kaonCavern;
  Double_t eventWeight;
  Int_t processID;

//	bool scintToPMT;
  ClassDef(mqROOTEvent,12);

};


#endif /* MQROOTEVENT_HH_ */

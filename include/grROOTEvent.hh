/*
 * grROOTEvent.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQROOTEVENT_HH_
#define MQROOTEVENT_HH_

// only include classes known to ROOT
#include "grPhotonTrack.hh"
#include "grGammaTrack.hh"
#include "grNeutronTrack.hh"
#include "grMuonTrack.hh"
#include "grElectronTrack.hh"
#include "grMCPTrack.hh"
#include "grPMTRHit.hh"
#include "grScintRHit.hh"



#include <TObject.h>

class grROOTEvent :  public TObject
{
public:
  grROOTEvent();
  //virtual
  ~grROOTEvent();

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
  void AddGammaTrack (grGammaTrack *track);
  void SetGammaTracks(grGammaTrackVector *trackVec);
  grGammaTrack* GetGammaTrack(Int_t trackID);
  grGammaTrackVector* GetGammaTracks();

  //neutron tracking
  void AddNeutronTrack (grNeutronTrack *track);
  void SetNeutronTracks(grNeutronTrackVector *trackVec);
  grNeutronTrack* GetNeutronTrack(Int_t trackID);
  grNeutronTrackVector* GetNeutronTracks();

  //muon tracking
  void AddMuonTrack (grMuonTrack *track);
  void SetMuonTracks(grMuonTrackVector *trackVec);
  grMuonTrack* GetMuonTrack(Int_t trackID);
  grMuonTrackVector* GetMuonTracks();

  //electron tracking
  void AddElectronTrack (grElectronTrack *track);
  void SetElectronTracks(grElectronTrackVector *trackVec);
  grElectronTrack* GetElectronTrack(Int_t trackID);
  grElectronTrackVector* GetElectronTracks();

  //mCP tracking
  void AddMCPTrack (grMCPTrack *track);
  void SetMCPTracks(grMCPTrackVector *trackVec);
  grMCPTrack* GetMCPTrack(Int_t trackID);
  grMCPTrackVector* GetMCPTracks();

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
//  void SetScintToPMT(bool in) {scintToPMT = in;}
//  bool GetScintToPMT()const{return scintToPMT;}


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
  void AddPhotonTrack (grPhotonTrack *track);
  void SetPhotonTracks(grPhotonTrackVector *trackVec);
  grPhotonTrack* GetPhotonTrack(Int_t trackID);
  grPhotonTrackVector* GetPhotonTracks();

  //PMT Hits
  void AddPMTRHit(grPMTRHit *hit);
  void SetPMTRHits(grPMTRHitVector *hitVec);
  grPMTRHitVector* GetPMTRHits();

  //Scint Hits
  void AddScintRHit(grScintRHit *hit);
  grScintRHitVector* GetScintRHits();

  friend std::ostream & operator<<(std::ostream &o, grROOTEvent const &event);

  //std::vector<unsigned long> Seeds;//Seeds of the random number generator for the current event
private:


  Int_t eventID;

  grGammaTrackVector GammaTracks;
  Int_t NbOfGammaTracks;

  grNeutronTrackVector NeutronTracks;
  Int_t NbOfNeutronTracks;

  grPhotonTrackVector PhotonTracks;
  Int_t NbOfPhotonTracks;

  grMuonTrackVector MuonTracks;
  Int_t NbOfMuonTracks;

  grElectronTrackVector ElectronTracks;
  Int_t NbOfElectronTracks;

  grMCPTrackVector MCPTracks;
  Int_t NbOfMCPTracks;

  grPMTRHitVector PMTHits;
  Int_t NbOfPMTHits;

  grScintRHitVector ScintRHits;
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

//  bool scintToPMT;
  ClassDef(grROOTEvent,12);

};


#endif /* MQROOTEVENT_HH_ */

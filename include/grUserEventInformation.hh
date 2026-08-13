/*
 * grUserEventInformation.hh
 *
 *  Created on: 18.09.2012
 *      Author: schmitz
 */

#ifndef MQUSEREVENTINFORMATION_HH_
#define MQUSEREVENTINFORMATION_HH_

#include "G4VUserEventInformation.hh"
#include "grPhotonTrack.hh"
#include "grGammaTrack.hh"
#include "grNeutronTrack.hh"
#include "grMuonTrack.hh"
#include "grElectronTrack.hh"
#include "grMCPTrack.hh"
#include "grPMTHit.hh"
#include "grScintHit.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "grROOTEvent.hh"

#include <TObject.h>
const int numDetectors = 31;

class grUserEventInformation : public G4VUserEventInformation
{
public:


  grUserEventInformation();
  //virtual
  ~grUserEventInformation();

  inline void Print()const{};

  void Initialize();
  void Reset();
  void Finalize();

  void SetEventID (G4int eventID);
  G4int GetEventID()const;
  void SetRunID (G4int runID);
  G4int GetRunID()const;


  //info about Cherenkov photons and PMT hits
  void IncPhotonCount_Scint(){photonCount_Scint++;}
  void IncPhotonCount(){photonCount++;}
  void IncPhotonCount_Cheren(){photonCount_Cheren++;}
  void IncAbsorption(){absorptionCount++;}
  void IncBoundaryAbsorption(){boundaryAbsorptionCount++;}
  void IncPECountPMT(G4int i=1){peCountPMT+=i;}

  G4int GetPECountPMT()const {return peCountPMT;}
  G4int GetAbsorptionCount()const {return absorptionCount;}
  G4int GetBoundaryAbsorptionCount() const {return boundaryAbsorptionCount;}

  //Gets the total photon count produced

  G4int GetPhotonCount_Scint()const{return photonCount_Scint;}
  G4int GetPhotonCount()const{return photonCount;}
  G4int GetPhotonCount_Cheren()const{return photonCount_Cheren;}
  void IncPMTSAboveThreshold(){pmtsAboveThreshold++;}
  G4int GetPMTSAboveThreshold()const{return pmtsAboveThreshold;}

  //info about primary gamma TODO store in gamma track




  // info about crystals
  void IncCrystalEDep(G4double dep){totalEDepInCrystals+=dep;}
  void IncCrystalENRDep(G4double NRdep){totalNREDepInCrystals+=NRdep;}


  G4double GetCrystalEDep()const {return totalEDepInCrystals;}
  G4double GetCrystalENRDep()const {return totalNREDepInCrystals;}

  void SetGammaOutScintillator (bool in){gammaOutScintillator=in;}
  bool GetGammaOutScintillator()const{return gammaOutScintillator;}

  void SetMuonTrigger (bool in){muonTrig=in;}
  bool GetMuonTrigger()const{return muonTrig;}

  void SetTrident (bool in) {trident = in;}
  bool GetTrident() const{return trident;}

  void SetTridentGamma (bool in) {tridentGamma = in;}
  bool GetTridentGamma() const{return tridentGamma;}

  void SetKaonCavern(bool in) {kaonCavern = in;}
  bool GetKaonCavern() const{return kaonCavern;}


  void SetScintToPMT (G4bool in){scintToPMT = in;}
  G4bool GetScintToPMT()const{return scintToPMT;}



  void SetBarHit(G4int hit){barHit = hit;}
  G4int GetBarHit()const {return barHit;}

  void SetSlabHit(G4int hit){slabHit = hit;}
  G4int GetSlabHit()const {return slabHit;}

  void SetPanelHit(G4int hit){panelHit = hit;}
  G4int GetPanelHit()const {return panelHit;}

  void SetEventEnergyDeposit(G4double out){Edep_MeV = out;}
  G4double GetEventEnergyDeposit()const {return Edep_MeV;}

  void SetEventWeight(G4double ew){eventWeight = ew;}
  G4double GetEventWeight()const{ return eventWeight;}

  void SetProcessID(G4int pn){processID = pn;}
  G4int GetProcessID()const{ return processID;}

  /*It's necessary to reset the lastTrackID for tracking, because for example there is mostly only 1 gamma (the primary id=1) per event
   * In the second event per run, the primary has again id=1, but tmLastTrackID is already 1 from the previous events
   * -> no new GammaTrack is created and the following step method try to access this non existing object -> core dump
   */
  void SetPhotonLastTrackID(G4int tpLastTrackID){ photonLastTrackID = tpLastTrackID ;}
  G4int GetPhotonLastTrackID()const{return photonLastTrackID;}

  void SetGammaLastTrackID(G4int tmLastTrackID){gammaLastTrackID = tmLastTrackID ;}
  G4int GetGammaLastTrackID()const{return gammaLastTrackID;}

  void SetNeutronLastTrackID(G4int tnLastTrackID){neutronLastTrackID = tnLastTrackID ;}
  G4int GetNeutronLastTrackID()const{return neutronLastTrackID;}

  void SetMuonLastTrackID(G4int tnLastTrackID){muonLastTrackID = tnLastTrackID ;}
  G4int GetMuonLastTrackID()const{return muonLastTrackID;}

  void SetElectronLastTrackID(G4int tnLastTrackID){electronLastTrackID = tnLastTrackID ;}
  G4int GetElectronLastTrackID()const{return electronLastTrackID;}

  void SetMCPLastTrackID(G4int tnLastTrackID){mcpLastTrackID = tnLastTrackID ;}
  G4int GetMCPLastTrackID()const{return mcpLastTrackID;}

  //gamma tracking
  void AddGammaTrack (grGammaTrack *track);
  grGammaTrack* GetGammaTrack (G4int trackID);
  grGammaTrackVector* GetGammaTracks();


  //neutron tracking
  void AddNeutronTrack (grNeutronTrack *track);
  grNeutronTrack* GetNeutronTrack(G4int trackID);
  grNeutronTrackVector* GetNeutronTracks();

  //muon tracking
  void AddMuonTrack (grMuonTrack *track);
  grMuonTrack* GetMuonTrack(G4int trackID);
  grMuonTrackVector* GetMuonTracks();

  //electron tracking
  void AddElectronTrack (grElectronTrack *track);
  grElectronTrack* GetElectronTrack(G4int trackID);
  grElectronTrackVector* GetElectronTracks();

  //muon tracking
  void AddMCPTrack (grMCPTrack *track);
  grMCPTrack* GetMCPTrack(G4int trackID);
  grMCPTrackVector* GetMCPTracks();

  //photon tracking
  void AddPhotonTrack (grPhotonTrack *track);
  grPhotonTrack* GetPhotonTrack(G4int trackID);
  grPhotonTrackVector* GetPhotonTracks();

  //PMT Hits
  void AddPMTHit(grPMTHit *hit);
  grPMTHitVector* GetPMTHits();

  //Scint Hits
  void AddScintHit(grScintHit *hit);
  grScintHitVector* GetScintHits();

  friend std::ostream & operator<<(std::ostream &o, grUserEventInformation const &event);

  grROOTEvent* ConvertToROOTEvent();

  //std::vector<unsigned long> Seeds;//Seeds of the random number generator for the current event
private:
  grGammaTrackVector GammaTracks;
  G4int NbOfGammaTracks;

  grNeutronTrackVector NeutronTracks;
  G4int NbOfNeutronTracks;

  grPhotonTrackVector PhotonTracks;
  G4int NbOfPhotonTracks;

  grMuonTrackVector MuonTracks;
  G4int NbOfMuonTracks;

  grElectronTrackVector ElectronTracks;
  G4int NbOfElectronTracks;

  grMCPTrackVector MCPTracks;
  G4int NbOfMCPTracks;

  grPMTHitVector PMTHits;
  G4int NbOfPMTHits;

  grScintHitVector ScintHits;
  G4int NbOfScintHits;

  G4int barHit;
  G4int slabHit;
  G4int panelHit;
  G4int peCountPMT;
  G4int photonCount_Scint;
  G4int photonCount_Cheren;
  G4int photonCount;
  G4int absorptionCount;
  G4int boundaryAbsorptionCount;

  G4double totalEDepInCrystals;
  G4double totalNREDepInCrystals;

  G4int pmtsAboveThreshold;
  G4double Edep_MeV;
  G4int runID;
  G4int eventID;
  G4int photonLastTrackID;
  G4int gammaLastTrackID;
  G4int neutronLastTrackID;
  G4int muonLastTrackID;
  G4int electronLastTrackID;
  G4int mcpLastTrackID;
  bool gammaOutScintillator;
  bool muonTrig;
  bool trident;
  bool tridentGamma;
  bool kaonCavern;
  G4bool scintToPMT;
  G4double eventWeight;
  G4int processID;
};


#endif /* MQUSEREVENTINFORMATION_HH_ */

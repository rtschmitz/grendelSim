#ifndef GRUSEREVENTINFORMATION_HH_
#define GRUSEREVENTINFORMATION_HH_

#include "G4VUserEventInformation.hh"
#include "globals.hh"
#include "grElectronTrack.hh"
#include "grGammaTrack.hh"
#include "grKaonTrack.hh"
#include "grMuonTrack.hh"
#include "grNeutronTrack.hh"
#include "grScintHit.hh"

class grUserEventInformation : public G4VUserEventInformation {
public:
  grUserEventInformation();
  ~grUserEventInformation();
  void Print() const {}
  void Initialize() {}
  void Finalize() {}
  void Reset();

  void SetEventID(G4int value) { eventID = value; }
  G4int GetEventID() const { return eventID; }
  void SetRunID(G4int value) { runID = value; }
  G4int GetRunID() const { return runID; }
  void SetEventWeight(G4double value) { eventWeight = value; }
  G4double GetEventWeight() const { return eventWeight; }
  void SetProcessID(G4int value) { processID = value; }
  G4int GetProcessID() const { return processID; }

  void SetKaonCavern(bool value) { kaonCavern = value; }
  bool GetKaonCavern() const { return kaonCavern; }

  void SetGammaLastTrackID(G4int value) { gammaLastTrackID = value; }
  G4int GetGammaLastTrackID() const { return gammaLastTrackID; }
  void SetNeutronLastTrackID(G4int value) { neutronLastTrackID = value; }
  G4int GetNeutronLastTrackID() const { return neutronLastTrackID; }
  void SetMuonLastTrackID(G4int value) { muonLastTrackID = value; }
  G4int GetMuonLastTrackID() const { return muonLastTrackID; }
  void SetElectronLastTrackID(G4int value) { electronLastTrackID = value; }
  G4int GetElectronLastTrackID() const { return electronLastTrackID; }
  void SetKaonLastTrackID(G4int value) { kaonLastTrackID = value; }
  G4int GetKaonLastTrackID() const { return kaonLastTrackID; }

  void AddGammaTrack(grGammaTrack* track) { GammaTracks.push_back(track); }
  grGammaTrack* GetGammaTrack(G4int trackID);
  grGammaTrackVector* GetGammaTracks() { return &GammaTracks; }
  void AddNeutronTrack(grNeutronTrack* track) { NeutronTracks.push_back(track); }
  grNeutronTrack* GetNeutronTrack(G4int trackID);
  grNeutronTrackVector* GetNeutronTracks() { return &NeutronTracks; }
  void AddMuonTrack(grMuonTrack* track) { MuonTracks.push_back(track); }
  grMuonTrack* GetMuonTrack(G4int trackID);
  grMuonTrackVector* GetMuonTracks() { return &MuonTracks; }
  void AddElectronTrack(grElectronTrack* track) { ElectronTracks.push_back(track); }
  grElectronTrack* GetElectronTrack(G4int trackID);
  grElectronTrackVector* GetElectronTracks() { return &ElectronTracks; }
  void AddKaonTrack(grKaonTrack* track) { KaonTracks.push_back(track); }
  grKaonTrack* GetKaonTrack(G4int trackID);
  grKaonTrackVector* GetKaonTracks() { return &KaonTracks; }
  void AddScintHit(grScintHit* hit) { ScintHits.push_back(hit); }
  grScintHitVector* GetScintHits() { return &ScintHits; }

private:
  grGammaTrackVector GammaTracks;
  grNeutronTrackVector NeutronTracks;
  grMuonTrackVector MuonTracks;
  grElectronTrackVector ElectronTracks;
  grKaonTrackVector KaonTracks;
  grScintHitVector ScintHits;

  G4int runID;
  G4int eventID;
  G4int processID;
  G4double eventWeight;
  bool kaonCavern;
  G4int gammaLastTrackID;
  G4int neutronLastTrackID;
  G4int muonLastTrackID;
  G4int electronLastTrackID;
  G4int kaonLastTrackID;
};

#endif

#include "grHistoManager.hh"
#include "grUserEventInformation.hh"

#include "Compression.h"
#include "G4SystemOfUnits.hh"
#include "TFile.h"
#include "TTree.h"

#include <limits>
#include <string>

struct grEventOutput {
  Int_t schemaVersion, runID, eventID, processID, trackCount, hitCount;
  Float_t eventWeight;
  Bool_t kaonCavern;

  void branch(TTree* tree) {
    tree->Branch("schemaVersion", &schemaVersion);
    tree->Branch("runID", &runID);
    tree->Branch("eventID", &eventID);
    tree->Branch("processID", &processID);
    tree->Branch("eventWeight", &eventWeight);
    tree->Branch("kaonCavern", &kaonCavern);
    tree->Branch("trackCount", &trackCount);
    tree->Branch("hitCount", &hitCount);
  }
};

struct grTrackOutput {
  Int_t runID, eventID, trackID, pdgID, parentID, initialCopyNo, finalCopyNo;
  Float_t initialTime_s, finalTime_s, initialEnergy_MeV, finalEnergy_MeV;
  Float_t initialX_m, initialY_m, initialZ_m;
  Float_t finalX_m, finalY_m, finalZ_m, trackLength_m;
  std::string initialProcess, finalProcess, initialVolume, finalVolume;

  void branch(TTree* tree) {
#define B(N) tree->Branch(#N, &N)
    B(runID); B(eventID); B(trackID); B(pdgID); B(parentID);
    B(initialCopyNo); B(finalCopyNo); B(initialTime_s); B(finalTime_s);
    B(initialEnergy_MeV); B(finalEnergy_MeV);
    B(initialX_m); B(initialY_m); B(initialZ_m);
    B(finalX_m); B(finalY_m); B(finalZ_m); B(trackLength_m);
    B(initialProcess); B(finalProcess); B(initialVolume); B(finalVolume);
#undef B
  }
};

struct grHitOutput {
  Int_t runID, eventID, trackID, parentID, copyNo, pdgID;
  Float_t kineticEnergy_MeV, time_ns, x_m, y_m, z_m;
  Float_t directionX, directionY, directionZ;
  std::string creatorProcess, originVolume;

  void branch(TTree* tree) {
#define B(N) tree->Branch(#N, &N)
    B(runID); B(eventID); B(trackID); B(parentID); B(copyNo); B(pdgID);
    B(kineticEnergy_MeV); B(time_ns); B(x_m); B(y_m); B(z_m);
    B(directionX); B(directionY); B(directionZ);
    B(creatorProcess); B(originVolume);
#undef B
  }
};

namespace {
std::string text(const TString& value) { return value.Data(); }

template<class Track>
void fillTrack(grTrackOutput& o, const Track* t, Int_t runID, Int_t eventID,
               bool timeStoredInNanoseconds = false) {
  o.runID = runID; o.eventID = eventID;
  o.trackID = t->GetTrackID(); o.pdgID = t->GetPDGID(); o.parentID = t->GetParentID();
  o.initialCopyNo = t->GetFirstCopyNo(); o.finalCopyNo = t->GetLastCopyNo();
  const double timeScale = timeStoredInNanoseconds ? ns/s : 1.;
  o.initialTime_s = static_cast<Float_t>(t->GetTimeOfFirstProcess() * timeScale);
  o.finalTime_s = static_cast<Float_t>(t->GetTimeOfLastProcess() * timeScale);
  o.initialEnergy_MeV = static_cast<Float_t>(t->GetInitialEnergy());
  o.finalEnergy_MeV = static_cast<Float_t>(t->GetFinalEnergy());
  o.initialX_m = static_cast<Float_t>(t->GetFirstPositionX());
  o.initialY_m = static_cast<Float_t>(t->GetFirstPositionY());
  o.initialZ_m = static_cast<Float_t>(t->GetFirstPositionZ());
  o.finalX_m = static_cast<Float_t>(t->GetLastPositionX());
  o.finalY_m = static_cast<Float_t>(t->GetLastPositionY());
  o.finalZ_m = static_cast<Float_t>(t->GetLastPositionZ());
  o.trackLength_m = static_cast<Float_t>(t->GetTotalTrackLength());
  o.initialProcess = text(t->GetFirstProcessName()); o.finalProcess = text(t->GetLastProcessName());
  o.initialVolume = text(t->GetFirstVolume()); o.finalVolume = text(t->GetLastVolume());
}

void fillTrack(grTrackOutput& o, const grElectronTrack* t, Int_t runID, Int_t eventID) {
  o.runID = runID; o.eventID = eventID;
  o.trackID = t->GetTrackID(); o.pdgID = t->GetPDGID(); o.parentID = t->GetParentID();
  o.initialCopyNo = t->GetFirstCopyNo(); o.finalCopyNo = t->GetLastCopyNo();
  o.initialTime_s = static_cast<Float_t>(t->GetTimeOfFirstProcess());
  o.finalTime_s = static_cast<Float_t>(t->GetTimeOfLastProcess());
  o.initialEnergy_MeV = static_cast<Float_t>(t->GetInitialEnergy());
  o.finalEnergy_MeV = static_cast<Float_t>(t->GetFinalEnergy());
  o.initialX_m = static_cast<Float_t>(t->GetFirstPositionX());
  o.initialY_m = static_cast<Float_t>(t->GetFirstPositionY());
  o.initialZ_m = static_cast<Float_t>(t->GetFirstPositionZ());
  o.finalX_m = o.finalY_m = o.finalZ_m = std::numeric_limits<Float_t>::quiet_NaN();
  o.trackLength_m = static_cast<Float_t>(t->GetTotalTrackLength());
  o.initialProcess = text(t->GetFirstProcessName()); o.finalProcess = text(t->GetLastProcessName());
  o.initialVolume = text(t->GetFirstVolume()); o.finalVolume = text(t->GetLastVolume());
}

template<class Sequence>
void writeTracks(TTree* tree, grTrackOutput& output, const Sequence& tracks,
                 Int_t runID, Int_t eventID, bool timesInNanoseconds = false) {
  for (typename Sequence::const_iterator it = tracks.begin(); it != tracks.end(); ++it) {
    fillTrack(output, *it, runID, eventID, timesInNanoseconds);
    tree->Fill();
  }
}

void writeElectronTracks(TTree* tree, grTrackOutput& output,
                         const grElectronTrackVector& tracks, Int_t runID, Int_t eventID) {
  for (grElectronTrackVector::const_iterator it = tracks.begin(); it != tracks.end(); ++it) {
    fillTrack(output, *it, runID, eventID);
    tree->Fill();
  }
}
}

grHistoManager::grHistoManager()
  : rootFile(0), eventTree(0), trackTree(0), hitTree(0),
    eventOutput(new grEventOutput()), trackOutput(new grTrackOutput()), hitOutput(new grHitOutput()) {}

grHistoManager::~grHistoManager() {
  delete rootFile;
  delete eventOutput;
  delete trackOutput;
  delete hitOutput;
}

void grHistoManager::book(G4String directory) {
  const G4String fileName = directory + "grendelSim.root";
  G4cout << "GRENDEL> Create compact standalone ROOT file: " << fileName << G4endl;
  rootFile = TFile::Open(fileName, "RECREATE", "GRENDEL standalone output",
                         ROOT::CompressionSettings(ROOT::kLZ4, 4));
  if (!rootFile || rootFile->IsZombie())
    G4Exception("grHistoManager::book", "OutputOpenFailed", FatalException, fileName);

  eventTree = new TTree("Events", "One row per simulated event");
  trackTree = new TTree("Tracks", "One row per explicitly selected particle track");
  hitTree = new TTree("Hits", "One row per sensitive-volume entry");
  eventTree->SetAutoFlush(-16LL * 1024LL * 1024LL);
  trackTree->SetAutoFlush(-16LL * 1024LL * 1024LL);
  hitTree->SetAutoFlush(-32LL * 1024LL * 1024LL);
  eventOutput->branch(eventTree);
  trackOutput->branch(trackTree);
  hitOutput->branch(hitTree);
}

void grHistoManager::save() {
  if (!rootFile) return;
  rootFile->cd();
  eventTree->Write(); trackTree->Write(); hitTree->Write();
  rootFile->Close();
  G4cout << "\n----> Compact standalone ROOT trees are saved\n" << G4endl;
}

void grHistoManager::FillEventNtuple(grUserEventInformation& e) {
  const Int_t runID = e.GetRunID(), eventID = e.GetEventID();
  const grMuonTrackVector& muons = *e.GetMuonTracks();
  const grGammaTrackVector& gammas = *e.GetGammaTracks();
  const grNeutronTrackVector& neutrons = *e.GetNeutronTracks();
  const grElectronTrackVector& electrons = *e.GetElectronTracks();
  const grMCPTrackVector& kaons = *e.GetMCPTracks();
  const grScintHitVector& hits = *e.GetScintHits();

  eventOutput->schemaVersion = 3;
  eventOutput->runID = runID; eventOutput->eventID = eventID;
  eventOutput->processID = e.GetProcessID();
  eventOutput->eventWeight = static_cast<Float_t>(e.GetEventWeight());
  eventOutput->kaonCavern = e.GetKaonCavern();
  eventOutput->trackCount = muons.size() + gammas.size() + neutrons.size() + electrons.size() + kaons.size();
  eventOutput->hitCount = hits.size();
  eventTree->Fill();

  writeTracks(trackTree, *trackOutput, muons, runID, eventID);
  writeTracks(trackTree, *trackOutput, gammas, runID, eventID, true);
  writeTracks(trackTree, *trackOutput, neutrons, runID, eventID);
  writeElectronTracks(trackTree, *trackOutput, electrons, runID, eventID);
  writeTracks(trackTree, *trackOutput, kaons, runID, eventID);

  for (grScintHitVector::const_iterator it = hits.begin(); it != hits.end(); ++it) {
    const grScintHit* h = *it;
    const G4ThreeVector position = h->GetHitPosition();
    const G4ThreeVector direction = h->GetDirection();
    hitOutput->runID = runID; hitOutput->eventID = eventID;
    hitOutput->trackID = h->GetTrackID(); hitOutput->parentID = h->GetParentID();
    hitOutput->copyNo = h->GetCopyNo(); hitOutput->pdgID = h->GetParticleName();
    hitOutput->kineticEnergy_MeV = static_cast<Float_t>(h->GetKineticEnergy()/MeV);
    hitOutput->time_ns = static_cast<Float_t>(h->GetHitTime()/ns);
    hitOutput->x_m = static_cast<Float_t>(position.x()/m);
    hitOutput->y_m = static_cast<Float_t>(position.y()/m);
    hitOutput->z_m = static_cast<Float_t>(position.z()/m);
    hitOutput->directionX = static_cast<Float_t>(direction.x());
    hitOutput->directionY = static_cast<Float_t>(direction.y());
    hitOutput->directionZ = static_cast<Float_t>(direction.z());
    hitOutput->creatorProcess = h->GetProcName();
    hitOutput->originVolume = h->GetCreatorVolName();
    hitTree->Fill();
  }
}

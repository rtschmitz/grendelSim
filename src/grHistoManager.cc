#include "grHistoManager.hh"
#include "grUserEventInformation.hh"

#include "Compression.h"
#include "G4SystemOfUnits.hh"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include <vector>

#define TRACK_FIELDS(X, P) \
  X(P, trackID, Int_t) X(P, pdgID, Int_t) X(P, parentID, Int_t) X(P, initialCopyNo, Int_t) X(P, finalCopyNo, Int_t) \
  X(P, initialTime_s, Double_t) X(P, finalTime_s, Double_t) X(P, initialEnergy_MeV, Double_t) X(P, finalEnergy_MeV, Double_t) \
  X(P, initialX_m, Double_t) X(P, initialY_m, Double_t) X(P, initialZ_m, Double_t) \
  X(P, finalX_m, Double_t) X(P, finalY_m, Double_t) X(P, finalZ_m, Double_t) X(P, trackLength_m, Double_t) \
  X(P, initialProcess, std::string) X(P, finalProcess, std::string) X(P, initialVolume, std::string) X(P, finalVolume, std::string)
#define ELECTRON_FIELDS(X, P) \
  X(P, trackID, Int_t) X(P, pdgID, Int_t) X(P, parentID, Int_t) X(P, initialCopyNo, Int_t) X(P, finalCopyNo, Int_t) \
  X(P, initialTime_s, Double_t) X(P, finalTime_s, Double_t) X(P, initialEnergy_MeV, Double_t) X(P, finalEnergy_MeV, Double_t) \
  X(P, initialX_m, Double_t) X(P, initialY_m, Double_t) X(P, initialZ_m, Double_t) X(P, trackLength_m, Double_t) \
  X(P, initialProcess, std::string) X(P, finalProcess, std::string) X(P, initialVolume, std::string) X(P, finalVolume, std::string)
#define DECLARE_VECTOR(P, N, T) std::vector<T> P##_##N;
#define CLEAR_VECTOR(P, N, T) P##_##N.clear();
#define BRANCH_VECTOR(P, N, T) tree->Branch(#P "_" #N, &P##_##N);

struct grOutputBuffer {
  Int_t schemaVersion, runID, eventID, processID;
  Double_t eventWeight;
  Bool_t kaonCavern;

  TRACK_FIELDS(DECLARE_VECTOR, muon)
  TRACK_FIELDS(DECLARE_VECTOR, gamma)
  TRACK_FIELDS(DECLARE_VECTOR, neutron)
  ELECTRON_FIELDS(DECLARE_VECTOR, electron)
  TRACK_FIELDS(DECLARE_VECTOR, kaon)

  std::vector<Int_t> scint_trackID, scint_parentID, scint_copyNo, scint_pdgID;
  std::vector<Double_t> scint_energyDeposit_MeV, scint_entryTime_ns, scint_exitTime_ns;
  std::vector<Double_t> scint_entryX_m, scint_entryY_m, scint_entryZ_m;
  std::vector<Double_t> scint_exitX_m, scint_exitY_m, scint_exitZ_m;
  std::vector<std::string> scint_entryProcess, scint_originVolume;

  void branch(TTree* tree);
  void clear();
};

#define BRANCH(N) tree->Branch(#N, &N)
void grOutputBuffer::branch(TTree* tree) {
  BRANCH(schemaVersion); BRANCH(runID); BRANCH(eventID); BRANCH(processID);
  BRANCH(eventWeight); BRANCH(kaonCavern);
  TRACK_FIELDS(BRANCH_VECTOR, muon)
  TRACK_FIELDS(BRANCH_VECTOR, gamma)
  TRACK_FIELDS(BRANCH_VECTOR, neutron)
  ELECTRON_FIELDS(BRANCH_VECTOR, electron)
  TRACK_FIELDS(BRANCH_VECTOR, kaon)
#define B(N) BRANCH(N);
  B(scint_trackID) B(scint_parentID) B(scint_copyNo) B(scint_pdgID)
  B(scint_energyDeposit_MeV) B(scint_entryTime_ns) B(scint_exitTime_ns)
  B(scint_entryX_m) B(scint_entryY_m) B(scint_entryZ_m)
  B(scint_exitX_m) B(scint_exitY_m) B(scint_exitZ_m)
  B(scint_entryProcess) B(scint_originVolume)
#undef B
}

void grOutputBuffer::clear() {
  TRACK_FIELDS(CLEAR_VECTOR, muon)
  TRACK_FIELDS(CLEAR_VECTOR, gamma)
  TRACK_FIELDS(CLEAR_VECTOR, neutron)
  ELECTRON_FIELDS(CLEAR_VECTOR, electron)
  TRACK_FIELDS(CLEAR_VECTOR, kaon)
#define C(N) N.clear();
  C(scint_trackID) C(scint_parentID) C(scint_copyNo) C(scint_pdgID)
  C(scint_energyDeposit_MeV) C(scint_entryTime_ns) C(scint_exitTime_ns)
  C(scint_entryX_m) C(scint_entryY_m) C(scint_entryZ_m)
  C(scint_exitX_m) C(scint_exitY_m) C(scint_exitZ_m)
  C(scint_entryProcess) C(scint_originVolume)
#undef C
}

namespace {
std::string text(const TString& value) { return value.Data(); }
#define FILL_TRACK(O, T, P) \
  O.P##_trackID.push_back(T->GetTrackID()); O.P##_pdgID.push_back(T->GetPDGID()); O.P##_parentID.push_back(T->GetParentID()); \
  O.P##_initialCopyNo.push_back(T->GetFirstCopyNo()); O.P##_finalCopyNo.push_back(T->GetLastCopyNo()); \
  O.P##_initialTime_s.push_back(T->GetTimeOfFirstProcess()); O.P##_finalTime_s.push_back(T->GetTimeOfLastProcess()); \
  O.P##_initialEnergy_MeV.push_back(T->GetInitialEnergy()); O.P##_finalEnergy_MeV.push_back(T->GetFinalEnergy()); \
  O.P##_initialX_m.push_back(T->GetFirstPositionX()); O.P##_initialY_m.push_back(T->GetFirstPositionY()); O.P##_initialZ_m.push_back(T->GetFirstPositionZ()); \
  O.P##_finalX_m.push_back(T->GetLastPositionX()); O.P##_finalY_m.push_back(T->GetLastPositionY()); O.P##_finalZ_m.push_back(T->GetLastPositionZ()); \
  O.P##_trackLength_m.push_back(T->GetTotalTrackLength()); \
  O.P##_initialProcess.push_back(text(T->GetFirstProcessName())); O.P##_finalProcess.push_back(text(T->GetLastProcessName())); \
  O.P##_initialVolume.push_back(text(T->GetFirstVolume())); O.P##_finalVolume.push_back(text(T->GetLastVolume()));
}

grHistoManager::grHistoManager() : rootFile(0), eventTree(0), output(new grOutputBuffer()) {}
grHistoManager::~grHistoManager() { delete rootFile; delete output; }

void grHistoManager::book(G4String directory) {
  const G4String fileName = directory + "grendelSim.root";
  G4cout << "GRENDEL> Create standalone ROOT file: " << fileName << G4endl;
  rootFile = TFile::Open(fileName, "RECREATE", "GRENDEL standalone output", ROOT::CompressionSettings(ROOT::kLZ4, 4));
  if (!rootFile || rootFile->IsZombie()) G4Exception("grHistoManager::book", "OutputOpenFailed", FatalException, fileName);
  eventTree = new TTree("Events", "GRENDEL event, selected-track, and sensitive-volume hit data");
  eventTree->SetAutoFlush(-50LL * 1024LL * 1024LL);
  output->branch(eventTree);
}

void grHistoManager::save() {
  if (!rootFile) return;
  rootFile->cd(); eventTree->Write(); rootFile->Close();
  G4cout << "\n----> Standalone ROOT event tree is saved\n" << G4endl;
}

void grHistoManager::FillEventNtuple(grUserEventInformation& e) {
  grOutputBuffer& o = *output;
  o.clear();
  o.schemaVersion = 2;
  o.runID = e.GetRunID(); o.eventID = e.GetEventID(); o.processID = e.GetProcessID(); o.eventWeight = e.GetEventWeight();
  o.kaonCavern = e.GetKaonCavern();

  const grMuonTrackVector& muons = *e.GetMuonTracks();
  for (std::size_t i=0; i<muons.size(); ++i) { grMuonTrack* t=muons[i]; FILL_TRACK(o,t,muon) }
  const grGammaTrackVector& gammas = *e.GetGammaTracks();
  for (std::size_t i=0; i<gammas.size(); ++i) { grGammaTrack* t=gammas[i]; FILL_TRACK(o,t,gamma) o.gamma_initialTime_s.back()*=ns/s; o.gamma_finalTime_s.back()*=ns/s; }
  const grNeutronTrackVector& neutrons = *e.GetNeutronTracks();
  for (std::size_t i=0; i<neutrons.size(); ++i) { grNeutronTrack* t=neutrons[i]; FILL_TRACK(o,t,neutron) }
  const grElectronTrackVector& electrons = *e.GetElectronTracks();
  for (std::size_t i=0; i<electrons.size(); ++i) { grElectronTrack* t=electrons[i];
    o.electron_trackID.push_back(t->GetTrackID()); o.electron_pdgID.push_back(t->GetPDGID()); o.electron_parentID.push_back(t->GetParentID());
    o.electron_initialCopyNo.push_back(t->GetFirstCopyNo()); o.electron_finalCopyNo.push_back(t->GetLastCopyNo());
    o.electron_initialTime_s.push_back(t->GetTimeOfFirstProcess()); o.electron_finalTime_s.push_back(t->GetTimeOfLastProcess());
    o.electron_initialEnergy_MeV.push_back(t->GetInitialEnergy()); o.electron_finalEnergy_MeV.push_back(t->GetFinalEnergy());
    o.electron_initialX_m.push_back(t->GetFirstPositionX()); o.electron_initialY_m.push_back(t->GetFirstPositionY()); o.electron_initialZ_m.push_back(t->GetFirstPositionZ());
    o.electron_trackLength_m.push_back(t->GetTotalTrackLength());
    o.electron_initialProcess.push_back(text(t->GetFirstProcessName())); o.electron_finalProcess.push_back(text(t->GetLastProcessName()));
    o.electron_initialVolume.push_back(text(t->GetFirstVolume())); o.electron_finalVolume.push_back(text(t->GetLastVolume()));
  }
  const grMCPTrackVector& kaons = *e.GetMCPTracks();
  for (std::size_t i=0; i<kaons.size(); ++i) { grMCPTrack* t=kaons[i]; FILL_TRACK(o,t,kaon) }

  const grScintHitVector& hits = *e.GetScintHits();
  for (std::size_t i=0; i<hits.size(); ++i) {
    grScintHit* h=hits[i]; const G4ThreeVector p=h->GetHitPosition(), q=h->GetExitPosition();
    o.scint_trackID.push_back(h->GetTrackID()); o.scint_parentID.push_back(h->GetParentID());
    o.scint_copyNo.push_back(h->GetCopyNo()); o.scint_pdgID.push_back(h->GetParticleName());
    o.scint_energyDeposit_MeV.push_back((h->GetHitEnergy()-h->GetExitEnergy())/MeV);
    o.scint_entryTime_ns.push_back(h->GetHitTime()/ns); o.scint_exitTime_ns.push_back(h->GetExitTime()/ns);
    o.scint_entryX_m.push_back(p.x()/m); o.scint_entryY_m.push_back(p.y()/m); o.scint_entryZ_m.push_back(p.z()/m);
    o.scint_exitX_m.push_back(q.x()/m); o.scint_exitY_m.push_back(q.y()/m); o.scint_exitZ_m.push_back(q.z()/m);
    o.scint_entryProcess.push_back(h->GetProcName()); o.scint_originVolume.push_back(h->GetCreatorVolName());
  }
  eventTree->Fill();
}

#include "grHistoManager.hh"
#include "grUserEventInformation.hh"

#include "Compression.h"
#include "G4SystemOfUnits.hh"
#include "TFile.h"
#include "TTree.h"

#include <string>
#include <vector>

#define TRACK_COMMON_FIELDS(X, P) \
 X(P, trackID, Int_t) X(P, parentID, Int_t) X(P, initialCopyNo, Int_t) X(P, finalCopyNo, Int_t) \
 X(P, initialTime_s, Double_t) X(P, finalTime_s, Double_t) X(P, initialEnergy_MeV, Double_t) X(P, finalEnergy_MeV, Double_t) \
 X(P, totalEnergy_MeV, Double_t) X(P, initialX_m, Double_t) X(P, initialY_m, Double_t) X(P, initialZ_m, Double_t) \
 X(P, finalX_m, Double_t) X(P, finalY_m, Double_t) X(P, finalZ_m, Double_t) X(P, trackLength_m, Double_t) \
 X(P, initialProcess, std::string) X(P, finalProcess, std::string) X(P, initialVolume, std::string) X(P, finalVolume, std::string)
#define ELECTRON_FIELDS(X, P) \
 X(P, trackID, Int_t) X(P, parentID, Int_t) X(P, initialCopyNo, Int_t) X(P, finalCopyNo, Int_t) \
 X(P, initialTime_s, Double_t) X(P, finalTime_s, Double_t) X(P, initialEnergy_MeV, Double_t) X(P, finalEnergy_MeV, Double_t) \
 X(P, totalEnergy_MeV, Double_t) X(P, initialX_m, Double_t) X(P, initialY_m, Double_t) X(P, initialZ_m, Double_t) X(P, trackLength_m, Double_t) \
 X(P, initialProcess, std::string) X(P, finalProcess, std::string) X(P, initialVolume, std::string) X(P, finalVolume, std::string)
#define DECLARE_VECTOR(P, N, T) std::vector<T> P##_##N;
#define CLEAR_VECTOR(P, N, T) P##_##N.clear();
#define BRANCH_VECTOR(P, N, T) tree->Branch(#P "_" #N, &P##_##N);

struct grOutputBuffer {
  Int_t schemaVersion, runID, eventID, processID, barHit, slabHit, panelHit;
  Int_t photonCount, photoelectronCount, cherenkovPhotonCount, scintillationPhotonCount, absorptionCount, boundaryAbsorptionCount, pmtsAboveTrigger;
  Double_t eventWeight, energyDeposit_MeV, crystalEnergyDeposit_MeV, crystalNuclearRecoilEnergyDeposit_MeV;
  Bool_t gammaOutScintillator, scintillationPhotonReachedPMT, muonTrigger, trident, tridentGamma, kaonCavern;

  TRACK_COMMON_FIELDS(DECLARE_VECTOR, muon)
  std::vector<Double_t> muon_energyDeposit_MeV, muon_initialPx_MeV, muon_initialPy_MeV, muon_initialPz_MeV;
  std::vector<Double_t> muon_finalPx_MeV, muon_finalPy_MeV, muon_finalPz_MeV;
  std::vector<Double_t> muon_energyDifference_MeV, muon_cavernEntryX, muon_cavernEntryY, muon_cavernEntryZ;
  std::vector<Double_t> muon_cavernExitX, muon_cavernExitY, muon_cavernExitZ;
  std::vector<Int_t> muon_nElastic, muon_nInteraction;
  std::vector<unsigned char> muon_triggerUp, muon_triggerLow;

  TRACK_COMMON_FIELDS(DECLARE_VECTOR, gamma)
  std::vector<Double_t> gamma_energyDeposit_MeV;
  std::vector<unsigned char> gamma_outScintillator;

  TRACK_COMMON_FIELDS(DECLARE_VECTOR, neutron)
  std::vector<Double_t> neutron_energyEnterScint_MeV;
  std::vector<Int_t> neutron_nElastic, neutron_nInteraction;

  ELECTRON_FIELDS(DECLARE_VECTOR, electron)
  std::vector<Double_t> electron_energyDeposit_MeV, electron_energyDifference_MeV;
  std::vector<Int_t> electron_nElastic, electron_nInteraction;

  TRACK_COMMON_FIELDS(DECLARE_VECTOR, mcp)
  std::vector<Double_t> mcp_energyDeposit_MeV, mcp_energyDifference_MeV;
  std::vector<Int_t> mcp_nElastic, mcp_nInteraction;

  std::vector<Int_t> photon_trackID, photon_parentID, photon_nReflections, photon_nInternalReflections;
  std::vector<Double_t> photon_initialTime_s, photon_finalTime_s, photon_initialEnergy_eV, photon_finalEnergy_eV, photon_totalEnergy_eV;
  std::vector<Double_t> photon_initialX_m, photon_initialY_m, photon_initialZ_m, photon_finalX_m, photon_finalY_m, photon_finalZ_m, photon_trackLength_m;
  std::vector<unsigned char> photon_absorbed, photon_scintToPMT;

  std::vector<Int_t> scint_trackID, scint_parentID, scint_copyNo, scint_particleID;
  std::vector<Double_t> scint_energyDeposit_MeV, scint_hitTime_ns, scint_exitTime_ns;
  std::vector<Double_t> scint_hitX_m, scint_hitY_m, scint_hitZ_m, scint_exitX_m, scint_exitY_m, scint_exitZ_m;
  std::vector<std::string> scint_process, scint_creatorVolume;

  std::vector<Int_t> pmt_number, pmt_photonCount, pmt_trackID, pmt_parentID;
  std::vector<Double_t> pmt_initialEnergy_eV, pmt_firstHitTime_ns, pmt_lastHitTime_ns, pmt_hitAngle, pmt_energyDeposit_eV;
  std::vector<Double_t> pmt_hitX_m, pmt_hitY_m, pmt_hitZ_m;

  void branch(TTree* tree);
  void clear();
};

#define BRANCH(N) tree->Branch(#N, &N)
void grOutputBuffer::branch(TTree* tree) {
  BRANCH(schemaVersion); BRANCH(runID); BRANCH(eventID); BRANCH(processID); BRANCH(eventWeight); BRANCH(energyDeposit_MeV);
  BRANCH(barHit); BRANCH(slabHit); BRANCH(panelHit); BRANCH(muonTrigger); BRANCH(trident); BRANCH(tridentGamma); BRANCH(kaonCavern);
  BRANCH(photonCount); BRANCH(photoelectronCount); BRANCH(cherenkovPhotonCount); BRANCH(scintillationPhotonCount); BRANCH(absorptionCount); BRANCH(boundaryAbsorptionCount); BRANCH(pmtsAboveTrigger); BRANCH(crystalEnergyDeposit_MeV); BRANCH(crystalNuclearRecoilEnergyDeposit_MeV); BRANCH(gammaOutScintillator); BRANCH(scintillationPhotonReachedPMT);
  TRACK_COMMON_FIELDS(BRANCH_VECTOR, muon) TRACK_COMMON_FIELDS(BRANCH_VECTOR, gamma)
  TRACK_COMMON_FIELDS(BRANCH_VECTOR, neutron) ELECTRON_FIELDS(BRANCH_VECTOR, electron) TRACK_COMMON_FIELDS(BRANCH_VECTOR, mcp)
#define B(N) BRANCH(N);
  B(muon_energyDeposit_MeV) B(muon_initialPx_MeV) B(muon_initialPy_MeV) B(muon_initialPz_MeV) B(muon_finalPx_MeV) B(muon_finalPy_MeV) B(muon_finalPz_MeV)
  B(muon_energyDifference_MeV) B(muon_cavernEntryX) B(muon_cavernEntryY) B(muon_cavernEntryZ) B(muon_cavernExitX) B(muon_cavernExitY) B(muon_cavernExitZ) B(muon_nElastic) B(muon_nInteraction) B(muon_triggerUp) B(muon_triggerLow)
  B(gamma_energyDeposit_MeV) B(gamma_outScintillator) B(neutron_energyEnterScint_MeV) B(neutron_nElastic) B(neutron_nInteraction)
  B(electron_energyDeposit_MeV) B(electron_energyDifference_MeV) B(electron_nElastic) B(electron_nInteraction)
  B(mcp_energyDeposit_MeV) B(mcp_energyDifference_MeV) B(mcp_nElastic) B(mcp_nInteraction)
  B(photon_trackID) B(photon_parentID) B(photon_nReflections) B(photon_nInternalReflections) B(photon_initialTime_s) B(photon_finalTime_s) B(photon_initialEnergy_eV) B(photon_finalEnergy_eV) B(photon_totalEnergy_eV) B(photon_initialX_m) B(photon_initialY_m) B(photon_initialZ_m) B(photon_finalX_m) B(photon_finalY_m) B(photon_finalZ_m) B(photon_trackLength_m) B(photon_absorbed) B(photon_scintToPMT)
  B(scint_trackID) B(scint_parentID) B(scint_copyNo) B(scint_particleID) B(scint_energyDeposit_MeV) B(scint_hitTime_ns) B(scint_exitTime_ns) B(scint_hitX_m) B(scint_hitY_m) B(scint_hitZ_m) B(scint_exitX_m) B(scint_exitY_m) B(scint_exitZ_m) B(scint_process) B(scint_creatorVolume)
  B(pmt_number) B(pmt_photonCount) B(pmt_trackID) B(pmt_parentID) B(pmt_initialEnergy_eV) B(pmt_firstHitTime_ns) B(pmt_lastHitTime_ns) B(pmt_hitAngle) B(pmt_energyDeposit_eV) B(pmt_hitX_m) B(pmt_hitY_m) B(pmt_hitZ_m)
#undef B
}

void grOutputBuffer::clear() {
  TRACK_COMMON_FIELDS(CLEAR_VECTOR, muon) TRACK_COMMON_FIELDS(CLEAR_VECTOR, gamma)
  TRACK_COMMON_FIELDS(CLEAR_VECTOR, neutron) ELECTRON_FIELDS(CLEAR_VECTOR, electron) TRACK_COMMON_FIELDS(CLEAR_VECTOR, mcp)
#define C(N) N.clear();
  C(muon_energyDeposit_MeV) C(muon_initialPx_MeV) C(muon_initialPy_MeV) C(muon_initialPz_MeV) C(muon_finalPx_MeV) C(muon_finalPy_MeV) C(muon_finalPz_MeV) C(muon_energyDifference_MeV) C(muon_cavernEntryX) C(muon_cavernEntryY) C(muon_cavernEntryZ) C(muon_cavernExitX) C(muon_cavernExitY) C(muon_cavernExitZ) C(muon_nElastic) C(muon_nInteraction) C(muon_triggerUp) C(muon_triggerLow)
  C(gamma_energyDeposit_MeV) C(gamma_outScintillator) C(neutron_energyEnterScint_MeV) C(neutron_nElastic) C(neutron_nInteraction) C(electron_energyDeposit_MeV) C(electron_energyDifference_MeV) C(electron_nElastic) C(electron_nInteraction) C(mcp_energyDeposit_MeV) C(mcp_energyDifference_MeV) C(mcp_nElastic) C(mcp_nInteraction)
  C(photon_trackID) C(photon_parentID) C(photon_nReflections) C(photon_nInternalReflections) C(photon_initialTime_s) C(photon_finalTime_s) C(photon_initialEnergy_eV) C(photon_finalEnergy_eV) C(photon_totalEnergy_eV) C(photon_initialX_m) C(photon_initialY_m) C(photon_initialZ_m) C(photon_finalX_m) C(photon_finalY_m) C(photon_finalZ_m) C(photon_trackLength_m) C(photon_absorbed) C(photon_scintToPMT)
  C(scint_trackID) C(scint_parentID) C(scint_copyNo) C(scint_particleID) C(scint_energyDeposit_MeV) C(scint_hitTime_ns) C(scint_exitTime_ns) C(scint_hitX_m) C(scint_hitY_m) C(scint_hitZ_m) C(scint_exitX_m) C(scint_exitY_m) C(scint_exitZ_m) C(scint_process) C(scint_creatorVolume)
  C(pmt_number) C(pmt_photonCount) C(pmt_trackID) C(pmt_parentID) C(pmt_initialEnergy_eV) C(pmt_firstHitTime_ns) C(pmt_lastHitTime_ns) C(pmt_hitAngle) C(pmt_energyDeposit_eV) C(pmt_hitX_m) C(pmt_hitY_m) C(pmt_hitZ_m)
#undef C
}

namespace {
std::string text(const TString& value) { return value.Data(); }
#define FILL_COMMON(O, T, P) \
 O.P##_trackID.push_back(T->GetTrackID()); O.P##_parentID.push_back(T->GetParentID()); O.P##_initialCopyNo.push_back(T->GetFirstCopyNo()); O.P##_finalCopyNo.push_back(T->GetLastCopyNo()); \
 O.P##_initialTime_s.push_back(T->GetTimeOfFirstProcess()); O.P##_finalTime_s.push_back(T->GetTimeOfLastProcess()); O.P##_initialEnergy_MeV.push_back(T->GetInitialEnergy()); O.P##_finalEnergy_MeV.push_back(T->GetFinalEnergy()); O.P##_totalEnergy_MeV.push_back(T->GetTotalEnergy()); \
 O.P##_initialX_m.push_back(T->GetFirstPositionX()); O.P##_initialY_m.push_back(T->GetFirstPositionY()); O.P##_initialZ_m.push_back(T->GetFirstPositionZ()); O.P##_finalX_m.push_back(T->GetLastPositionX()); O.P##_finalY_m.push_back(T->GetLastPositionY()); O.P##_finalZ_m.push_back(T->GetLastPositionZ()); O.P##_trackLength_m.push_back(T->GetTotalTrackLength()); \
 O.P##_initialProcess.push_back(text(T->GetFirstProcessName())); O.P##_finalProcess.push_back(text(T->GetLastProcessName())); O.P##_initialVolume.push_back(text(T->GetFirstVolume())); O.P##_finalVolume.push_back(text(T->GetLastVolume()));
}

grHistoManager::grHistoManager() : rootFile(0), eventTree(0), output(new grOutputBuffer()) {}
grHistoManager::~grHistoManager() { delete rootFile; delete output; }

void grHistoManager::book(G4String directory) {
  const G4String fileName = directory + "grendelSim.root";
  G4cout << "GRENDEL> Create standalone ROOT file: " << fileName << G4endl;
  rootFile = TFile::Open(fileName, "RECREATE", "GRENDEL standalone output", ROOT::CompressionSettings(ROOT::kLZ4, 4));
  if (!rootFile || rootFile->IsZombie()) G4Exception("grHistoManager::book", "OutputOpenFailed", FatalException, fileName);
  eventTree = new TTree("Events", "GRENDEL standalone event data");
  eventTree->SetAutoFlush(-50LL * 1024LL * 1024LL);
  output->branch(eventTree);
}

void grHistoManager::save() {
  if (!rootFile) return;
  rootFile->cd(); eventTree->Write(); rootFile->Close();
  G4cout << "\n----> Standalone ROOT event tree is saved\n" << G4endl;
}

void grHistoManager::FillEventNtuple(grUserEventInformation& e) {
  grOutputBuffer& o=*output; o.clear(); o.schemaVersion=1; o.runID=e.GetRunID(); o.eventID=e.GetEventID(); o.processID=e.GetProcessID(); o.eventWeight=e.GetEventWeight(); o.energyDeposit_MeV=e.GetEventEnergyDeposit();
  o.barHit=e.GetBarHit(); o.slabHit=e.GetSlabHit(); o.panelHit=e.GetPanelHit(); o.muonTrigger=e.GetMuonTrigger(); o.trident=e.GetTrident(); o.tridentGamma=e.GetTridentGamma(); o.kaonCavern=e.GetKaonCavern();
  o.photonCount=e.GetPhotonCount(); o.photoelectronCount=e.GetPECountPMT(); o.cherenkovPhotonCount=e.GetPhotonCount_Cheren(); o.scintillationPhotonCount=e.GetPhotonCount_Scint(); o.absorptionCount=e.GetAbsorptionCount(); o.boundaryAbsorptionCount=e.GetBoundaryAbsorptionCount(); o.pmtsAboveTrigger=e.GetPMTSAboveThreshold(); o.crystalEnergyDeposit_MeV=e.GetCrystalEDep()/MeV; o.crystalNuclearRecoilEnergyDeposit_MeV=e.GetCrystalENRDep()/MeV; o.gammaOutScintillator=e.GetGammaOutScintillator(); o.scintillationPhotonReachedPMT=e.GetScintToPMT();

  const grMuonTrackVector& muons=*e.GetMuonTracks();
  for(std::size_t i=0;i<muons.size();++i){grMuonTrack* t=muons[i]; FILL_COMMON(o,t,muon)
    o.muon_energyDeposit_MeV.push_back(t->GetEnergyDeposit()); o.muon_initialPx_MeV.push_back(t->GetFirstMomentumX()); o.muon_initialPy_MeV.push_back(t->GetFirstMomentumY()); o.muon_initialPz_MeV.push_back(t->GetFirstMomentumZ()); o.muon_finalPx_MeV.push_back(t->GetLastMomentumX()); o.muon_finalPy_MeV.push_back(t->GetLastMomentumY()); o.muon_finalPz_MeV.push_back(t->GetLastMomentumZ());
    o.muon_energyDifference_MeV.push_back(t->GetEnergyDiff()); o.muon_cavernEntryX.push_back(t->GetXposition()); o.muon_cavernEntryY.push_back(t->GetYposition()); o.muon_cavernEntryZ.push_back(t->GetZposition()); o.muon_cavernExitX.push_back(t->GetXfposition()); o.muon_cavernExitY.push_back(t->GetYfposition()); o.muon_cavernExitZ.push_back(t->GetZfposition()); o.muon_nElastic.push_back(t->GetNbOfElastics()); o.muon_nInteraction.push_back(t->GetNbOfInteractions()); o.muon_triggerUp.push_back(t->GetMuonTriggerUp()); o.muon_triggerLow.push_back(t->GetMuonTriggerLow()); }

  const grGammaTrackVector& gammas=*e.GetGammaTracks();
  for(std::size_t i=0;i<gammas.size();++i){grGammaTrack* t=gammas[i]; FILL_COMMON(o,t,gamma) o.gamma_initialTime_s.back() *= ns/s; o.gamma_finalTime_s.back() *= ns/s; o.gamma_energyDeposit_MeV.push_back(t->GetEnergyDeposit()); o.gamma_outScintillator.push_back(t->GetGammaOutScintillator());}
  const grNeutronTrackVector& neutrons=*e.GetNeutronTracks();
  for(std::size_t i=0;i<neutrons.size();++i){grNeutronTrack* t=neutrons[i]; FILL_COMMON(o,t,neutron) o.neutron_energyEnterScint_MeV.push_back(t->GetEnergyEnterScinti()); o.neutron_nElastic.push_back(t->GetNbOfElastics()); o.neutron_nInteraction.push_back(t->GetNbOfInteractions());}
  const grMCPTrackVector& mcps=*e.GetMCPTracks();
  for(std::size_t i=0;i<mcps.size();++i){grMCPTrack* t=mcps[i]; FILL_COMMON(o,t,mcp) o.mcp_energyDeposit_MeV.push_back(t->GetEnergyDeposit()); o.mcp_energyDifference_MeV.push_back(t->GetEnergyDiff()); o.mcp_nElastic.push_back(t->GetNbOfElastics()); o.mcp_nInteraction.push_back(t->GetNbOfInteractions());}

  const grElectronTrackVector& electrons=*e.GetElectronTracks();
  for(std::size_t i=0;i<electrons.size();++i){grElectronTrack* t=electrons[i];
    o.electron_trackID.push_back(t->GetTrackID()); o.electron_parentID.push_back(t->GetParentID()); o.electron_initialCopyNo.push_back(t->GetFirstCopyNo()); o.electron_finalCopyNo.push_back(t->GetLastCopyNo()); o.electron_initialTime_s.push_back(t->GetTimeOfFirstProcess()); o.electron_finalTime_s.push_back(t->GetTimeOfLastProcess()); o.electron_initialEnergy_MeV.push_back(t->GetInitialEnergy()); o.electron_finalEnergy_MeV.push_back(t->GetFinalEnergy()); o.electron_totalEnergy_MeV.push_back(t->GetTotalEnergy()); o.electron_energyDeposit_MeV.push_back(t->GetEnergyDeposit()); o.electron_initialX_m.push_back(t->GetFirstPositionX()); o.electron_initialY_m.push_back(t->GetFirstPositionY()); o.electron_initialZ_m.push_back(t->GetFirstPositionZ()); o.electron_trackLength_m.push_back(t->GetTotalTrackLength()); o.electron_energyDifference_MeV.push_back(t->GetEnergyDiff()); o.electron_initialProcess.push_back(text(t->GetFirstProcessName())); o.electron_finalProcess.push_back(text(t->GetLastProcessName())); o.electron_initialVolume.push_back(text(t->GetFirstVolume())); o.electron_finalVolume.push_back(text(t->GetLastVolume())); o.electron_nElastic.push_back(t->GetNbOfElastics()); o.electron_nInteraction.push_back(t->GetNbOfInteractions()); }

  const grPhotonTrackVector& photons=*e.GetPhotonTracks();
  for(std::size_t i=0;i<photons.size();++i){grPhotonTrack* t=photons[i]; o.photon_trackID.push_back(t->GetTrackID()); o.photon_parentID.push_back(t->GetParentID()); o.photon_nReflections.push_back(t->GetNbOfReflections()); o.photon_nInternalReflections.push_back(t->GetNbOfIntReflections()); o.photon_absorbed.push_back(t->GetAbsorption()); o.photon_scintToPMT.push_back(t->GetScintToPMT()); o.photon_initialTime_s.push_back(t->GetTimeOfFirstProcess()); o.photon_finalTime_s.push_back(t->GetTimeOfLastProcess()); o.photon_initialEnergy_eV.push_back(t->GetInitialEnergy()); o.photon_finalEnergy_eV.push_back(t->GetFinalEnergy()); o.photon_totalEnergy_eV.push_back(t->GetTotalEnergy()); o.photon_initialX_m.push_back(t->GetFirstPositionX()); o.photon_initialY_m.push_back(t->GetFirstPositionY()); o.photon_initialZ_m.push_back(t->GetFirstPositionZ()); o.photon_finalX_m.push_back(t->GetLastPositionX()); o.photon_finalY_m.push_back(t->GetLastPositionY()); o.photon_finalZ_m.push_back(t->GetLastPositionZ()); o.photon_trackLength_m.push_back(t->GetTotalTrackLength()); }

  const grScintHitVector& scint=*e.GetScintHits();
  for(std::size_t i=0;i<scint.size();++i){grScintHit* h=scint[i]; G4ThreeVector p=h->GetHitPosition(), q=h->GetExitPosition(); o.scint_trackID.push_back(h->GetTrackID()); o.scint_parentID.push_back(h->GetParentID()); o.scint_copyNo.push_back(h->GetCopyNo()); o.scint_particleID.push_back(h->GetParticleName()); o.scint_energyDeposit_MeV.push_back((h->GetHitEnergy()-h->GetExitEnergy())/MeV); o.scint_hitTime_ns.push_back(h->GetHitTime()/ns); o.scint_exitTime_ns.push_back(h->GetExitTime()/ns); o.scint_hitX_m.push_back(p.x()/m); o.scint_hitY_m.push_back(p.y()/m); o.scint_hitZ_m.push_back(p.z()/m); o.scint_exitX_m.push_back(q.x()/m); o.scint_exitY_m.push_back(q.y()/m); o.scint_exitZ_m.push_back(q.z()/m); o.scint_process.push_back(h->GetProcName()); o.scint_creatorVolume.push_back(h->GetCreatorVolName()); }

  const grPMTHitVector& pmts=*e.GetPMTHits();
  for(std::size_t i=0;i<pmts.size();++i){grPMTHit* h=pmts[i]; G4ThreeVector p=h->GetHitPosition(); o.pmt_number.push_back(h->GetPMTNumber()); o.pmt_photonCount.push_back(h->GetPhotonCount()); o.pmt_trackID.push_back(h->GetTrackID()); o.pmt_parentID.push_back(h->GetParentID()); o.pmt_initialEnergy_eV.push_back(h->GetInitialEDep()/eV); o.pmt_firstHitTime_ns.push_back(h->GetHitTime()/ns); o.pmt_lastHitTime_ns.push_back(h->GetLastHitTime()/ns); o.pmt_hitAngle.push_back(h->GetHitAngle()); o.pmt_energyDeposit_eV.push_back(h->GetEDep()/eV); o.pmt_hitX_m.push_back(p.x()/m); o.pmt_hitY_m.push_back(p.y()/m); o.pmt_hitZ_m.push_back(p.z()/m); }
  eventTree->Fill();
}

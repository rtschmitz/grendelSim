#include "grEventAction.hh"
#include "grHistoManager.hh"
#include "grScintHit.hh"
#include "grUserEventInformation.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4ios.hh"

grEventAction::grEventAction(grHistoManager* histo, G4int, G4double eventWeight, G4int processID)
  : histoManager(histo), scintCollID(-1), verbose(0),
    fEventWeight(eventWeight), fProcessID(processID) {}

grEventAction::~grEventAction() {}

void grEventAction::BeginOfEventAction(const G4Event* event) {
  grUserEventInformation* info = static_cast<grUserEventInformation*>(event->GetUserInformation());
  if (!info) {
    info = new grUserEventInformation();
    G4EventManager::GetEventManager()->SetUserInformation(info);
  }

  if (scintCollID < 0 && G4SDManager::GetSDMpointer()->FindSensitiveDetector("Scint_SD", false))
    scintCollID = G4SDManager::GetSDMpointer()->GetCollectionID("scintCollection");

  info->Reset();
  info->SetEventID(event->GetEventID());
  info->SetRunID(G4RunManager::GetRunManager()->GetCurrentRun()->GetRunID());
  info->SetGammaLastTrackID(-1);
  info->SetNeutronLastTrackID(-1);
  info->SetMuonLastTrackID(-1);
  info->SetElectronLastTrackID(-1);
  info->SetMCPLastTrackID(-1);
  info->SetEventWeight(fEventWeight);
  info->SetProcessID(fProcessID);

  if (verbose >= 1) G4cout << ">> Enter event #" << event->GetEventID() << G4endl;
}

void grEventAction::EndOfEventAction(const G4Event* event) {
  grUserEventInformation* info = static_cast<grUserEventInformation*>(event->GetUserInformation());
  G4HCofThisEvent* collections = event->GetHCofThisEvent();
  grScintHitsCollection* hits = collections && scintCollID >= 0
    ? static_cast<grScintHitsCollection*>(collections->GetHC(scintCollID)) : 0;

  if (hits) {
    const grScintHitVector& hitVector = *hits->GetVector();
    for (std::size_t i=0; i<hitVector.size(); ++i) info->AddScintHit(hitVector[i]);
  }

  info->Finalize();
  if (verbose > 0) G4cout << "\tSensitive-volume hits: " << info->GetScintHits()->size() << G4endl;

  histoManager->FillEventNtuple(*info);
  info->Reset();
}

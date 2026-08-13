#include "grSteppingAction.hh"
#include "grScintSD.hh"
#include "grUserEventInformation.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4LogicalVolume.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"

namespace {
grScintSD* FindScintDetector()
{
  static grScintSD* detector = 0;
  if (!detector) {
    detector = dynamic_cast<grScintSD*>(
        G4SDManager::GetSDMpointer()->FindSensitiveDetector("Scint_SD", false));
  }
  return detector;
}

G4bool UsesDetector(const G4VPhysicalVolume* volume, const grScintSD* detector)
{
  return volume && volume->GetLogicalVolume()->GetSensitiveDetector() == detector;
}
}

grSteppingAction::grSteppingAction(grHistoManager* histo)
  : oneStepPrimaries(false), steppingMessenger(new grSteppingMessenger(this)),
    histoManager(histo), killPhoton(false) {}

grSteppingAction::~grSteppingAction()
{
  delete steppingMessenger;
}

void grSteppingAction::UserSteppingAction(const G4Step* step)
{
  const G4Track* track = step->GetTrack();
  if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) return;
  const G4StepPoint* post = step->GetPostStepPoint();
  if (!post || post->GetStepStatus() != fGeomBoundary) return;

  const G4StepPoint* pre = step->GetPreStepPoint();
  const G4VPhysicalVolume* preVolume = pre ? pre->GetPhysicalVolume() : 0;
  const G4VPhysicalVolume* postVolume = post->GetPhysicalVolume();

  // Ignore ordinary transportation steps and replica/copy bookkeeping that did
  // not move the track into a different physical detector element.
  if (preVolume == postVolume &&
      pre->GetTouchable()->GetCopyNumber() == post->GetTouchable()->GetCopyNumber()) return;

  grScintSD* detector = FindScintDetector();
  if (!detector) return;

  const G4bool exitsSensitive = UsesDetector(preVolume, detector);
  const G4bool entersSensitive = UsesDetector(postVolume, detector);
  if (!exitsSensitive && !entersSensitive) return;

  if (track->GetDefinition()->GetParticleName().contains("kaon")) {
    grUserEventInformation* event = static_cast<grUserEventInformation*>(
        G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
    if (event) event->SetKaonCavern(true);
  }

  if (exitsSensitive) detector->ProcessHitsExit(step, 0);
  if (entersSensitive) detector->ProcessHitsEnter(step, 0);
}

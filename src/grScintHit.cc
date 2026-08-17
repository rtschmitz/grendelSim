#include "grScintHit.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

G4Allocator<grScintHit> grScintHitAllocator;

grScintHit::grScintHit()
  : trackID(-1), parentID(-1), kineticEnergy(0.), hitTime(0.),
    hitPosition(), direction(), particleName(0), procName(), originVolumeID(0), copyNo(-1), entering(false) {}

grScintHit::~grScintHit() {}

grScintHit::grScintHit(const grScintHit& right) : G4VHit(right) { *this = right; }

const grScintHit& grScintHit::operator=(const grScintHit& right) {
  trackID = right.trackID; parentID = right.parentID;
  kineticEnergy = right.kineticEnergy; hitTime = right.hitTime;
  hitPosition = right.hitPosition; direction = right.direction;
  particleName = right.particleName; procName = right.procName;
  originVolumeID = right.originVolumeID; copyNo = right.copyNo; entering = right.entering;
  return *this;
}

G4int grScintHit::operator==(const grScintHit& right) const { return this == &right; }
void grScintHit::Draw() {}
void grScintHit::Print() {
  G4cout << "  trackID: " << trackID << "  Parent ID: " << parentID
         << "  Kinetic energy: " << G4BestUnit(kineticEnergy, "Energy")
         << (entering ? "  Entry position: " : "  Exit position: ") << G4BestUnit(hitPosition, "Length") << G4endl;
}

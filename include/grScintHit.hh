#ifndef GRSCINTHIT_HH_
#define GRSCINTHIT_HH_

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include <vector>

class grScintHit : public G4VHit {
public:
  grScintHit();
  ~grScintHit();
  grScintHit(const grScintHit&);
  const grScintHit& operator=(const grScintHit&);
  G4int operator==(const grScintHit&) const;

  inline void* operator new(size_t);
  inline void operator delete(void*);

  void Draw();
  void Print();

  void SetTrackID(G4int value) { trackID = value; }
  G4int GetTrackID() const { return trackID; }
  void SetParentID(G4int value) { parentID = value; }
  G4int GetParentID() const { return parentID; }
  void SetKineticEnergy(G4double value) { kineticEnergy = value; }
  G4double GetKineticEnergy() const { return kineticEnergy; }
  void SetHitTime(G4double value) { hitTime = value; }
  G4double GetHitTime() const { return hitTime; }
  void SetParticleName(G4int value) { particleName = value; }
  G4int GetParticleName() const { return particleName; }
  void SetCopyNo(G4int value) { copyNo = value; }
  G4int GetCopyNo() const { return copyNo; }
  void SetHitPosition(G4ThreeVector value) { hitPosition = value; }
  G4ThreeVector GetHitPosition() const { return hitPosition; }
  void SetDirection(G4ThreeVector value) { direction = value; }
  G4ThreeVector GetDirection() const { return direction; }
  void SetProcName(G4String value) { procName = value; }
  G4String GetProcName() const { return procName; }
  void SetCreatorVolName(G4String value) { creatorVolName = value; }
  G4String GetCreatorVolName() const { return creatorVolName; }

private:
  G4int trackID;
  G4int parentID;
  G4double kineticEnergy;
  G4double hitTime;
  G4ThreeVector hitPosition;
  G4ThreeVector direction;
  G4int particleName;
  G4String procName;
  G4String creatorVolName;
  G4int copyNo;
};

typedef std::vector<grScintHit*> grScintHitVector;
typedef G4THitsCollection<grScintHit> grScintHitsCollection;
extern G4Allocator<grScintHit> grScintHitAllocator;

inline void* grScintHit::operator new(size_t) {
  return static_cast<void*>(grScintHitAllocator.MallocSingle());
}
inline void grScintHit::operator delete(void* hit) {
  grScintHitAllocator.FreeSingle(static_cast<grScintHit*>(hit));
}

#endif

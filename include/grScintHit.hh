/*
 * grScintHit.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSCINTHIT_HH_
#define MQSCINTHIT_HH_

#include "grScintRHit.hh"

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Track.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include <vector>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class grScintHit : public G4VHit
{
  public:

    grScintHit();
    ~grScintHit();
    grScintHit(const grScintHit&);
    const grScintHit& operator=(const grScintHit&);
    G4int operator==(const grScintHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void Draw();
    void Print();

    void SetTrackID(G4int track){ trackID = track; };
    G4int    GetTrackID()const{ return trackID; };

    void SetParentID(G4int pid){ parentID  = pid; };
    G4int    GetParentID()const{ return parentID; };

    void SetHitEnergy(G4double ed){ hitEnergy    = ed;    };
    G4double GetHitEnergy()const{ return hitEnergy; };

    void SetExitEnergy(G4double ed){ exitEnergy    = ed;    };
    G4double GetExitEnergy()const{ return exitEnergy; };

    void SetHitTime(G4double tm){ hitTime    = tm;    };
    G4double GetHitTime()const{ return hitTime; };

    void SetExitTime(G4double tm){ exitTime    = tm;    };
    G4double GetExitTime()const{ return exitTime; };

    void SetParticleName(G4int pn){ particleName   = pn;    };
    G4int GetParticleName()const{ return particleName; };

    void SetCopyNo(G4int nb){ copyNo  = nb;   };
    G4int    GetCopyNo()const{ return copyNo; };

    void SetHitPosition     (G4ThreeVector xyz){ hitPosition     = xyz;   };
    G4ThreeVector GetHitPosition()const{ return hitPosition; };

    void SetExitPosition     (G4ThreeVector xyz){ exitPosition     = xyz;   };
    G4ThreeVector GetExitPosition()const{ return exitPosition; };

    void SetProcName(G4String pn){ procName   = pn;    };
    G4String GetProcName()const{ return procName; };

    void SetCreatorVolName(G4String vn){ creatorVolName   = vn;    };
    G4String GetCreatorVolName()const{ return creatorVolName; };

    grScintRHit* ConvertToROOTHit() const;

  private:

    G4int         trackID;
    G4int         parentID;
    G4double      hitEnergy;
    G4double      exitEnergy;
    G4double      hitTime;
    G4double      exitTime;
    G4ThreeVector hitPosition;
    G4ThreeVector exitPosition;
    G4int         particleName;
    G4String      procName;
    G4String      creatorVolName;
    G4int         copyNo;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
typedef std::vector<grScintHit*> grScintHitVector;
typedef G4THitsCollection<grScintHit> grScintHitsCollection;

extern G4Allocator<grScintHit> grScintHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* grScintHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) grScintHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void grScintHit::operator delete(void *aHit)
{
  grScintHitAllocator.FreeSingle((grScintHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif /* MQSCINTHIT_HH_ */

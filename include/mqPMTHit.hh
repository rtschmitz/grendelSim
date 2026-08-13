/*
 * mqPMTHit.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQPMTHIT_HH_
#define MQPMTHIT_HH_

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "mqPMTRHit.hh"

//#include "G4VPhysicalVolume.hh"
#include <vector>

//class G4VTouchable;  OLD: G4.9
#include "G4VTouchable.hh"

class mqPMTHit : public G4VHit
{
public:

  mqPMTHit();
  ~mqPMTHit();
  mqPMTHit(const mqPMTHit &right);

  const mqPMTHit& operator=(const mqPMTHit &right);
  G4int operator==(const mqPMTHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();

  inline void SetDrawit(G4bool b){drawit=b;}
  inline G4bool GetDrawit()const{return drawit;}

   void IncPhotonCount(){photons++;}
   G4int GetPhotonCount()const{return photons;}

   void SetPMTNumber(G4int n) { pmtNumber = n; }
   G4int GetPMTNumber()const{ return pmtNumber; }

  //inline void SetPMTPhysVol(G4VPhysicalVolume* physVol){this->physVol=physVol;}
  //inline G4VPhysicalVolume* GetPMTPhysVol(){return physVol;}

  inline void SetPMTPos(G4double x,G4double y,G4double z){
    pos=G4ThreeVector(x,y,z);}
  inline G4ThreeVector GetPMTPos(){return pos;}

   void SetInitialEDep(G4double E){energy = E;} // Sets energy of first hit
   G4double GetInitialEDep()const{return energy;}

   void SetHitTime(G4double gtime){ hitTime = gtime;}
   G4double GetHitTime()const{return hitTime;}

   void SetLastHitTime(G4double gtimeFinal){finalHitTime = gtimeFinal;}
   G4double GetLastHitTime()const{return finalHitTime; }
   
   void SetHitAngle(G4double ang){hitAngle = ang;}
   G4double GetHitAngle()const{return hitAngle; }

   void SetHitPosition(G4ThreeVector hitpos){position = hitpos;}
   G4ThreeVector GetHitPosition()const{return position;}

   void SetTrackID(G4int tid){trackID = tid;}
   G4int GetTrackID()const{return trackID;}

   void IncEDep(G4double dep){totE+=dep;}
   G4double GetEDep()const{return totE;}

  void SetParentID(G4int pid){parentID = pid;}
  G4int GetParentID()const{return parentID;}

  void SetEventID(G4int eid){eventID = eid;}
  G4int GetEventID()const{return eventID;}

  inline void SetScintToPMT(G4bool set){scintToPMT = set;}
  inline G4bool GetScintToPMT()const{return scintToPMT;}

  static bool compareHits(mqPMTHit* const &a, mqPMTHit* const &b);

  mqPMTRHit* ConvertToROOTHit() const;



private:
  G4int eventID;
  G4int parentID;
  G4int pmtNumber;
  G4int photons;
  G4ThreeVector pos;
  //G4VPhysicalVolume* physVol;
  G4bool drawit;
  G4double hitAngle;
  G4double energy;
  G4double hitTime;
  G4double finalHitTime;
  G4ThreeVector position;
  G4int trackID;
  G4double totE;
  G4bool scintToPMT;


};

typedef G4THitsCollection<mqPMTHit> mqPMTHitsCollection;
typedef std::vector<mqPMTHit*> mqPMTHitVector;

extern G4Allocator<mqPMTHit> mqPMTHitAllocator;

inline void* mqPMTHit::operator new(size_t){
  void *aHit;
  aHit = (void *) mqPMTHitAllocator.MallocSingle();
  return aHit;
}

inline void mqPMTHit::operator delete(void *aHit){
  mqPMTHitAllocator.FreeSingle((mqPMTHit*) aHit);
}



#endif /* MQPMTHIT_HH_ */

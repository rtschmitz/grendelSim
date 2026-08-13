/*
 * grPMTRHit.cc
 * PMT Hit Class which is compatible to ROOT.
 * It consists of the same methods and variables but does not inherit from G4VHit but from TObject
 * (in order to be able store the information in a TTree).
 *
 *  Created on: 22.04.2019

 *      Author: schmitz
 */

#include "grPMTRHit.hh"

#include <iomanip>

ClassImp(grPMTRHit)
//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grPMTRHit::grPMTRHit()
  :pmtNumber(-1), //totalPE(0),
   //initalEDep_eV(0.),
   initialHitTime_ns(0.),
   //finalHitTime_ns(0.), hitPosition(),
   trackID(-1), //totalEDep_eV(0.),
   parentID(-1) //scintToPMT(false)
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grPMTRHit::~grPMTRHit()
{}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
grPMTRHit::grPMTRHit(const grPMTRHit &right) :
    TObject()
{
    pmtNumber         = right.pmtNumber      ;
//    totalPE            = right.totalPE        ;
//    initalEDep_eV     = right.initalEDep_eV     ;
    initialHitTime_ns = right.initialHitTime_ns ;
//    finalHitTime_ns   = right.finalHitTime_ns   ;
//    hitPosition       = right.hitPosition    ;
//    totalEDep_eV      = right.totalEDep_eV   ;
    trackID           = right.trackID        ;
    parentID          = right.parentID       ;
//    scintToPMT      = right.scintToPMT    ;

//G4cout << "hitPosition X " << hitPosition.x() << std::endl;
//G4cout << "hitPosition Y " << hitPosition.y() << std::endl;
//G4cout << "hitPosition Z " << hitPosition.z() << std::endl;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
const grPMTRHit& grPMTRHit::operator=(const grPMTRHit &right){
    pmtNumber         = right.pmtNumber      ;
//    totalPE            = right.totalPE        ;
//    initalEDep_eV     = right.initalEDep_eV     ;
    initialHitTime_ns = right.initialHitTime_ns ;
//    finalHitTime_ns   = right.finalHitTime_ns   ;
//    hitPosition       = right.hitPosition    ;
//    totalEDep_eV      = right.totalEDep_eV   ;
    trackID           = right.trackID        ;
    parentID          = right.parentID       ;
//    scintToPMT      = right.scintToPMT    ;

//G4cout << "hitPosition X " << hitPosition.x() << std::endl;
//G4cout << "hitPosition Y " << hitPosition.y() << std::endl;
//G4cout << "hitPosition Z " << hitPosition.z() << std::endl;
return *this;
}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
int grPMTRHit::operator==(const grPMTRHit &right) const{
  return (pmtNumber == right.pmtNumber);

}

//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
bool grPMTRHit::compareHits(grPMTRHit* const &a, grPMTRHit* const &b){
  Double_t aTime = a->GetFirstHitTime();
  Double_t bTime = b->GetFirstHitTime();
  return (aTime < bTime);
}


//_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
//void grPMTRHit::Print(){
//
//  std::cout
//       << " trackID: " << trackID << " pmtNumber: " << pmtNumber
//       << " Number of photons: "
//       << std::setw(7) << nbPhotons
//       << " Parent ID: "
//       << std::setw(7) << parentID
//       << " Hit time: "
//       << std::setw(7) << initialHitTime
//       << " Total energy deposit: "
//       << std::setw(7) << totalEDep
//       << std::endl;
//
//}

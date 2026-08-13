/*
 * grGammaTrack.cc
 *
 * Gamma track class. This class allows to store all necessary information about
 * gamma tracks such as ID, track length, ... .
 * It is an in-memory record populated by grTrackingAction.
 * Track informations are assigned in the grTrackingAction.
 *
 * Created on: 22.04.2019
 * Author: schmitz
 */
#include "grGammaTrack.hh"


//==============================================================================

grGammaTrack::grGammaTrack() :
      trackID(-1),
      pdgID(0),
      initialTime_ns(0.), finalTime_ns(0.),
      initialEnergy_MeV(0.),finalEnergy_MeV(0.),
      parentID(-1),
      initialPositionX_m(0.),finalPositionX_m(0.),
      initialPositionY_m(0.),finalPositionY_m(0.),
      initialPositionZ_m(0.),finalPositionZ_m(0.),
      totalTrackLength_m(0.0) {

}


//==============================================================================
grGammaTrack::~grGammaTrack() {
}
const grGammaTrack& grGammaTrack::operator=(const grGammaTrack &right){

  trackID           = right.trackID          ;
  pdgID            = right.pdgID;
  initialTime_ns        = right.initialTime_ns       ;
  finalTime_ns          = right.finalTime_ns         ;
  initialEnergy_MeV     = right.initialEnergy_MeV    ;
  finalEnergy_MeV       = right.finalEnergy_MeV      ;
  parentID              = right.parentID             ;
  initialPositionX_m    = right.initialPositionX_m   ;
  finalPositionX_m      = right.finalPositionX_m     ;
  initialPositionY_m    = right.initialPositionY_m   ;
  finalPositionY_m      = right.finalPositionY_m     ;
  initialPositionZ_m    = right.initialPositionZ_m   ;
  finalPositionZ_m      = right.finalPositionZ_m     ;
  totalTrackLength_m    = right.totalTrackLength_m   ;
  return *this;
}

Int_t grGammaTrack::operator==(const grGammaTrack& right) const
{
  return (this==&right) ? 1 : 0;
}

//==============================================================================



bool grGammaTrack::compareHits(grGammaTrack* const &a, grGammaTrack* const &b){
  Double_t aTime = a->GetTimeOfLastProcess();
  Double_t bTime = b->GetTimeOfLastProcess();
  return (aTime < bTime);
}

/*
 * grMCPTrack.cc
 *
 *  Created on: 7.19.2016
 *      Author: Ryan Schmitz
 */
#include "grMCPTrack.hh"


//==============================================================================

grMCPTrack::grMCPTrack() :
      trackID(-1),
      pdgID(0),
      initialProcessName(""),finalProcessName(""),
      initialVolumeName(""), finalVolumeName(""),
      initialTime_s(0.), finalTime_s(0.),
      initialEnergy_MeV(0.),finalEnergy_MeV(0.),
      parentID(-1),
      initialPositionX_m(0.),finalPositionX_m(0.),
      initialPositionY_m(0.),finalPositionY_m(0.),
      initialPositionZ_m(0.),finalPositionZ_m(0.),
      totalTrackLength_m(0.0) {
}


//==============================================================================
grMCPTrack::~grMCPTrack() {
}
const grMCPTrack& grMCPTrack::operator=(const grMCPTrack &right){

  trackID        = right.trackID           ;
  pdgID            = right.pdgID;
  initialProcessName = right.initialProcessName;
  finalProcessName   = right.finalProcessName  ;
  initialVolumeName  = right.initialVolumeName ;
  finalVolumeName    = right.finalVolumeName   ;
  initialTime_s      = right.initialTime_s     ;
  finalTime_s        = right.finalTime_s       ;
  initialEnergy_MeV  = right.initialEnergy_MeV ;
  finalEnergy_MeV    = right.finalEnergy_MeV   ;
  parentID           = right.parentID          ;
  initialPositionX_m = right.initialPositionX_m;
  finalPositionX_m   = right.finalPositionX_m  ;
  initialPositionY_m = right.initialPositionY_m;
  finalPositionY_m   = right.finalPositionY_m  ;
  initialPositionZ_m = right.initialPositionZ_m;
  finalPositionZ_m   = right.finalPositionZ_m  ;
  totalTrackLength_m = right.totalTrackLength_m;
  return *this;
}

Int_t grMCPTrack::operator==(const grMCPTrack& right) const
{
  return (this==&right) ? 1 : 0;
}

//==============================================================================



bool grMCPTrack::compareHits(grMCPTrack* const &a, grMCPTrack* const &b){
  Double_t aTime = a->GetTimeOfLastProcess();
  Double_t bTime = b->GetTimeOfLastProcess();
  return (aTime < bTime);
}

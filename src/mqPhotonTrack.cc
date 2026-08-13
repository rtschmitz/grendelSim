/*
 * mqPhotonTrack.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */
#include "mqPhotonTrack.hh"
#include "TObject.h"

ClassImp(mqPhotonTrack)

//==============================================================================

mqPhotonTrack::mqPhotonTrack() :
			trackID(-1),
			initialTime_s(0.), finalTime_s(0.),
			initialEnergy_eV(0.),finalEnergy_eV(0.),
			totalEnergy_eV(0.),
			parentID(-1),
			initialPositionX_m(0.),finalPositionX_m(0.),
			initialPositionY_m(0.),finalPositionY_m(0.),
			initialPositionZ_m(0.),finalPositionZ_m(0.),
			totalTrackLength_m(0.0),
			absorption(false), scintToPMT(false),
			nbOfReflections(0.), nbOfIntReflections(0.)
{

}


//==============================================================================
mqPhotonTrack::~mqPhotonTrack() {
}
const mqPhotonTrack& mqPhotonTrack::operator=(const mqPhotonTrack &right){

	trackID 		   = right.trackID           ;
	initialTime_s        = right.initialTime_s       ;
	finalTime_s          = right.finalTime_s         ;
	initialEnergy_eV      = right.initialEnergy_eV     ;
	finalEnergy_eV        = right.finalEnergy_eV       ;
	totalEnergy_eV       = right.totalEnergy_eV       ;
	parentID             = right.parentID          ;
	initialPositionX_m   = right.initialPositionX_m  ;
	finalPositionX_m     = right.finalPositionX_m    ;
	initialPositionY_m   = right.initialPositionY_m  ;
	finalPositionY_m     = right.finalPositionY_m    ;
	initialPositionZ_m   = right.initialPositionZ_m  ;
	finalPositionZ_m     = right.finalPositionZ_m   ;
	totalTrackLength_m   = right.totalTrackLength_m  ;
	nbOfReflections    = right.nbOfReflections   ;
	nbOfIntReflections    = right.nbOfIntReflections   ;
	absorption           = right.absorption      ;
	scintToPMT         = right.scintToPMT           ;

	return *this;
}

Int_t mqPhotonTrack::operator==(const mqPhotonTrack& right) const
{
  return (this==&right) ? 1 : 0;
}

//==============================================================================



bool mqPhotonTrack::compareHits(mqPhotonTrack* const &a, mqPhotonTrack* const &b){
	Double_t aTime = a->GetTimeOfLastProcess();
	Double_t bTime = b->GetTimeOfLastProcess();
	return (aTime < bTime);
}






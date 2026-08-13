/*
 * mqNeutronTrack.cc
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */
#include "mqNeutronTrack.hh"
#include "TObject.h"

ClassImp(mqNeutronTrack)

//==============================================================================

mqNeutronTrack::mqNeutronTrack() :
			trackID(-1),
			initialProcessName(""),finalProcessName(""),
			initialVolumeName(""), finalVolumeName(""),
			initialTime_s(0.), finalTime_s(0.),
			initialEnergy_MeV(0.),finalEnergy_MeV(0.),
			totalEnergy_MeV(0.),
			parentID(-1),
			initialPositionX_m(0.),finalPositionX_m(0.),
			initialPositionY_m(0.),finalPositionY_m(0.),
			initialPositionZ_m(0.),finalPositionZ_m(0.),
			totalTrackLength_m(0.0),
			nbOfElastics(-1),
			nbOfInteractions(-1),
			energyEnterScinti_MeV(0.){

}


//==============================================================================
mqNeutronTrack::~mqNeutronTrack() {
}
const mqNeutronTrack& mqNeutronTrack::operator=(const mqNeutronTrack &right){

	trackID 		   = right.trackID           ;
	initialProcessName = right.initialProcessName;
	finalProcessName   = right.finalProcessName  ;
	initialVolumeName  = right.initialVolumeName ;
	finalVolumeName    = right.finalVolumeName   ;
	initialTime_s      = right.initialTime_s     ;
	finalTime_s        = right.finalTime_s       ;
	initialEnergy_MeV  = right.initialEnergy_MeV ;
	finalEnergy_MeV    = right.finalEnergy_MeV   ;
	totalEnergy_MeV       = right.totalEnergy_MeV       ;
	parentID           = right.parentID          ;
	initialPositionX_m = right.initialPositionX_m;
	finalPositionX_m   = right.finalPositionX_m  ;
	initialPositionY_m = right.initialPositionY_m;
	finalPositionY_m   = right.finalPositionY_m  ;
	initialPositionZ_m = right.initialPositionZ_m;
	finalPositionZ_m   = right.finalPositionZ_m  ;
	totalTrackLength_m = right.totalTrackLength_m;
	nbOfElastics       = right.nbOfElastics      ;
	nbOfInteractions   = right.nbOfInteractions  ;
	energyEnterScinti_MeV  = right.energyEnterScinti_MeV ;

	return *this;
}

Int_t mqNeutronTrack::operator==(const mqNeutronTrack& right) const
{
  return (this==&right) ? 1 : 0;
}

//==============================================================================



bool mqNeutronTrack::compareHits(mqNeutronTrack* const &a, mqNeutronTrack* const &b){
	Double_t aTime = a->GetTimeOfLastProcess();
	Double_t bTime = b->GetTimeOfLastProcess();
	return (aTime < bTime);
}






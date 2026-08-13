/*
 * mqMuonTrack.cc
 *
 *  Created on: 7.19.2016
 *      Author: Ryan Schmitz
 */
#include "mqMuonTrack.hh"
#include "TObject.h"

ClassImp(mqMuonTrack)

//==============================================================================

mqMuonTrack::mqMuonTrack() :
			trackID(-1),
			initialProcessName(""),finalProcessName(""),
			initialVolumeName(""), finalVolumeName(""),
			initialTime_s(0.), finalTime_s(0.),
			initialEnergy_MeV(0.),finalEnergy_MeV(0.),
			totalEnergy_MeV(0.), energyDeposit_MeV(0.),
			parentID(-1),
			initialPositionX_m(0.),finalPositionX_m(0.),
			initialPositionY_m(0.),finalPositionY_m(0.),
			initialPositionZ_m(0.),finalPositionZ_m(0.),
			initialMomentumX_MeV(0.),finalMomentumX_MeV(0.),
			initialMomentumY_MeV(0.),finalMomentumY_MeV(0.),
			initialMomentumZ_MeV(0.),finalMomentumZ_MeV(0.),
			genPt(0.),genEta(0.),genPhi(0.),
			totalTrackLength_m(0.0),
			nbOfElastics(-1),
			nbOfInteractions(-1),
			energyDiff(0.),
			Zposition(-100),
			Xposition(-100),
			Yposition(-100),
			Zfposition(100),
			Xfposition(100),
			Yfposition(100),
			muonTrigUp(false),
			muonTrigLow(false)
			{
}


//==============================================================================
mqMuonTrack::~mqMuonTrack() {
}
const mqMuonTrack& mqMuonTrack::operator=(const mqMuonTrack &right){

	trackID 		   = right.trackID           ;
	initialProcessName = right.initialProcessName;
	finalProcessName   = right.finalProcessName  ;
	initialVolumeName  = right.initialVolumeName ;
	finalVolumeName    = right.finalVolumeName   ;
	initialTime_s      = right.initialTime_s     ;
	finalTime_s        = right.finalTime_s       ;
	initialEnergy_MeV  = right.initialEnergy_MeV ;
	finalEnergy_MeV    = right.finalEnergy_MeV   ;
	energyDeposit_MeV  = right.energyDeposit_MeV ;
	totalEnergy_MeV    = right.totalEnergy_MeV   ;
	parentID           = right.parentID          ;

	initialPositionX_m = right.initialPositionX_m;
	finalPositionX_m   = right.finalPositionX_m  ;
	initialPositionY_m = right.initialPositionY_m;
	finalPositionY_m   = right.finalPositionY_m  ;
	initialPositionZ_m = right.initialPositionZ_m;
	finalPositionZ_m   = right.finalPositionZ_m  ;

	initialMomentumX_MeV = right.initialMomentumX_MeV;
	finalMomentumX_MeV   = right.finalMomentumX_MeV  ;
	initialMomentumY_MeV = right.initialMomentumY_MeV;
	finalMomentumY_MeV   = right.finalMomentumY_MeV  ;
	initialMomentumZ_MeV = right.initialMomentumZ_MeV;
	finalMomentumZ_MeV   = right.finalMomentumZ_MeV  ;

	genPt    	   = right.genPt             ;
	genEta    	   = right.genEta            ;
	genPhi    	   = right.genPhi            ;

	totalTrackLength_m = right.totalTrackLength_m;
	nbOfElastics       = right.nbOfElastics      ;
	nbOfInteractions   = right.nbOfInteractions  ;
	energyDiff         = right.energyDiff        ;
	muonTrigUp         = right.muonTrigUp        ;
	muonTrigLow        = right.muonTrigLow       ;
	Xposition          = right.Xposition         ;
	Yposition          = right.Yposition         ;
	Zposition          = right.Zposition         ;
	Xfposition         = right.Xfposition         ;
	Yfposition         = right.Yfposition         ;
	Zfposition         = right.Zfposition         ;
	return *this;
}

Int_t mqMuonTrack::operator==(const mqMuonTrack& right) const
{
  return (this==&right) ? 1 : 0;
}

//==============================================================================



bool mqMuonTrack::compareHits(mqMuonTrack* const &a, mqMuonTrack* const &b){
	Double_t aTime = a->GetTimeOfLastProcess();
	Double_t bTime = b->GetTimeOfLastProcess();
	return (aTime < bTime);
}






/*
 * mqMuonTrack.hh
 *
 *  Created on: 7.19.2017
 *      Author: Ryan Schmitz
 */

#ifndef MQMUONTRACK_HH_
#define MQMUONTRACK_HH_

#include <iostream>
#include <vector>
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
//==============================================================================
class mqMuonTrack: public TObject {
public:
	mqMuonTrack();
	ClassDef(mqMuonTrack,9);
	~mqMuonTrack();
	 const mqMuonTrack& operator=(const mqMuonTrack &right);
     Int_t operator==(const mqMuonTrack&) const;


	void SetTrackID(Int_t tID){trackID = tID; }
	void SetFirstProcessName(std::string firstProcessName){ initialProcessName = firstProcessName; }
	void SetLastProcessName(TString lastProcessName){ finalProcessName = lastProcessName; }
	void SetFirstVolume(TString firstVolume){ initialVolumeName = firstVolume; }
	void SetLastVolume(TString lastVolume){ finalVolumeName = lastVolume; }
	void SetTimeOfFirstProcess(Double_t timeOfFirstProcess){ initialTime_s = timeOfFirstProcess; }
	void SetTimeOfLastProcess(Double_t timeOfLastProcess){ finalTime_s = timeOfLastProcess; }
	void SetInitialEnergy(Double_t firstEnergy){ initialEnergy_MeV = firstEnergy; }
	void SetFinalEnergy(Double_t lastEnergy){ finalEnergy_MeV = lastEnergy; }
	void SetEnergyDeposit(Double_t energyDeposit){ energyDeposit_MeV = energyDeposit; }
	void SetTotalEnergy(Double_t totalEnergy){totalEnergy_MeV = totalEnergy; }
	void SetParentID(Int_t pID){ parentID = pID; }
	void SetFirstPositionX(Double_t firstPositionX){ initialPositionX_m = firstPositionX; }
	void SetLastPositionX(Double_t lastPositionX) { finalPositionX_m = lastPositionX; }
	void SetFirstPositionY(Double_t firstPositionY){ initialPositionY_m = firstPositionY; }
	void SetLastPositionY(Double_t lastPositionY) { finalPositionY_m = lastPositionY; }
	void SetFirstPositionZ(Double_t firstPositionZ){ initialPositionZ_m = firstPositionZ; }
	void SetLastPositionZ(Double_t lastPositionZ){ finalPositionZ_m = lastPositionZ; }
	void SetFirstMomentumX(Double_t firstMomentumX){ initialMomentumX_MeV = firstMomentumX; }
	void SetLastMomentumX(Double_t lastMomentumX){ finalMomentumX_MeV = lastMomentumX; }
	void SetFirstMomentumY(Double_t firstMomentumY){ initialMomentumY_MeV = firstMomentumY; }
	void SetLastMomentumY(Double_t lastMomentumY){ finalMomentumY_MeV = lastMomentumY; }
	void SetFirstMomentumZ(Double_t firstMomentumZ){ initialMomentumZ_MeV = firstMomentumZ; }
	void SetLastMomentumZ(Double_t lastMomentumZ){ finalMomentumZ_MeV = lastMomentumZ; }


	void SetGenPt(Double_t genPt){ genPt = genPt; }
	void SetGenEta(Double_t genEta){ genEta = genEta; }
	void SetGenPhi(Double_t genPhi){ genPhi = genPhi; }

	void SetTotalTrackLength(Double_t totalTrackLength){ totalTrackLength_m = totalTrackLength; }
	void SetYposition(Double_t yp) {Yposition = yp;}
	void SetXposition(Double_t xp) {Xposition = xp;}
	void SetZposition(Double_t zp) {Zposition = zp;}
	void SetYfposition(Double_t yfp) {Yfposition = yfp;}
	void SetXfposition(Double_t xfp) {Xfposition = xfp;}
	void SetZfposition(Double_t zfp) {Zfposition = zfp;}

	Int_t GetTrackID()const{ return trackID; }
	TString GetFirstProcessName()const{ return initialProcessName; }
	TString GetLastProcessName()const{ return finalProcessName;}
	TString GetFirstVolume()const{ return initialVolumeName;}
	TString GetLastVolume()const{ return finalVolumeName; }
	Double_t GetTimeOfFirstProcess()const{ return initialTime_s; }
	Double_t GetTimeOfLastProcess()const{ return finalTime_s; }
	Double_t GetInitialEnergy()const{ return initialEnergy_MeV; }
	Double_t GetEnergyDeposit()const{ return energyDeposit_MeV; }
	Double_t GetFinalEnergy()const{ return finalEnergy_MeV; }
	Double_t GetTotalEnergy() const{return totalEnergy_MeV; }
	Double_t GetYposition() {return Yposition;}
	Double_t GetXposition() {return Xposition;}
	Double_t GetZposition() {return Zposition;}
	Double_t GetYfposition() {return Yfposition;}
	Double_t GetXfposition() {return Xfposition;}
	Double_t GetZfposition() {return Zfposition;}

	Int_t 	 GetParentID()const{ return parentID; }
	Double_t GetFirstPositionX()const{ return initialPositionX_m; }
	Double_t GetLastPositionX()const{ return finalPositionX_m; }
	Double_t GetFirstPositionY()const{ return initialPositionY_m; };
	Double_t GetLastPositionY()const{ return finalPositionY_m; }
	Double_t GetFirstPositionZ()const{ return initialPositionZ_m; };
	Double_t GetLastPositionZ()const{ return finalPositionZ_m; }
	Double_t GetTotalTrackLength()const{ return totalTrackLength_m; }
	Double_t GetFirstMomentumX()const{ return initialMomentumX_MeV; }
	Double_t GetLastMomentumX()const{ return finalMomentumX_MeV; }
	Double_t GetFirstMomentumY()const{ return initialMomentumY_MeV; }
	Double_t GetLastMomentumY()const{ return finalMomentumY_MeV; }
	Double_t GetFirstMomentumZ()const{ return initialMomentumZ_MeV; }
	Double_t GetLastMomentumZ()const{ return finalMomentumZ_MeV; }



	Double_t GetGenPt()const{ return genPt; }
	Double_t GetGenEta()const{ return genEta; }
	Double_t GetGenPhi()const{ return genPhi; }

        void SetFirstCopyNo(Int_t firstCopyNo){ initialCopyNo = firstCopyNo; }
        void SetLastCopyNo(Int_t lastCopyNo){ finalCopyNo = lastCopyNo; }
        Int_t   GetFirstCopyNo()const{ return initialCopyNo; }
        Int_t   GetLastCopyNo()const{ return finalCopyNo; }

	//Muon track information

	void SetNbOfElastics(Int_t elastics){nbOfElastics = elastics; }
	void SetNbOfInteractions(Int_t interactions){nbOfInteractions = interactions; }
	void AddNbOfElastics(Int_t elastics){nbOfElastics += elastics; }
	void AddNbOfInteractions(Int_t interactions){nbOfInteractions += interactions; }
	Int_t GetNbOfElastics()const{ return nbOfElastics; }
	Int_t GetNbOfInteractions()const{ return nbOfInteractions; }
	void SetEnergyDiff(Double_t EDiff){energyDiff = EDiff;}
	Double_t GetEnergyDiff()const{return energyDiff; }

	// order tracks accordong to timing information
	static bool compareHits(mqMuonTrack* const &a, mqMuonTrack* const &b);
	//friend std::ostream& operator<<(std::ostream &o,
	//		mqNeutronTrack const &track);

	void SetMuonTriggerUp(bool trig){muonTrigUp=trig;}
	void SetMuonTriggerLow(bool trig){muonTrigLow=trig;}
	bool GetMuonTriggerUp()const{ return muonTrigUp;}
	bool GetMuonTriggerLow()const{ return muonTrigLow;}

	private:

	Int_t trackID;
	std::string initialProcessName ; //Name of the first process of the given track
	TString finalProcessName   ; //Name of the last process of the given track
	TString initialVolumeName  ; //Name of the volume where the track starts
	TString finalVolumeName    ; //Name of the volume where the track ends
        Int_t initialCopyNo        ; //copyNo of initial volume
        Int_t finalCopyNo          ; //copyNo of final volume
	Double_t initialTime_s       ; //Global time when the first process occurs
 	Double_t finalTime_s         ; //Global time when the last process occurs
	Double_t initialEnergy_MeV     ; //The kinetic energy at the begin of the track
	Double_t finalEnergy_MeV       ; //The kinetic energy at the end of the track
	Double_t energyDeposit_MeV       ; //The kinetic energy at the end of the track
	Int_t    parentID          ; //The track ID of the parent track. If the track is a primary then it's zero.
	Double_t initialPositionX_m  ; //The position of the vertex of the track
	Double_t finalPositionX_m    ; //The position of the end of the track
	Double_t initialPositionY_m  ; //The position of the vertex of the track
	Double_t finalPositionY_m    ; //The position of the end of the track
	Double_t initialPositionZ_m  ; //The position of the vertex of the track
	Double_t finalPositionZ_m    ; //The position of the end of the track
	Double_t initialMomentumX_MeV    ; //The momentum of the track at the vertex
	Double_t finalMomentumX_MeV      ; //The momentum of the track at the end
	Double_t initialMomentumY_MeV    ; //The momentum of the track at the vertex
	Double_t finalMomentumY_MeV      ; //The momentum of the track at the end
	Double_t initialMomentumZ_MeV    ; //The momentum of the track at the vertex
	Double_t finalMomentumZ_MeV      ; //The momentum of the track at the end

	Double_t genPt; //The transverse momentum of the track
	Double_t genEta; //The pseudorapidity of the track
	Double_t genPhi; //The azimuthal angle of the track
	Double_t totalTrackLength_m  ; //The total track length of the track
	Double_t totalEnergy_MeV       ; //Energy loss of the track
	Double_t Xposition; //the position that muon enter  the cavern
	Double_t Yposition;
	Double_t Zposition;
	Double_t Xfposition; //the position that muon exist the Cavern
	Double_t Yfposition; 
	Double_t Zfposition;
	Int_t nbOfElastics         ; //Number of elastic scatterings of the track
	Int_t nbOfInteractions     ; //Number of interactions of the track
	Double_t energyDiff; // Muon energy difference between creation and end of track
	bool muonTrigUp;
	bool muonTrigLow;


};

typedef std::vector<mqMuonTrack*> mqMuonTrackVector;

#endif /* MQMUONTRACK_HH_ */

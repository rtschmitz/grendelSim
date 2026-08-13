/*
 * mqPhotonTrack.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQPHOTONTRACK_HH_
#define MQPHOTONTRACK_HH_

#include <iostream>
#include <vector>
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"
//==============================================================================
class mqPhotonTrack: public TObject {
public:
	 mqPhotonTrack();
	ClassDef(mqPhotonTrack,9);
	~mqPhotonTrack();
	 const mqPhotonTrack& operator=(const mqPhotonTrack &right);
     Int_t operator==(const mqPhotonTrack&) const;

    //
    // basic information about the track
    //
	void SetTrackID(Int_t tID){trackID = tID; }

	void SetTimeOfFirstProcess(Double_t timeOfFirstProcess){ initialTime_s = timeOfFirstProcess; }
	void SetTimeOfLastProcess(Double_t timeOfLastProcess){ finalTime_s = timeOfLastProcess; }
	void SetInitialEnergy(Double_t firstEnergy){ initialEnergy_eV = firstEnergy; }
	void SetFinalEnergy(Double_t lastEnergy){ finalEnergy_eV = lastEnergy; }
	void SetTotalEnergy(Double_t totalEnergy){totalEnergy_eV = totalEnergy; }
	void SetParentID(Int_t pID){ parentID = pID; }
	void SetFirstPositionX(Double_t firstPositionX){ initialPositionX_m = firstPositionX; }
	void SetFirstPositionY(Double_t firstPositionY){ initialPositionY_m = firstPositionY; }
	void SetFirstPositionZ(Double_t firstPositionZ){ initialPositionZ_m = firstPositionZ; }
	void SetLastPositionX(Double_t lastPositionX){ finalPositionX_m = lastPositionX; }
	void SetLastPositionY(Double_t lastPositionY){ finalPositionY_m = lastPositionY; }
	void SetLastPositionZ(Double_t lastPositionZ){ finalPositionZ_m = lastPositionZ; }
	void SetTotalTrackLength(Double_t totalMuTrackLength){ totalTrackLength_m = totalMuTrackLength; }
	void SetAbsorption(bool absorb){absorption=absorb;}
	
	bool GetAbsorption()const{return absorption;}
	Int_t   GetTrackID()const{ return trackID; }
	Double_t GetTimeOfFirstProcess()const{ return initialTime_s; }
	Double_t GetTimeOfLastProcess()const{ return finalTime_s; }
	Double_t GetInitialEnergy()const{ return initialEnergy_eV; }
	Double_t GetFinalEnergy()const{ return finalEnergy_eV; }
	Int_t    GetParentID()const{ return parentID; }
	Double_t GetFirstPositionX()const{ return initialPositionX_m; }
	Double_t GetLastPositionX()const{ return finalPositionX_m; }
	Double_t GetFirstPositionY()const{ return initialPositionY_m; };
	Double_t GetLastPositionY()const{ return finalPositionY_m; }
	Double_t GetFirstPositionZ()const{ return initialPositionZ_m; };
	Double_t GetLastPositionZ()const{ return finalPositionZ_m; }
	Double_t GetTotalTrackLength()const{ return totalTrackLength_m; }
	Double_t GetTotalEnergy() const{return totalEnergy_eV; }

	//Photon track specific information
	void SetNbOfReflections(Int_t reflections){nbOfReflections = reflections; }
	Int_t GetNbOfReflections()const{ return nbOfReflections; }

	void SetNbOfIntReflections (Int_t intReflections){nbOfIntReflections = intReflections;}
	Int_t GetNbOfIntReflections()const{ return nbOfIntReflections; }

	void SetScintToPMT(bool scint_to_pmt){scintToPMT = scint_to_pmt;}
	bool GetScintToPMT()const{return scintToPMT;}

	static bool compareHits(mqPhotonTrack* const &a, mqPhotonTrack* const &b);
	//friend std::ostream& operator<<(std::ostream &o,
	//		mqPhotonTrack const &track);

	private:

	Int_t trackID;
	Double_t initialTime_s       ; //Global time when the first process occurs
 	Double_t finalTime_s         ; //Global time when the last process occurs
	Double_t initialEnergy_eV     ; //The kinetic energy at the begin of the track
	Double_t finalEnergy_eV       ; //The kinetic energy at the end of the track
	Double_t totalEnergy_eV       ; //Energy loss of the track
	Int_t parentID             ; //The track ID of the parent track. If the track is a primary then it's zero.
	Double_t initialPositionX_m  ; //The position of the vertex of the track
	Double_t finalPositionX_m    ; //The position of the end of the track
	Double_t initialPositionY_m  ; //The position of the vertex of the track
	Double_t finalPositionY_m   ; //The position of the end of the track
	Double_t initialPositionZ_m  ; //The position of the vertex of the track
	Double_t finalPositionZ_m   ; //The position of the end of the track
	Double_t totalTrackLength_m  ; //The total track length of the track

	Int_t nbOfReflections      ; //Number of reflections
	Int_t nbOfIntReflections      ; //Old, legacy parameter. Use if you want to.

	bool absorption;
	bool scintToPMT;



};

typedef std::vector<mqPhotonTrack*> mqPhotonTrackVector;

#endif /* MQPHOTONTRACK_HH_ */

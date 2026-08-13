/*
 * grMCPTrack.hh
 *
 *  Created on: 7.19.2017
 *      Author: Ryan Schmitz
 */

#ifndef MQMCPTRACK_HH_
#define MQMCPTRACK_HH_

#include <iostream>
#include <vector>
#include "TString.h"
//==============================================================================
class grMCPTrack {
public:
  grMCPTrack();
  ~grMCPTrack();
   const grMCPTrack& operator=(const grMCPTrack &right);
     Int_t operator==(const grMCPTrack&) const;


  void SetTrackID(Int_t tID){trackID = tID; }
  void SetPDGID(Int_t value){ pdgID = value; }
  Int_t GetPDGID() const { return pdgID; }
  void SetFirstProcessName(std::string firstProcessName){ initialProcessName = firstProcessName; }
  void SetLastProcessName(TString lastProcessName){ finalProcessName = lastProcessName; }
  void SetFirstVolume(TString firstVolume){ initialVolumeName = firstVolume; }
  void SetLastVolume(TString lastVolume){ finalVolumeName = lastVolume; }
  void SetTimeOfFirstProcess(Double_t timeOfFirstProcess){ initialTime_s = timeOfFirstProcess; }
  void SetTimeOfLastProcess(Double_t timeOfLastProcess){ finalTime_s = timeOfLastProcess; }
  void SetInitialEnergy(Double_t firstEnergy){ initialEnergy_MeV = firstEnergy; }
  void SetFinalEnergy(Double_t lastEnergy){ finalEnergy_MeV = lastEnergy; }
  void SetParentID(Int_t pID){ parentID = pID; }
  void SetFirstPositionX(Double_t firstPositionX){ initialPositionX_m = firstPositionX; }
  void SetLastPositionX(Double_t lastPositionX) { finalPositionX_m = lastPositionX; }
  void SetFirstPositionY(Double_t firstPositionY){ initialPositionY_m = firstPositionY; }
  void SetLastPositionY(Double_t lastPositionY) { finalPositionY_m = lastPositionY; }
  void SetFirstPositionZ(Double_t firstPositionZ){ initialPositionZ_m = firstPositionZ; }
  void SetLastPositionZ(Double_t lastPositionZ){ finalPositionZ_m = lastPositionZ; }
  void SetTotalTrackLength(Double_t totalTrackLength){ totalTrackLength_m = totalTrackLength; }

  Int_t GetTrackID()const{ return trackID; }
  TString GetFirstProcessName()const{ return initialProcessName; }
  TString GetLastProcessName()const{ return finalProcessName;}
  TString GetFirstVolume()const{ return initialVolumeName;}
  TString GetLastVolume()const{ return finalVolumeName; }
  Double_t GetTimeOfFirstProcess()const{ return initialTime_s; }
  Double_t GetTimeOfLastProcess()const{ return finalTime_s; }
  Double_t GetInitialEnergy()const{ return initialEnergy_MeV; }
  Double_t GetFinalEnergy()const{ return finalEnergy_MeV; }

  Int_t    GetParentID()const{ return parentID; }
  Double_t GetFirstPositionX()const{ return initialPositionX_m; }
  Double_t GetLastPositionX()const{ return finalPositionX_m; }
  Double_t GetFirstPositionY()const{ return initialPositionY_m; };
  Double_t GetLastPositionY()const{ return finalPositionY_m; }
  Double_t GetFirstPositionZ()const{ return initialPositionZ_m; };
  Double_t GetLastPositionZ()const{ return finalPositionZ_m; }
  Double_t GetTotalTrackLength()const{ return totalTrackLength_m; }
        void SetFirstCopyNo(Int_t firstCopyNo){ initialCopyNo = firstCopyNo; }
        void SetLastCopyNo(Int_t lastCopyNo){ finalCopyNo = lastCopyNo; }
        Int_t   GetFirstCopyNo()const{ return initialCopyNo; }
        Int_t   GetLastCopyNo()const{ return finalCopyNo; }

  //MCP track information


  // order tracks accordong to timing information
  static bool compareHits(grMCPTrack* const &a, grMCPTrack* const &b);
  //friend std::ostream& operator<<(std::ostream &o,
  //    grNeutronTrack const &track);

  private:

  Int_t trackID;
  Int_t pdgID;
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
  Int_t    parentID          ; //The track ID of the parent track. If the track is a primary then it's zero.
  Double_t initialPositionX_m  ; //The position of the vertex of the track
  Double_t finalPositionX_m    ; //The position of the end of the track
  Double_t initialPositionY_m  ; //The position of the vertex of the track
  Double_t finalPositionY_m    ; //The position of the end of the track
  Double_t initialPositionZ_m  ; //The position of the vertex of the track
  Double_t finalPositionZ_m    ; //The position of the end of the track
  Double_t totalTrackLength_m  ; //The total track length of the track


};

typedef std::vector<grMCPTrack*> grMCPTrackVector;

#endif /* MQMCPTRACK_HH_ */

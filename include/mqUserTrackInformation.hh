/*
 * mqUserTrackInformation.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */
#include "G4VUserTrackInformation.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

#ifndef MQUSERTRACKINFORMATION_HH_
#define MQUSERTRACKINFORMATION_HH_


enum mqTrackStatus { active=1, hitPMT=2, absorbed=4, boundaryAbsorbed=8,
                      inactive=14};

/*mqTrackStatus:
  active: still being tracked
  hitPMT: stopped by being detected in a PMT
  absorbed: stopped by being absorbed with G4OpAbsorbtion
  boundaryAbsorbed: stopped by being aborbed with G4OpAbsorbtion
  inactive: track is stopped for some reason
   -This is the sum of all stopped flags so can be used to remove stopped flags

 */

class mqUserTrackInformation : public G4VUserTrackInformation
{
public:
  mqUserTrackInformation();
  ~mqUserTrackInformation();

  //Sets the track status to s (does not check validity of flags)
  void SetTrackStatusFlags(int s){status=s;}
  //Does a smart add of track status flags (disabling old flags that conflict)
  //If s conflicts with itself it will not be detected
  void AddTrackStatusFlag(int s);

  int GetTrackStatus()const {return status;}

  void IncReflections(){reflections++;}
  void IncInternalReflections(){intReflections++;}
  G4int GetReflectionCount()const {return reflections;}
  G4int GetInternalReflectionCount()const {return intReflections;}

  void SetForceDrawTrajectory(G4bool b){forcedraw=b;}
  G4bool GetForceDrawTrajectory(){return forcedraw;}

  void SetTrackID(G4int ID){trackID = ID;}
  G4int GetTrackID(){return trackID;}







  inline void Print()const{};


private:
  int status;

  G4int reflections;
  G4int intReflections;
  G4bool forcedraw;
  G4int trackID;


};



#endif /* MQUSERTRACKINFORMATION_HH_ */

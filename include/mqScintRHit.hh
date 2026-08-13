/*
 * mqScintRHit.hh
 *
 *  Created on: 22.04.2019
 *      Author: schmitz
 */

#ifndef MQSCINTRHIT_HH_
#define MQSCINTRHIT_HH_

#include "TObject.h"
#include "TString.h"
#include <vector>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class mqScintRHit : public TObject
{
  public:

    mqScintRHit();
    ~mqScintRHit();
    mqScintRHit(const mqScintRHit&);
    const mqScintRHit& operator=(const mqScintRHit&);
    Int_t operator==(const mqScintRHit&) const;


    void SetTrackID(Int_t track){ trackID = track; };
    Int_t    GetTrackID()const{ return trackID; };
    
    void SetParentID(Int_t pid){ parentID  = pid; };
    Int_t    GetParentID()const{ return parentID; };
    

    void SetEDep(Double_t ed){ EDep_MeV    = ed;    };
//    void AddEDep(Double_t de){ EDep_MeV   += de;    };
    Double_t GetEDep()const{ return EDep_MeV; };

//    void SetTrackLength(Double_t tl){ trackLength_cm    = tl;    };
//    Double_t GetTrackLength()const{ return trackLength_cm; };

    void SetCopyNo(Int_t n){ copyNo = n;};
    Int_t GetCopyNo()const{ return copyNo;};

    void SetHitTime(Double_t tm){ hitTime_ns    = tm;    };
    Double_t GetHitTime()const{ return hitTime_ns; };
    

    void SetExitTime(Double_t tm){ exitTime_ns    = tm;    };
    Double_t GetExitTime()const{ return exitTime_ns; };
    void SetParticleName(Int_t pn){ particleName   = pn;    };
    Int_t GetParticleName(){ return particleName; };
    void SetProcName(TString pn){ procName   = pn;    };
    TString GetProcName(){ return procName; };
    void SetCreatorVolName(TString vn){ creatorVolName   = vn;    };
    TString GetCreatorVolName(){ return creatorVolName; };


    void SetHitPositionX(Double_t x){ hitPositionX_cm     =  x;   };
    void SetHitPositionY(Double_t y){ hitPositionY_cm     =  y;   };
    void SetHitPositionZ(Double_t z){ hitPositionZ_cm     =  z;   };
    Double_t GetHitPositionX()const{ return hitPositionX_cm; };
    Double_t GetHitPositionY()const{ return hitPositionY_cm; };
    Double_t GetHitPositionZ()const{ return hitPositionZ_cm; };

    void SetExitPositionX(Double_t x){ exitPositionX_cm     =  x;   };
    void SetExitPositionY(Double_t y){ exitPositionY_cm     =  y;   };
    void SetExitPositionZ(Double_t z){ exitPositionZ_cm     =  z;   };
    Double_t GetExitPositionX()const{ return exitPositionX_cm; };
    Double_t GetExitPositionY()const{ return exitPositionY_cm; };
    Double_t GetExitPositionZ()const{ return exitPositionZ_cm; };

  private:

    Int_t    trackID;
    Int_t    parentID;
    Double_t EDep_MeV;
    Double_t trackLength_cm;
    Int_t    copyNo;
    Double_t hitTime_ns;
    Double_t exitTime_ns;
    Int_t    particleName;
    TString  procName;
    TString  creatorVolName;
    Double_t hitPositionX_cm;
    Double_t hitPositionY_cm;
    Double_t hitPositionZ_cm;
    Double_t exitPositionX_cm;
    Double_t exitPositionY_cm;
    Double_t exitPositionZ_cm;

    ClassDef(mqScintRHit,11);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
typedef std::vector<mqScintRHit*> mqScintRHitVector;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



#endif /* MQSCINTRHIT_HH_ */

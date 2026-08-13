//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file mqPMTParameterisation.hh
/// \brief Definition of the mqPMTParameterisation class

#ifndef mqPMTParameterisation_h
#define mqPMTParameterisation_h 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"

class G4VPhysicalVolume;
class G4Tubs;

// Dummy declarations to get rid of warnings ...
///*
class G4Trd;
class G4Box;
class G4Trap;
class G4Cons;
class G4Orb;
class G4Sphere;
class G4Ellipsoid;
class G4Torus;
class G4Para;
class G4Hype;
class G4Tubs;
class G4Polycone;
class G4Polyhedra;
//*/
///  A parameterisation that describes a series of boxes along Z.
///
///  The boxes have equal width, & their lengths are a linear equation.
///  They are spaced an equal distance apart, starting from given location.

///////////////////above are comments from B2bChamberParamterisation. Here, we use the parameterisation method so that we can possibly modify bars
///////////////////in general, the bars will all have the same size and properties unless otherwise noted

class mqPMTParameterisation : public G4VPVParameterisation
{ 
  public:
  
    mqPMTParameterisation(G4int    nBarX, //number of bars in layer, X axis
			      G4int nBarY, //number of bars in layer, Y axis
                              G4double startZ, // 
                              G4double XYspacing, //bar spacing
                              G4double radius, //half cross section length of bar
                              G4double halfLengthBar); //half length of bar

//dummy declarations to get rid of warnings
    void ComputeDimensions (G4Box&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trd&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Trap&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Cons&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Sphere&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Orb&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Ellipsoid&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Torus&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Para&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Hype&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polycone&,const G4int,
                            const G4VPhysicalVolume*) const {}
    void ComputeDimensions (G4Polyhedra&,const G4int,
                            const G4VPhysicalVolume*) const {}


    virtual ~mqPMTParameterisation();
   
    void ComputeTransformation (const G4int copyNo,
                                G4VPhysicalVolume* physVol) const;
    
    void ComputeDimensions (G4Tubs& trackerLayer, const G4int copyNo,
                            const G4VPhysicalVolume* physVol) const;

  private:

    G4int    fnBars;             //total number of bars (=fnBarsX^2*fnLayers)
    G4int    fnBarsX;             //number of bars in X axis
    G4int    fnBarsY;             //number of bars in Y axis
    G4double fStartZ;
    G4double fRadius;        //  The radius of the PMT
    G4double fHalfLength;        //  The half-size of PMT length
    G4double fXYSpacing;          //  The distance between the bars' center in cross section
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

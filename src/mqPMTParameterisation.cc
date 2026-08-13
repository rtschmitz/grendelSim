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
/// \file mqPMTParameterisation.cc
/// \brief Implementation of the mqPMTParameterisation class

#include "mqPMTParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"
#include <math.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqPMTParameterisation::mqPMTParameterisation(  
        G4int    nBarX,
        G4int    nBarY,
        G4double startZ,          //  Z of center of grid. Always set to zero for now
        G4double XYspacing,        //  spacing of axis centers of bars
        G4double radius,       //bars are square in cross-section, this gives length
        G4double halfLengthBar)      //bar length
 : G4VPVParameterisation()
{
   fnBars =  nBarX*nBarY;
   fnBarsX = nBarX;
   fnBarsY = nBarY;
   fStartZ     =  startZ; 
   fXYSpacing    =  XYspacing;
   fRadius  =  radius;
   fHalfLength =  halfLengthBar;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqPMTParameterisation::~mqPMTParameterisation()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqPMTParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  // Note: copyNo will start with zero!
  G4double Xposition = (copyNo % fnBarsX)*fXYSpacing - (fnBarsX-1)*fXYSpacing/2;
  G4double Yposition = (copyNo/fnBarsX)*fXYSpacing   - (fnBarsY-1)*fXYSpacing/2;
  //G4double Yposition = ((copyNo/fnBarsX) % fnBarsX)*fXYSpacing   - (fnBarsX-1)*fXYSpacing/2;
  G4int nBarsPerLayer = fnBarsX*fnBarsY;
  //G4double Zposition = fStartZ+(copyNo/nBarsPerLayer)*fZSpacing - (fnLayers-1)*fZSpacing/2;
  G4double Zposition = fStartZ;
  G4ThreeVector origin(Xposition,Yposition,Zposition);
  physVol->SetTranslation(origin);
  physVol->SetRotation(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqPMTParameterisation::ComputeDimensions
(G4Tubs& mqPMT, const G4int copyNo, const G4VPhysicalVolume*) const
{
  // Note: copyNo will start with zero! and for now, we don't change our geom size for each copy
  //mqPMT.SetInnerRadius(0);
  //mqPMT.SetOuterRadius(fRadius);
  //mqPMT.SetStartPhiAngle(0*deg);
  //mqPMT.SetDeltaPhiAngle(360*deg);
  //mqPMT.SetZHalfLength(fHalfLength);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

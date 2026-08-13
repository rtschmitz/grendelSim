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
/// \file mqBarParameterisation.cc
/// \brief Implementation of the mqBarParameterisation class

#include "mqBarParameterisation.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4SystemOfUnits.hh"
#include <math.h>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqBarParameterisation::mqBarParameterisation(  
        G4int    nBarX,
        G4int    nBarY,
        G4double startZ,          //  Z of center of grid. Always set to zero for now
        G4double angleDelta,        //  angular separation between bars
	G4double sourceDist,      //distance to interaction point
	G4double halfWidthBar,       //bars are square in cross-section, this gives length
        G4double halfLengthBar)      //bar length
 : G4VPVParameterisation()
{
   fnBars =  nBarX*nBarY;
   fnBarsX = nBarX;
   fnBarsY = nBarY;
   fStartZ     =  startZ; 
   fAngleDelta    =  angleDelta;
   fSourceDist = sourceDist;
   fHalfWidth  =  halfWidthBar; //barSpacingXY
   fHalfLength =  halfLengthBar; //rotAng
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mqBarParameterisation::~mqBarParameterisation()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqBarParameterisation::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{

	// Note: copyNo will start with zero!
  double fXSpacing = 16.4*cm; //from Sam's model
  double fYSpacing = 13.7*cm; //from Sam's model
  //calculate x,y pos
  G4double Xposition = ((copyNo % fnBarsX) % fnBarsX)*fXSpacing   - (fnBarsX-1)*fXSpacing/2;
  G4double Yposition = ((copyNo / fnBarsX) % fnBarsX)*fYSpacing   - (fnBarsX-1)*fYSpacing/2;
  G4double Zposition = fStartZ;//+(copyNo % fnBarsX)*barSpacingXY/std::tan(rotAng); // gives offset look

//  G4int nBarsPerLayer = fnBarsX*fnBarsY;

  G4ThreeVector origin(Xposition,Yposition,Zposition);
  physVol->SetTranslation(origin);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mqBarParameterisation::ComputeDimensions
(G4Box& mqBar, const G4int copyNo, const G4VPhysicalVolume*) const
{
  // Note: copyNo will start with zero! and for now, we don't change our geom size for each copy
  //mqBar.SetXHalfLength(fHalfWidth);
  //mqBar.SetYHalfLength(fHalfWidth);
  //mqBar.SetZHalfLength(fHalfLength);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

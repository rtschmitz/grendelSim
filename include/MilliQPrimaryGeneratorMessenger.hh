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
/// \file field/field03/include/MilliQPrimaryGeneratorMessenger.hh
/// \brief Definition of the MilliQPrimaryGeneratorMessenger class
//
//
// $Id: MilliQPrimaryGeneratorMessenger.hh 76602 2013-11-13 08:33:35Z gcosmo $
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef MilliQPrimaryGeneratorMessenger_h
#define MilliQPrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"

class MilliQPrimaryGeneratorAction;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MilliQPrimaryGeneratorMessenger: public G4UImessenger
{
public:
  MilliQPrimaryGeneratorMessenger(MilliQPrimaryGeneratorAction*);
  virtual ~MilliQPrimaryGeneratorMessenger();
 
  virtual void SetNewValue(G4UIcommand*, G4String);

private:
  MilliQPrimaryGeneratorAction* fAction;
  G4UIcmdWithAString*        fRndmCmd;
  G4UIcmdWithADoubleAndUnit* fSetXVertexCmd;
  G4UIcmdWithADoubleAndUnit* fSetYVertexCmd;
  G4UIcmdWithADoubleAndUnit* fSetZVertexCmd;
  G4UIcmdWithADoubleAndUnit* fSetMomentumXVertexCmd;
  G4UIcmdWithADoubleAndUnit* fSetMomentumYVertexCmd;
  G4UIcmdWithADoubleAndUnit* fSetMomentumZVertexCmd;
  G4UIcmdWithADoubleAndUnit* fSetCalibEnergyCmd;
};

#endif

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
/// \file exoticphysics/monopole/include/MilliQMonopoleFieldMessenger.hh
/// \brief Definition of the MilliQMonopoleFieldMessenger class
//
// $Id: MilliQMonopoleFieldMessenger.hh 68036 2013-03-13 14:13:45Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// =======================================================================
// Created:  15 Jun 2010, B. Bozsogi
// =======================================================================

#ifndef MilliQMonopoleFieldMessenger_h
#define MilliQMonopoleFieldMessenger_h

#include "globals.hh"
#include "G4UImessenger.hh"

class MilliQMonopoleFieldSetup;
class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class MilliQMonopoleFieldMessenger: public G4UImessenger
{
public:
  
  MilliQMonopoleFieldMessenger(MilliQMonopoleFieldSetup* );
  ~MilliQMonopoleFieldMessenger();
    
  virtual void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  MilliQMonopoleFieldSetup*  fField;
    
  G4UIdirectory*             fFieldDir;
  G4UIcmdWithADoubleAndUnit* fSetFieldCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


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
/// \file exoticphysics/monopole/include/grMonopolePhysics.hh
/// \brief Definition of the grMonopolePhysics class
//
// $Id: grMonopolePhysics.hh 68036 2013-03-13 14:13:45Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef grMonopolePhysics_h
#define grMonopolePhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class grMonopolePhysicsMessenger;
class grMonopole;

class grMonopolePhysics : public G4VPhysicsConstructor
{
public:

  grMonopolePhysics(const boost::property_tree::ptree pt, const G4String& nam = "Monopole Physics");

  ~grMonopolePhysics();

  // This method is dummy for physics
  virtual void ConstructParticle();

  // This method will be invoked in the Construct() method.
  // each physics process will be instantiated and
  // registered to the process manager of each particle type
  virtual void ConstructProcess();

  void SetMagneticCharge(G4double);
  void SetElectricCharge(G4double);
  void SetMonopoleMass(G4double);

  G4double GetMagQ() {return fMagCharge;}
  G4double GetElQ() {return fElCharge;}
  G4double GetMass() {return fMonopoleMass;}

private:

  // hide assignment operator
  grMonopolePhysics & operator=(const grMonopolePhysics &right);
  grMonopolePhysics(const grMonopolePhysics&);

  G4double    fMagCharge;
  G4double    fElCharge;
  G4double    fMonopoleMass;

  // Configuration
  const boost::property_tree::ptree fPTree;
  boost::property_tree::ptree fParticlePTree;


  grMonopolePhysicsMessenger*  fMessenger;
  grMonopole* fMpl;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

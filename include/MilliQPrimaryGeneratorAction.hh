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
// $Id: MilliQPrimaryGeneratorAction.hh 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file MilliQPrimaryGeneratorAction.hh
/// \brief Definition of the MilliQPrimaryGeneratorAction class

#ifndef MilliQPrimaryGeneratorAction_h
#define MilliQPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
//#include "G4LorentzVector.hh"
//#include "fstream"
//#include <list>
//#include "iostream"
//#include "sstream"
#include "vector"
#include "globals.hh"
#include "G4ParticleGun.hh"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


class G4ParticleGun;
class G4Event;
class MilliQPrimaryGeneratorMessenger;

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the Tracker
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class
/// (see the macros provided with this example).

class MilliQPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  MilliQPrimaryGeneratorAction(const boost::property_tree::ptree pt, G4int eventOffset);
  virtual ~MilliQPrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* );
  void GetLHEFourVectors();
  void SetRndmFlag(G4String val) { fRndmFlag = val; }
  void SetXVertex(G4double x);
  void SetYVertex(G4double y);
  void SetZVertex(G4double z);
  void SetMomentumXVertex(G4double x);
  void SetMomentumYVertex(G4double y);
  void SetMomentumZVertex(G4double z);
  void SetCalibEnergy(G4double e);

  static G4String GetPrimaryName();

  G4ParticleGun* GetParticleGun() {return fParticleGun;}

  // Set methods
  void SetRandomFlag(G4bool );
  static G4int neventLHE;
  static G4bool firstPass;
  static std::vector<std::vector<G4double> >	vertexList;
  static std::vector<std::vector<G4double> > momentumList;
  static std::vector<std::vector<G4double> > qmeList;
  static std::vector<G4double> eventWeight;


private:
  G4ParticleGun*          fParticleGun; // G4 particle gun

  MilliQPrimaryGeneratorMessenger* fGunMessenger; //messenger of this class
  G4String                      fRndmFlag;     //flag for random impact point


  static G4ParticleDefinition* fgPrimaryParticle;
  G4double fMomentumXVertex, fMomentumYVertex, fMomentumZVertex;
  G4double fXVertex, fYVertex, fZVertex, fEnergy;
  G4bool fVertexDefined, fCalibDefined;

  G4double yRescale;
  G4double zRescale;

  G4int fEventOffset;

  G4String fPathname;
  G4String fFilename;
  const boost::property_tree::ptree fPTree;
  boost::property_tree::ptree fParticlePTree;





};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

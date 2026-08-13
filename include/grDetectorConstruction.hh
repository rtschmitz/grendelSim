#ifndef grDetectorConstruction_h
#define grDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicsVector.hh"
#include "G4Material.hh"
#include "G4Box.hh"

#include "grMonopoleFieldSetup.hh"

class grDetectorConstruction: public G4VUserDetectorConstruction {
public:
  grDetectorConstruction();
  ~grDetectorConstruction();

  G4VPhysicalVolume* Construct();
  G4double GetWorldX();
  G4double GetWorldY();
  G4double GetWorldZ();

  void SetNBarPerLayer(G4int nB){fNBar=nB;}
  void SetNLayer(G4int nL){fNLayer=nL;}
  G4int GetNBarPerLayer()const{return fNBar;}
  G4int GetNLayer()const{return fNLayer;}
  void UpdateGeometry();
  G4bool GetUpdated(){return updated;}
  G4PhysicsVector GetPMTEff_R878();
  G4PhysicsVector GetPMTEff_R7725();
  G4PhysicsVector GetPMTEff_ET9814B();
        void SetMagField(G4double, G4double, G4double);
  void SetDefaults();

private:
  grDetectorConstruction & operator=(const grDetectorConstruction &right);
  grDetectorConstruction(const grDetectorConstruction&);

  grMonopoleFieldSetup* fMonFieldSetup;

  G4MaterialPropertiesTable *SetOpticalPropertiesOfPS();
  G4MaterialPropertiesTable *SetOpticalPropertiesOfPMT();
  G4VPhysicalVolume* SetupGeometry();
  G4bool updated;
  G4double refl;
  G4int fNBar;
  G4int fNLayer;
  G4int verbose; //Manages the level of information which is printed/logged
  //verbose>=0: Print global information
  //verbose>=1: Print detailed global information
  //verbose>=2: Print information about each module
  //verbose>=3: Print detailed information about each module


  //G4VPhysicalVolume* ConstructVoid(G4VPhysicalVolume* aVolume,
  //    G4LogicalVolume* lWorld, G4ThreeVector Ts);


  //G4Material* worldMaterial;

  //G4Box* solidWorld; // pointer to the solid envelope
  //G4LogicalVolume* logicWorld; // pointer to the logical envelope
  //G4VPhysicalVolume* physicWorld; // pointer to the physical envelope

  //G4double fWorld_x; // world volume
  //G4double fWorld_y;
  //G4double fWorld_z;



};

#endif

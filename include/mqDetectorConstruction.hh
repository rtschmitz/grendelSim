#ifndef mqDetectorConstruction_h
#define mqDetectorConstruction_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PhysicsVector.hh"
#include "G4Material.hh"
#include "G4Box.hh"

#include "MilliQMonopoleFieldSetup.hh"

class mqDetectorConstruction: public G4VUserDetectorConstruction {
public:
	mqDetectorConstruction();
	~mqDetectorConstruction();

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
	void GetPMTEff_R878(G4PhysicsVector& effVec);
	void GetPMTEff_R7725(G4PhysicsVector& effVec);
	void GetPMTEff_ET9814B(G4PhysicsVector& effVec);
        void SetMagField(G4double, G4double, G4double);
	void SetDefaults();

private:
	mqDetectorConstruction & operator=(const mqDetectorConstruction &right);
	mqDetectorConstruction(const mqDetectorConstruction&);

	MilliQMonopoleFieldSetup* fMonFieldSetup;

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
	//		G4LogicalVolume* lWorld, G4ThreeVector Ts);


	//G4Material* worldMaterial;

	//G4Box* solidWorld; // pointer to the solid envelope
	//G4LogicalVolume* logicWorld; // pointer to the logical envelope
	//G4VPhysicalVolume* physicWorld; // pointer to the physical envelope

	//G4double fWorld_x; // world volume
	//G4double fWorld_y;
	//G4double fWorld_z;



};

#endif

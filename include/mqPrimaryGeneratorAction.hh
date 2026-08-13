#ifndef mqPrimaryGeneratorAction_h
#define mqPrimaryGeneratorAction_h 1

#include "globals.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include <vector>

class mqDetectorConstruction;

class mqPrimaryGeneratorActionMessenger;

class G4ParticleGun;
class G4GeneralParticleSource;
class mqVMuonSource;

class G4ParticleTable;

class mqPrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction {
public:
	mqPrimaryGeneratorAction(mqDetectorConstruction* detector);
	~mqPrimaryGeneratorAction();

	void GeneratePrimaries(G4Event*);
	void UseGeantinos(G4bool myGeantinoStatus);


	void SetSeeds(const unsigned long seed0, const unsigned long seed1,
			const unsigned long seed2);


private:
	void InitPGA();
	mqDetectorConstruction* myDetector;
	G4ParticleGun* myParticleGun;
	G4GeneralParticleSource* myGeneralParticleSource;
	mqPrimaryGeneratorActionMessenger* myMessenger;
	//mqAnalysisManager *myAnalysisManager;
	//Verbosity 0: Prints only a summary of the properties
	//Verbosity 1: Prints also every setting of the paramters
	//Verbosity 2: Prints progress information during the creation of the look-up table, every 10%
	//Verbosity 4: Prints each single entry of the look-up table (very slow!)
	G4int verbosity;
	G4String MsgPrefix;

	G4int sourceID;
	G4bool useGeantinos;

	G4ParticleTable *myParticleTable;

	std::vector<unsigned long> Seeds;
	bool UseDefaultSeeds;

	//void StoreSeeds();
	void ChangeSeeds();
};

#endif


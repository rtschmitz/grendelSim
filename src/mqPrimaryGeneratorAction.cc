#include "mqPrimaryGeneratorAction.hh"
#include "mqDetectorConstruction.hh"
#include "mqPrimaryGeneratorActionMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4UnitsTable.hh"
// Includes Physical Constants and System of Units
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

mqPrimaryGeneratorAction::mqPrimaryGeneratorAction(mqDetectorConstruction* myDC) :
  	  myDetector(myDC), myParticleGun(NULL), myGeneralParticleSource(NULL),
  	  verbosity(1), MsgPrefix(""), sourceID(0),
  	  useGeantinos(false), myParticleTable(NULL),
  	  UseDefaultSeeds(true){

    MsgPrefix = "mqPrimaryGeneratorAction> ";
    myMessenger = new mqPrimaryGeneratorActionMessenger(this);
    this->InitPGA();
    myParticleTable = G4ParticleTable::GetParticleTable();
}

mqPrimaryGeneratorAction::~mqPrimaryGeneratorAction() {
	if (myGeneralParticleSource != NULL) {
		delete myGeneralParticleSource;
		myGeneralParticleSource = NULL;
	}

	if (myParticleGun != NULL) {
		delete myParticleGun;
		myParticleGun = NULL;
	}
	delete myMessenger;
}



void mqPrimaryGeneratorAction::UseGeantinos(G4bool myGeantinoStatus){
  useGeantinos = myGeantinoStatus;
  if(verbosity>=1){
    G4cout << MsgPrefix << "Use geantino as primary particle: " << useGeantinos << G4endl;
  }
}



void mqPrimaryGeneratorAction::InitPGA() {


	  G4cout << MsgPrefix << "Use General Particle Source." << G4endl;
	  myGeneralParticleSource = new G4GeneralParticleSource();
	  myParticleGun = new G4ParticleGun();

}
//generate neutrons here
void mqPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	ChangeSeeds();
	//StoreSeeds();
	//generate initial vertex
	myGeneralParticleSource->GeneratePrimaryVertex(anEvent);

///*	
	//initialize
	G4ThreeVector shiftDirection, shiftVector;

	//get its direction and orthogonal vector
	G4ThreeVector momentumDirection = myGeneralParticleSource->GetParticleMomentumDirection().unit();
	//G4ThreeVector momentumDirection = startDir;//myGeneralParticleSource->GetParticleMomentumDirection().unit();
	G4ThreeVector showerPlane = momentumDirection.orthogonal().unit();
	
	//gets its position and the width of the circle we want to locate this other vector in
	G4ThreeVector genPos = myGeneralParticleSource->GetParticlePosition();
	//G4ThreeVector genPos = startPosition;//myGeneralParticleSource->GetParticlePosition();
	G4double circleWidth = 247*cm;	

	//set momenta and particle definition to match the generated one
	myParticleGun->SetParticleMomentumDirection(myGeneralParticleSource->GetParticleMomentumDirection());
	myParticleGun->SetParticleEnergy(myGeneralParticleSource->GetParticleEnergy());
	myParticleGun->SetParticleDefinition(myGeneralParticleSource->GetParticleDefinition());

	//check if we have a shower muon
	//G4double u = G4UniformRand();	
	G4int numParticles=0; //EXTRA PARTICLES. First one already generated above
	/*
	if(u<0.835245) numParticles=0;
	else if(u<0.972856) numParticles=1;
	else if(u<0.995528) numParticles=2;
	else if(u<0.999263) numParticles=3;
	else if(u<0.999878) numParticles=4;
	else if(u<0.999979) numParticles=5;
	else if(u<0.999996) numParticles=6;
	else if(u<0.999999) numParticles=7;
	else {numParticles=8; G4double k = G4UniformRand(); while(k<0.166) {numParticles++; k=G4UniformRand();}}
	//using multiplicity distribution from JCAP01(2016)032 with a simple extension to the tail for completeness
	//based on e^-x/0.55
	*/
	for(G4int i=0;i<numParticles;i++){
//		numParticles++;
//		myParticleGun->SetNumberOfParticles(numParticles);	
		//if we do, rotate the orthogonal vector randomly through 2 pi (random angle)
		shiftDirection = showerPlane.rotate(G4UniformRand()*2*pi*radian,momentumDirection);

		//then, randomly pick a number between 0 and R and place the shower primary there
		shiftVector = circleWidth*G4UniformRand()*shiftDirection;

		//set position and generate event
		myParticleGun->SetParticlePosition(genPos+shiftVector);
		myParticleGun->GeneratePrimaryVertex(anEvent);
	}
//*/
}

/*void mqPrimaryGeneratorAction::StoreSeeds(){ //TODO store seeds vie recorder
	std::vector<unsigned long> v;
	v = G4Random::getTheEngine()->put();
	myAnalysisManager->StoreRandomGeneratorState(v);
	//CLHEP::HepRandom::showEngineStatus();
}*/

void mqPrimaryGeneratorAction::ChangeSeeds(){
	//Restore seeds
	if(!UseDefaultSeeds){
		G4cout << "/MilliQan> - BeginOfEventAction: Change random number generator seeds from :" << G4endl;
		G4Random::showEngineStatus();
		G4cout << "              to:" << G4endl;
		G4Random::getTheEngine()->get(Seeds);
		G4Random::showEngineStatus();
		UseDefaultSeeds = true;
		Seeds.clear();
	}
}

void mqPrimaryGeneratorAction::SetSeeds(const unsigned long seed0,
		const unsigned long seed1, const unsigned long seed2) {
	Seeds.push_back(1878463799);//ID to identify the used random number generator, here it is the RanecuEngine (see MilliQan.cc:28)
	Seeds.push_back(static_cast<unsigned long>(seed0));
	Seeds.push_back(static_cast<unsigned long>(seed1));
	Seeds.push_back(static_cast<unsigned long>(seed2));
	UseDefaultSeeds = false;
	G4cout << "MilliQan/RunAction> SetSeeds" << G4endl;
	for(size_t i = 0; i < Seeds.size(); i++){
		G4cout << " Seeds[" << i << "]: " << Seeds[i] << G4endl;
	}
}


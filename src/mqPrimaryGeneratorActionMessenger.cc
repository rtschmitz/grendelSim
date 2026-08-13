#include "mqPrimaryGeneratorActionMessenger.hh"

#include "mqPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "globals.hh"

mqPrimaryGeneratorActionMessenger::mqPrimaryGeneratorActionMessenger(
		mqPrimaryGeneratorAction* PGA) :
		myPGA(PGA), Seed0(0), Seed1(0), Seed2(0) {

	G4String strPGA = "/MilliQan/PGA/";
	dirPGA = new G4UIdirectory(strPGA);
	dirPGA->SetGuidance("UI commands specific to the primary particle action.");




	G4String strUseGeantinos = strPGA;
	strUseGeantinos.append("UseGeantinos");
	cmdUseGeantinos = new G4UIcmdWithABool(strUseGeantinos, this);
	cmdUseGeantinos->SetGuidance("Use Geantinos as primary particles.");
	cmdUseGeantinos->SetGuidance(
			"This command doesn't affect the general particle source.");
	cmdUseGeantinos->SetGuidance("Default value is 'false'.");
	cmdUseGeantinos->SetDefaultValue(false);
	cmdUseGeantinos->AvailableForStates(G4State_PreInit, G4State_Idle);



	//Macro command to set the verbosity level of neutron counter related information
	G4String strSeed0 = strPGA;
	strSeed0.append("Seed0");
	cmdSeed0 = new G4UIcmdWithAnInteger(strSeed0, this);
	cmdSeed0->SetGuidance("Set the seed[0], must be positive.");
	cmdSeed0->SetParameterName("Seed0", false); //Parameter not omittable.
	cmdSeed0->SetRange("Seed0>0");
	cmdSeed0->AvailableForStates(G4State_Idle, G4State_PreInit);

	G4String strSeed1 = strPGA;
	strSeed1.append("Seed1");
	cmdSeed1 = new G4UIcmdWithAnInteger(strSeed1, this);
	cmdSeed1->SetGuidance("Set the seed[1], must be positive.");
	cmdSeed1->SetParameterName("Seed1", false);
	cmdSeed1->SetRange("Seed1>0");
	cmdSeed1->AvailableForStates(G4State_Idle, G4State_PreInit);

	G4String strSeed2 = strPGA;
	strSeed2.append("Seed2");
	cmdSeed2 = new G4UIcmdWithAnInteger(strSeed2, this);
	cmdSeed2->SetGuidance("Set the seed[2], must be positive.");
	cmdSeed2->SetParameterName("Seed2", false);
	cmdSeed2->SetRange("Seed2>0");
	cmdSeed2->AvailableForStates(G4State_Idle, G4State_PreInit);

	G4String strSetSeeds = strPGA;
	strSetSeeds.append("SetSeeds");
	cmdSetSeeds = new G4UIcmdWithoutParameter(strSetSeeds, this);
	cmdSetSeeds->SetGuidance(
			"Set the seeds (previously passed on as seed[0], seed[1], seed[2]).");
	cmdSetSeeds->AvailableForStates(G4State_Idle, G4State_PreInit);
}

mqPrimaryGeneratorActionMessenger::~mqPrimaryGeneratorActionMessenger() {

	delete cmdUseGeantinos;
	delete cmdSeed0;
	delete cmdSeed1;
	delete cmdSeed2;
	delete cmdSetSeeds;
	delete dirPGA;
}

void mqPrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command,
		G4String newValue) {

	if (command == cmdUseGeantinos) {
		myPGA->UseGeantinos(cmdUseGeantinos->GetNewBoolValue(newValue));
	}
	if (command == cmdSeed0) {
		Seed0 = cmdSeed0->GetNewIntValue(newValue);
	}
	if (command == cmdSeed1) {
		Seed1 = cmdSeed1->GetNewIntValue(newValue);
	}
	if (command == cmdSeed2) {
		Seed2 = cmdSeed2->GetNewIntValue(newValue);
	}
	if (command == cmdSetSeeds) {
		myPGA->SetSeeds(Seed0, Seed1, Seed2);
	}
}

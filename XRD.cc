#include "mqDetectorConstruction.hh"
#include "mqPrimaryGeneratorAction.hh"
#include "mqRunAction.hh"
#include "mqEventAction.hh"
#include "mqSteppingAction.hh"
#include "mqSteppingVerbose.hh"
#include "mqTrackingAction.hh"
#include "mqStackingAction.hh"
#include "mqSession.hh"
#include "mqHistoManager.hh"
//#include "mqMuonTrack.hh"

#include "mqShieldingPhysicsList.hh"
//#include "ShieldingPhysicsList.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "Randomize.hh"



//#include "QGSP_BERT_HP.hh"

/*Grafikausgabe----------------------------------------------------------------*/
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

int main(int argc, char** argv) {

	/*Randomizer for primary particle--------------------------------------*/
	//CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
	CLHEP::RanluxEngine defaultEngine( 1234567, 4 );
	G4Random::setTheEngine( &defaultEngine );
	/*User Verbose output class--------------------------------------------*/
	G4VSteppingVerbose* verbosity = new mqSteppingVerbose;
	G4VSteppingVerbose::SetInstance(verbosity);

	/*Run manager----------------------------------------------------------*/
	G4RunManager* runManager = new G4RunManager;

	/*User Initialization classes (mandatory)------------------------------*/
	mqDetectorConstruction* detector = new mqDetectorConstruction;

	runManager->SetUserInitialization(detector);

	/*User Physics List----------------------------------------------------*/

	runManager->SetUserInitialization(new mqShieldingList(0, "HP"));
	//runManager->SetUserInitialization(new ShieldingPhysicsList(0, "HP"));
	/*Visualization, if you choose to have it!-----------------------------*/
#ifdef G4VIS_USE

	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
#endif

	/*Analysis manager-----------------------------------------------------*/
	//mqOutputFile      *outputFile      = new mqOutputFile();

	//RecorderBase* recorder = NULL;//No recording is done in this example

	// set an HistoManager
	//
	mqHistoManager*  histo = new mqHistoManager();

	/*User Action classes--------------------------------------------------*/
	runManager->SetUserAction(new mqRunAction(histo));
	runManager->SetUserAction(new mqEventAction(histo));
	runManager->SetUserAction(new mqSteppingAction(histo));
	runManager->SetUserAction(new mqPrimaryGeneratorAction(detector));
	runManager->SetUserAction(new mqTrackingAction(histo));
	runManager->SetUserAction(new mqStackingAction);
	/*Initialize G4 kernel-------------------------------------------------*/
	//runManager->Initialize(); //HERE

	/*Get the pointer to the User Interface manager------------------------*/
	G4UImanager * UI = G4UImanager::GetUIpointer();
	mqSession * LoggedSession = new mqSession();
	UI->SetCoutDestination(LoggedSession);
	if (argc == 1) {
		G4cout << "BeamRad> Enter interactive mode." << G4endl;
		// Define (G)UI terminal for interactive mode
		// G4UIterminal is a (dumb) terminal
		G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
		session = new G4UIterminal(new G4UItcsh);
#else
		session = new G4UIterminal();
#endif
		// 		UI->ApplyCommand("/control/execute run25.mac");
		session->SessionStart();
		delete session;
	}
	//Full batch mode
	if (argc == 2) {
		G4cout << "BeamRad> Enter full batch mode" << G4endl;
		//Full batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		G4cout << "apply mac file " << command + fileName << G4endl;
		UI->ApplyCommand(command + fileName);
	}
	//Run macro file and then switch to interactive mode
	if (argc == 3) {
		G4cout << "BeamRad> Enter batch mode." << G4endl;
		if ((strcmp(argv[2], "-GUI") == 0) || (strcmp(argv[1], "-GUI") == 0)) {
			//Graphical batch mode
			G4UIsession * session = 0;
#ifdef G4UI_USE_TCSH
			session = new G4UIterminal(new G4UItcsh);
#else
			session = new G4UIterminal();
#endif
			G4String command = "/control/execute ";
			G4String fileName = "";
			//if GUI is the 2nd argument then the filename is the 1st ...
			if (strcmp(argv[2], "-GUI") == 0) {
				fileName = argv[1];
			}
			//... and vice versa
			if (strcmp(argv[1], "-GUI") == 0) {
				fileName = argv[2];
			}
			UI->ApplyCommand(command + fileName);
			session->SessionStart();
			delete session;
		}
	}


	/*Free the store-------------------------------------------------------*/
	//user actions, physics_list and detector_description are
	//owned and deleted by the run manager, so they should not
	//be deleted in the main() program !

#ifdef G4VIS_USE
	delete visManager;
#endif

	delete runManager;
	delete verbosity;
	delete LoggedSession;
	delete histo;
	//delete outputFile;
	return 0;
}

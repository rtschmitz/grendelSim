#include "mqDetectorConstruction.hh"
#include "mqPrimaryGeneratorAction.hh"
#include "MilliQPrimaryGeneratorAction.hh"
#include "mqRunAction.hh"
#include "mqEventAction.hh"
#include "mqSteppingAction.hh"
#include "mqSteppingVerbose.hh"
#include "mqTrackingAction.hh"
#include "mqStackingAction.hh"
#include "mqSession.hh"
#include "mqHistoManager.hh"

#include "mqShieldingPhysicsList.hh"
//#include "MilliQPhysicsList.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "Randomize.hh"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>



//#include "QGSP_BERT_HP.hh"

/*Grafikausgabe----------------------------------------------------------------*/
#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

int main(int argc, char** argv) {

  	boost::property_tree::ptree pt;
        boost::property_tree::ptree particlePTree;
	std::string configFile = "config/onepc.ini";

	  try {
	    boost::property_tree::ini_parser::read_ini(configFile, pt); // std::string, ptree
	  }
	  catch(boost::property_tree::ptree_error &e) {
	    G4ExceptionDescription msg;
	    msg << G4endl << "Configuration file " << e.what() << G4endl;
	    G4Exception("MilliQan::main()", "MilliQan::ConfigFileReadError", FatalException, msg);
	  }

	  try {
	    boost::property_tree::ini_parser::read_ini(pt.get<std::string>("Configuration.ParticleConfigFile"), particlePTree);
	  }
	  catch(boost::property_tree::ptree_error &e) {
	    G4ExceptionDescription msg;
	    msg << G4endl << "Configuration file " << e.what() << G4endl;
	    G4Exception("MilliQan::main()", "MilliQan::ConfigFileReadError", FatalException, msg);
  	  }

	G4int jobNumber = particlePTree.get<G4int>("ParticleProperties.JobNumber");
	G4int eventsPerJob = particlePTree.get<G4int>("ParticleProperties.EventsPerJob");
	G4double eventWeight = particlePTree.get<G4double>("ParticleProperties.EventWeight");
	G4int processID = particlePTree.get<G4int>("ParticleProperties.ProcessID");


	G4int eventOffset = jobNumber*eventsPerJob;
	
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

	//runManager->SetUserInitialization(new MilliQPhysicsList(pt));
	runManager->SetUserInitialization(new mqShieldingList(0, "HP", pt));
//	runManager->SetUserInitialization(new mqShieldingList(0, "HP", pt));
	
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
	runManager->SetUserAction(new mqEventAction(histo,eventOffset,eventWeight,processID));
	runManager->SetUserAction(new mqSteppingAction(histo));
	
	//CHANGE HERE to switch between no fourvectors and /gps/ (mqPrimGen) and loading in fourvectors and /gun/ (MilliQPrimaryGen)
     runManager->SetUserAction(new mqPrimaryGeneratorAction(detector));
   //runManager->SetUserAction(new MilliQPrimaryGeneratorAction(pt,eventOffset));
	
	runManager->SetUserAction(new mqTrackingAction(histo));
	runManager->SetUserAction(new mqStackingAction);
	/*Initialize G4 kernel-------------------------------------------------*/
	//runManager->Initialize(); //HERE

	/*Get the pointer to the User Interface manager------------------------*/
	G4UImanager * UI = G4UImanager::GetUIpointer();
	mqSession * LoggedSession = new mqSession();
	UI->SetCoutDestination(LoggedSession);
	if (argc == 1) {
		G4cout << "MilliQan> Enter interactive mode." << G4endl;
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
		G4cout << "MilliQan> Enter full batch mode" << G4endl;
		//Full batch mode
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		G4cout << "apply mac file " << command + fileName << G4endl;
		UI->ApplyCommand(command + fileName);
	}
	//Run macro file and then switch to interactive mode
	if (argc == 3) {
		G4cout << "MilliQan> Enter batch mode." << G4endl;
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

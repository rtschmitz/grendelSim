#include "grDetectorConstruction.hh"
#include "grEventAction.hh"
#include "grFilePrimaryGeneratorAction.hh"
#include "grHistoManager.hh"
#include "grPrimaryGeneratorAction.hh"
#include "grRunAction.hh"
#include "grSession.hh"
#include "grShieldingPhysicsList.hh"
#include "grStackingAction.hh"
#include "grSteppingAction.hh"
#include "grSteppingVerbose.hh"
#include "grTrackingAction.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#ifdef G4UI_USE
#include "G4UIterminal.hh"
#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif
#endif
#include "G4VisExecutive.hh"
#include "Randomize.hh"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>

namespace {
constexpr const char* kConfigFile = "config/onepc.ini";
void printUsage(const char* program) {
  std::cerr << "Usage: " << program << " [<cosmic|beam> <macro-file>]\n";
}
}

int main(int argc, char** argv) {
  if (argc != 1 && argc != 3) { printUsage(argv[0]); return 2; }
  const bool interactive = argc == 1;
  const std::string workflow = interactive ? "cosmic" : argv[1];
  if (workflow != "cosmic" && workflow != "beam") { printUsage(argv[0]); return 2; }

  boost::property_tree::ptree configuration;
  boost::property_tree::ptree particles;
  try {
    boost::property_tree::ini_parser::read_ini(kConfigFile, configuration);
    boost::property_tree::ini_parser::read_ini(
        configuration.get<std::string>("Configuration.ParticleConfigFile"), particles);
  } catch (const boost::property_tree::ptree_error& error) {
    G4ExceptionDescription message;
    message << G4endl << "Configuration file " << error.what() << G4endl;
    G4Exception("grendelSim::main()", "grendelSim::ConfigFileReadError", FatalException, message);
  }

  const G4int jobNumber = particles.get<G4int>("ParticleProperties.JobNumber");
  const G4int eventsPerJob = particles.get<G4int>("ParticleProperties.EventsPerJob");
  const G4double eventWeight = particles.get<G4double>("ParticleProperties.EventWeight");
  const G4int processID = particles.get<G4int>("ParticleProperties.ProcessID");
  const G4int eventOffset = jobNumber * eventsPerJob;

  CLHEP::RanluxEngine randomEngine(1234567, 4);
  G4Random::setTheEngine(&randomEngine);
  auto* verbosity = new grSteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);
  auto* runManager = new G4RunManager;
  auto* detector = new grDetectorConstruction;
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(new grShieldingList(0, "HP", configuration));
#ifdef G4VIS_USE
  auto* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif
  auto* histograms = new grHistoManager;
  runManager->SetUserAction(new grRunAction(histograms));
  runManager->SetUserAction(new grEventAction(histograms, eventOffset, eventWeight, processID));
  runManager->SetUserAction(new grSteppingAction(histograms));
  if (workflow == "cosmic") {
    runManager->SetUserAction(new grPrimaryGeneratorAction(detector));
  } else {
    runManager->SetUserAction(new grFilePrimaryGeneratorAction(configuration, eventOffset));
  }
  runManager->SetUserAction(new grTrackingAction(histograms));
  runManager->SetUserAction(new grStackingAction);

  auto* ui = G4UImanager::GetUIpointer();
  auto* loggedSession = new grSession;
  ui->SetCoutDestination(loggedSession);
  G4int commandStatus = 0;
  if (interactive) {
#ifdef G4UI_USE
    G4cout << "GRENDEL> Enter cosmic interactive mode" << G4endl;
#ifdef G4UI_USE_TCSH
    G4UIterminal session(new G4UItcsh);
#else
    G4UIterminal session;
#endif
    session.SessionStart();
#else
    G4cerr << "GRENDEL> Interactive mode requires GRENDEL_WITH_UIVIS=ON" << G4endl;
    commandStatus = 1;
#endif
  } else {
    G4cout << "GRENDEL> Enter " << workflow << " batch mode" << G4endl;
    commandStatus = ui->ApplyCommand(G4String("/control/execute ") + argv[2]);
  }
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;
  delete verbosity;
  delete loggedSession;
  delete histograms;
  return commandStatus == 0 ? 0 : 1;
}

#include "mqSessionMessenger.hh"
#include "mqSession.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "mqGenFileName.hh"
#include <ctime>

mqSessionMessenger::mqSessionMessenger(mqSession* Sess) :
	mySession(Sess) {
#ifdef DEBUG_MSG
	G4cout << "enter mqSessionMessenger::mqSessionMessenger" << G4endl;
#endif

	char logExtension[6] = ".log";
	G4String defaultFileName = mqGenFileName::genFileName(logExtension);

	G4bool defaultLogOn = false;
	mySession->logOn(defaultLogOn);
	mySession->SetLogFileName(defaultFileName);

	dirLogging = new G4UIdirectory("/logging/");
	dirLogging->SetGuidance("   UI commands specific to Logging.");
	cmdLogFileName = new G4UIcmdWithAString("/logging/logFileName", this);
	cmdLogFileName->SetGuidance("   Name of the Log file.");
	cmdLogFileName->SetGuidance("   Default value: yyyy-mm-dd--hh-mm.log");
	cmdLogFileName->SetParameterName("LogFileName", true);
	cmdLogFileName->SetDefaultValue(defaultFileName);

	cmdLogOn = new G4UIcmdWithABool("/logging/logFileOn", this);
	cmdLogOn->SetGuidance("   Turn logging on");
	cmdLogOn->SetGuidance("   Default value: false");
	cmdLogOn->SetParameterName("LogOn", true);
	cmdLogOn->SetDefaultValue(defaultLogOn);

#ifdef DEBUG_MSG
	G4cout << "leaving mqSessionMessenger::mqSessionMessenger" << G4endl;
#endif
}

mqSessionMessenger::~mqSessionMessenger() {
	delete dirLogging;
	delete cmdLogFileName;
}

void mqSessionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
#ifdef DEBUG_MSG
	G4cout << "enter mqSessionMessenger::SetNewValue" << G4endl;
#endif

	if (command == cmdLogFileName) {
		mySession->SetLogFileName(newValue);
	} else if (command == cmdLogOn) {
		mySession->logOn(cmdLogOn->GetNewBoolValue(newValue));
	}

#ifdef DEBUG_MSG
	G4cout << "leaving mqSessionMessenger::SetNewValue" << G4endl;
#endif
}


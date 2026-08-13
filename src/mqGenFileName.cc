#include "mqGenFileName.hh"
#include <ctime>

char mqGenFileName::prefix[24] = "";

mqGenFileName::mqGenFileName() {
	time_t myTime; //retrieves date and time
	myTime = time(NULL);
	struct tm* myBrokenTime; //Broken-down Time
	myBrokenTime = localtime(&myTime);
	strftime(prefix, 24, "%Y-%m-%d--%H-%M-%S", myBrokenTime);
}

mqGenFileName::~mqGenFileName() {

}

G4String mqGenFileName::genFileName(char postfix[6]) {
	char final[24] = "";
	strcat(final, prefix);
	strcat(final, postfix);
	G4String tmp = final;
	return tmp;
}

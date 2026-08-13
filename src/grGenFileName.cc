#include "grGenFileName.hh"
#include <ctime>

char grGenFileName::prefix[24] = "";

grGenFileName::grGenFileName() {
  time_t myTime; //retrieves date and time
  myTime = time(NULL);
  struct tm* myBrokenTime; //Broken-down Time
  myBrokenTime = localtime(&myTime);
  strftime(prefix, 24, "%Y-%m-%d--%H-%M-%S", myBrokenTime);
}

grGenFileName::~grGenFileName() {

}

G4String grGenFileName::genFileName(char postfix[6]) {
  char final[24] = "";
  strcat(final, prefix);
  strcat(final, postfix);
  G4String tmp = final;
  return tmp;
}

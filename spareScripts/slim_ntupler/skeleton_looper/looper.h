#include <algorithm>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"

struct Directory{
    TDirectory *dir;
    std::map<std::string, TH1*> histmap;
};

void loop(TChain *ch, TFile *fout);
void plot1D(std::string name, float xval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax);
bool isbar(int ch);
bool isslab(int ch);
bool ispanel(int ch);

#include <algorithm>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"

struct Directory{
    TDirectory *dir;
    std::map<std::string, TH1*> histmap;
};

struct Zoom{
    int nbins;
    double xlow, xhigh;
};

struct Config{
    std::string name;
    std::vector<std::string> zooms;
    bool justbars;
};
    
void loop(TChain *ch, TFile *fout, const bool COSMICS=false);
void plot1D(std::string name, float xval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax);
void plot2D(std::string name, float xval, float yval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax,
            int numbinsy, float ymin, float ymax);
void plot2D(std::string name, float xval, float yval, double weight, Directory& dir,
            std::string title, int numbinsx, float xmin, float xmax,
            int numbinsy, const double *ybins);
void plotProfile(std::string name, float xval, float yval, double weight, Directory& dir,
                 std::string title, int numbinsx, float xmin, float xmax, 
                 float ymin, float ymax, std::string opt);
bool isbar(int ch);
bool isslab(int ch);
bool ispanel(int ch);
int bar_layer(int ch);

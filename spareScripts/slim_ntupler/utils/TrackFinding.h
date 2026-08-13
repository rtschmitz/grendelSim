#include <algorithm>
#include <string>

#include "mqROOTEvent.hh"

struct track {
    int id, pid;
    int type; // 0=muon, 1=electron, 2=gamma, 3=photon, 4=neutron, 5=mCP
    uint idx;
    double init_x, init_y, init_z, init_t, init_E;
};

struct track_info {
    int id, pid;
    std::string type, start_volume, end_volume, prod_process;
    int start_copyNo;
    int source_encoding;
    /*
      0: muon
      1: elScint
      2: elRock
      3: elAl
      4: elPb
      -1: other
    */
};

track_info get_track_info(track& tk, mqROOTEvent *revent);
bool tracksortfunc(track& t1, track& t2);
void populate_tracks(mqROOTEvent *revent, std::vector<track>* tracks);
int find_track(int id, std::vector<track>* tracks);
std::vector<track> get_path(int start_id, std::vector<track>* tracks);
float get_path_distance(std::vector<track>& path, const std::vector<float>& init_pos);

#include <algorithm>
#include <numeric>
#include <string>

#include "mqROOTEvent.hh"
#include "mqPMTRHit.hh"
#include "mqMuonTrack.hh"
#include "mqElectronTrack.hh"
#include "mqGammaTrack.hh"
#include "mqPhotonTrack.hh"
#include "mqMCPTrack.hh"
#include "mqNeutronTrack.hh"

#include "TrackFinding.h"

bool _in(std::string s, std::string sub){
    return s.find(sub) != std::string::npos;
}

track_info get_track_info(track& tk, mqROOTEvent *revent){
    track_info ti;
    ti.id = tk.id;
    ti.pid = tk.pid;
    ti.start_copyNo = -1;
    if(tk.type==0){
        ti.type = "muon";
        ti.start_volume = revent->GetMuonTracks()->at(tk.idx)->GetFirstVolume().Data();
        ti.end_volume = revent->GetMuonTracks()->at(tk.idx)->GetLastVolume().Data();
        ti.prod_process = revent->GetMuonTracks()->at(tk.idx)->GetFirstProcessName().Data();
        ti.start_copyNo = revent->GetMuonTracks()->at(tk.idx)->GetFirstCopyNo();
    }
    if(tk.type==1){
        ti.type = "electron";
        ti.start_volume = revent->GetElectronTracks()->at(tk.idx)->GetFirstVolume().Data();
        ti.end_volume = revent->GetElectronTracks()->at(tk.idx)->GetLastVolume().Data();
        ti.prod_process = revent->GetElectronTracks()->at(tk.idx)->GetFirstProcessName().Data();
        ti.start_copyNo = revent->GetElectronTracks()->at(tk.idx)->GetFirstCopyNo();
    }
    if(tk.type==2){
        ti.type = "gamma";
        ti.start_volume = revent->GetGammaTracks()->at(tk.idx)->GetFirstVolume().Data();
        ti.end_volume = revent->GetGammaTracks()->at(tk.idx)->GetLastVolume().Data();
        ti.prod_process = revent->GetGammaTracks()->at(tk.idx)->GetFirstProcessName().Data();
        ti.start_copyNo = revent->GetGammaTracks()->at(tk.idx)->GetFirstCopyNo();
    }
    if(tk.type==3){
        ti.type = "photon";
        ti.start_volume = "";
        ti.end_volume = "";
        ti.prod_process = "";
        ti.start_copyNo = -1;
    }
    if(tk.type==4){
        ti.type = "neutron";
        ti.start_volume = revent->GetNeutronTracks()->at(tk.idx)->GetFirstVolume().Data();
        ti.end_volume = revent->GetNeutronTracks()->at(tk.idx)->GetLastVolume().Data();
        ti.prod_process = revent->GetNeutronTracks()->at(tk.idx)->GetFirstProcessName().Data();
        ti.start_copyNo = revent->GetNeutronTracks()->at(tk.idx)->GetFirstCopyNo();
    }
    if(tk.type==5){
        ti.type = "mCP";
        ti.start_volume = revent->GetMCPTracks()->at(tk.idx)->GetFirstVolume().Data();
        ti.end_volume = revent->GetMCPTracks()->at(tk.idx)->GetLastVolume().Data();
        ti.prod_process = revent->GetMCPTracks()->at(tk.idx)->GetFirstProcessName().Data();
        ti.start_copyNo = revent->GetMCPTracks()->at(tk.idx)->GetFirstCopyNo();
    }
    if(ti.start_volume == "") ti.start_volume="None";
    if(ti.end_volume == "") ti.end_volume="None";
    if(ti.prod_process == "") ti.prod_process="None";

    ti.source_encoding = -1;
    std::string& sv = ti.start_volume;
    if((ti.type=="muon" || ti.type=="mCP") && _in(sv, "World")){
        ti.source_encoding = 0;
    }else if(ti.type=="electron"){
        if(_in(sv,"Scin") || _in(sv,"slab") || _in(sv,"panel") ||_in(sv,"barParam"))
            ti.source_encoding = 1;
        else if(_in(sv,"rock"))
            ti.source_encoding = 2;
        else if(_in(sv,"AlSupport"))
            ti.source_encoding = 3;
        else if(_in(sv,"Lead"))
            ti.source_encoding = 4;
    }

    return ti;
}

bool tracksortfunc(track& t1, track& t2){
    return t1.id < t2.id;
}

void populate_tracks(mqROOTEvent *revent, std::vector<track>* tracks){
    tracks->clear();
    mqMuonTrackVector* muonTracks = revent->GetMuonTracks();
    for(uint i=0; i<muonTracks->size(); i++){
        mqMuonTrack* t = muonTracks->at(i);
        tracks->push_back({t->GetTrackID(), t->GetParentID(), 0, i, t->GetFirstPositionX(), t->GetFirstPositionY(), 
                    t->GetFirstPositionZ(), t->GetTimeOfFirstProcess()/1e-9, t->GetInitialEnergy()});
    }
    mqElectronTrackVector* electronTracks = revent->GetElectronTracks();
    for(uint i=0; i<electronTracks->size(); i++){
        mqElectronTrack* t = electronTracks->at(i);
        tracks->push_back({t->GetTrackID(), t->GetParentID(), 1, i, t->GetFirstPositionX(), t->GetFirstPositionY(), 
                    t->GetFirstPositionZ(), t->GetTimeOfFirstProcess()/1e-9, t->GetInitialEnergy()});
    }
    mqGammaTrackVector* gammaTracks = revent->GetGammaTracks();
    for(uint i=0; i<gammaTracks->size(); i++){
        mqGammaTrack* t = gammaTracks->at(i);
        tracks->push_back({t->GetTrackID(), t->GetParentID(), 2, i, t->GetFirstPositionX(), t->GetFirstPositionY(),
                    t->GetFirstPositionZ(), t->GetTimeOfFirstProcess(), t->GetInitialEnergy()});
    }
    mqPhotonTrackVector* photonTracks = revent->GetPhotonTracks();
    for(uint i=0; i<photonTracks->size(); i++){
        mqPhotonTrack* t = photonTracks->at(i);
        tracks->push_back({t->GetTrackID(), t->GetParentID(), 3, i, -999, -999, -999, -999, -999});
    }
    mqNeutronTrackVector* neutronTracks = revent->GetNeutronTracks();
    for(uint i=0; i<neutronTracks->size(); i++){
        mqNeutronTrack* t = neutronTracks->at(i);
        tracks->push_back({t->GetTrackID(), t->GetParentID(), 4, i, -999, -999, -999, -999, -999});
    }
    mqMCPTrackVector* MCPTracks = revent->GetMCPTracks();
    for(uint i=0; i<MCPTracks->size(); i++){
        mqMCPTrack* t = MCPTracks->at(i);
        tracks->push_back({t->GetTrackID(), t->GetParentID(), 5, i, t->GetFirstPositionX(), t->GetFirstPositionY(), 
                    t->GetFirstPositionZ(), t->GetTimeOfFirstProcess()/1e-9, t->GetInitialEnergy()});
    }
    std::sort(tracks->begin(), tracks->end(), tracksortfunc);
}

int find_track(int id, std::vector<track>* tracks){
    int left_idx = 0;
    int right_idx = tracks->size()-1;
    while(right_idx - left_idx > 1){
        int idx = (left_idx + right_idx) / 2;
        if(tracks->at(idx).id == id)
            return idx;
        if(tracks->at(idx).id < id)
            left_idx = idx;
        else
            right_idx = idx;
    }
    if(tracks->at(left_idx).id == id) return left_idx;
    if(tracks->at(right_idx).id == id) return right_idx;
    return -1;
}

std::vector<track> get_path(int start_id, std::vector<track>* tracks){
    std::vector<track> tks;
    while(start_id > 0){
        int idx = find_track(start_id, tracks);
        if(idx>=0) tks.push_back(tracks->at(idx));
        start_id = idx<0 ? -1 : tracks->at(idx).pid;
    }
    return tks;
}

float get_path_distance(std::vector<track>& path, const std::vector<float>& init_pos){
    float dist = 0.0;
    float x0 = init_pos.at(0), y0 = init_pos.at(1), z0 = init_pos.at(2);
    for(track& t : path){
        if(t.init_x < -998)
            return -1;
        dist += sqrt(pow(x0-t.init_x, 2) + pow(y0-t.init_y, 2) + pow(z0-t.init_z, 2));
        x0 = t.init_x;
        y0 = t.init_y;
        z0 = t.init_z;
    }
    return dist;
}

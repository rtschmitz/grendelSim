#include "grUserEventInformation.hh"

#include <cstddef>

template<class Sequence>
void DeleteAll(Sequence& sequence) {
  for (typename Sequence::iterator it=sequence.begin(); it!=sequence.end(); ++it) delete *it;
  sequence.clear();
}

template<class Sequence>
typename Sequence::value_type FindTrack(Sequence& sequence, G4int trackID) {
  for (typename Sequence::iterator it=sequence.begin(); it!=sequence.end(); ++it)
    if ((*it)->GetTrackID() == trackID) return *it;
  G4Exception("grUserEventInformation::FindTrack", "TrackNotFound", FatalException,
              "A selected track was not registered in PreUserTrackingAction.");
  return 0;
}

grUserEventInformation::grUserEventInformation()
  : runID(-1), eventID(-1), processID(0), eventWeight(0.), trident(false),
    tridentGamma(false), kaonCavern(false), gammaLastTrackID(-1),
    neutronLastTrackID(-1), muonLastTrackID(-1), electronLastTrackID(-1),
    mcpLastTrackID(-1) {}

grUserEventInformation::~grUserEventInformation() { Reset(); }

void grUserEventInformation::Reset() {
  DeleteAll(GammaTracks);
  DeleteAll(NeutronTracks);
  DeleteAll(MuonTracks);
  DeleteAll(ElectronTracks);
  DeleteAll(MCPTracks);
  ScintHits.clear(); // Geant4 owns sensitive-detector hits.
  trident = false;
  tridentGamma = false;
  kaonCavern = false;
}

grGammaTrack* grUserEventInformation::GetGammaTrack(G4int id) { return FindTrack(GammaTracks, id); }
grNeutronTrack* grUserEventInformation::GetNeutronTrack(G4int id) { return FindTrack(NeutronTracks, id); }
grMuonTrack* grUserEventInformation::GetMuonTrack(G4int id) { return FindTrack(MuonTracks, id); }
grElectronTrack* grUserEventInformation::GetElectronTrack(G4int id) { return FindTrack(ElectronTracks, id); }
grMCPTrack* grUserEventInformation::GetMCPTrack(G4int id) { return FindTrack(MCPTracks, id); }

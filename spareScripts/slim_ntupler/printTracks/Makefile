CC=g++ -Wno-deprecated
LDFLAGS=
LIBS=
INCLUDES = ../../include/mqGammaTrack.hh ../../include/mqNeutronTrack.hh ../../include/mqMuonTrack.hh ../../include/mqElectronTrack.hh ../../include/mqMCPTrack.hh ../../include/mqPhotonTrack.hh ../../include/mqPMTRHit.hh ../../include/mqScintRHit.hh ../../include/mqROOTEvent.hh
SOURCES  = ../../src/mqGammaTrack.cc ../../src/mqNeutronTrack.cc ../../src/mqMuonTrack.cc ../../src/mqElectronTrack.cc ../../src/mqMCPTrack.cc ../../src/mqPhotonTrack.cc ../../src/mqPMTRHit.cc ../../src/mqScintRHit.cc ../../src/mqROOTEvent.cc mqROOTEvent_dict.cc  ../../slim_ntupler/utils/TrackFinding.cc printTracks.cc
OBJECTS  = $(SOURCES:.cc=.o)
LINKERFLAGS  = $(shell root-config --ldflags --cflags --libs) -lGenVector
CFLAGS = $(shell root-config --cflags) -I../../include/

printTracks: rdict $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LINKERFLAGS)

rdict: 
	rootcint -f mqROOTEvent_dict.cc $(INCLUDES) ../../include/EventLinkDef.hh

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	rm -f $(OBJECTS) *_rdict.pcm

all: printTracks

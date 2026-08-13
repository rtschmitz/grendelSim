CC=g++ -Wno-deprecated
LDFLAGS=
LIBS=
INCLUDES = ../include/mqGammaTrack.hh ../include/mqNeutronTrack.hh ../include/mqMuonTrack.hh ../include/mqElectronTrack.hh ../include/mqMCPTrack.hh ../include/mqPhotonTrack.hh ../include/mqPMTRHit.hh ../include/mqScintRHit.hh ../include/mqROOTEvent.hh
SOURCES  = ../src/mqGammaTrack.cc ../src/mqNeutronTrack.cc ../src/mqMuonTrack.cc ../src/mqElectronTrack.cc ../src/mqMCPTrack.cc ../src/mqPhotonTrack.cc ../src/mqPMTRHit.cc ../src/mqScintRHit.cc ../src/mqROOTEvent.cc mqROOTEvent_dict.cc $(wildcard utils/*.cc) make_ntuple.cc
OBJECTS  = $(SOURCES:.cc=.o)
LINKERFLAGS  = $(shell root-config --ldflags --cflags --libs) -lGenVector
CFLAGS = $(shell root-config --cflags) -ggdb -I../include/

make_ntuple: rdict SlimTree/SlimTree.o $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) SlimTree/SlimTree.o $(LINKERFLAGS)

rdict: 
	rootcint -f mqROOTEvent_dict.cc $(INCLUDES) ../include/EventLinkDef.hh

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	rm -f $(OBJECTS) SlimTree/SlimTree.o *_rdict.pcm

all: make_ntuple

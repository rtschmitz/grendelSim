CC=g++ -Wno-deprecated
LDFLAGS=
LIBS=
SOURCES  = ../SlimTree/SlimTree.cc looper.cc utils/utils.cc
OBJECTS  = $(SOURCES:.cc=.o)
LINKERFLAGS  = $(shell root-config --ldflags --cflags --libs) -lGenVector
CFLAGS = $(shell root-config --cflags) -ggdb -I../include/

looper: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LINKERFLAGS)


%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS) 

clean:
	rm -f $(OBJECTS)

all: looper

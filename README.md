# grendelSim

`grendelSim` is the standalone Geant4 11 simulation for the GRENDEL detector. It retains two supported muon workflows: a propagated cosmic-muon spectrum generated with Geant4 GPS and a beam-muon workflow generated from an in-repository four-vector table.

## Prerequisites

- Geant4 11 with the high-precision data sets required by the `Shielding`-derived physics configuration
- ROOT 6 with Cling and Physics components
- Boost headers
- CMake 3.16 or newer and a C++17 compiler

Geant4 10 is not supported by this branch. Configure the shell environment for the Geant4 11 and ROOT installations before running CMake. Visualization support is enabled by default; pass `-DGRENDEL_WITH_UIVIS=OFF` for batch-only installations.

When `ROOTSYS` is set and `$ROOTSYS/cmake/ROOTConfig.cmake` exists, the build uses that ROOT installation in preference to a system ROOT package. This keeps ROOT and the active Geant4 compiler environment consistent. You can select ROOT explicitly with `-DROOT_DIR=/path/to/root/cmake` when configuring manually.

## Build

From a clean checkout:

```sh
cmake -S . -B build -DGRENDEL_WITH_UIVIS=OFF
cmake --build build --parallel
```

CMake copies the retained runtime configuration to `build/config`. Run the executable from the build directory so the configuration, optical tables, and beam input resolve through their documented relative paths.

## Interactive visualization

Build with visualization support (the default), then launch the executable without arguments:

```sh
cmake -S . -B build
cmake --build build --parallel
cd build
./grendelSim
```

At the Geant4 prompt, load the visualization setup and generate events:

```text
/control/execute vis.mac
/run/beamOn 1
```

`vis.mac` draws the GRENDEL geometry, stores and colors particle trajectories, and configures a 4 GeV cosmic muon for convenient event viewing. It does not start a run automatically, so additional `/run/beamOn 1` commands display further events. Camera controls and GPS commands can be changed interactively. This mode uses the cosmic GPS generator; the production cosmic and file-driven beam workflows remain unchanged.

The selected Geant4 installation must have an OpenGL visualization driver. Headless batch installations can still be built with `-DGRENDEL_WITH_UIVIS=OFF`, but zero-argument interactive mode is unavailable in that build.

## Cosmic-muon workflow

Entry point: `run_cosmic.sh`. It builds the project and runs 10 events from `macros/cosmic_muon.mac`:

```sh
./run_cosmic.sh
```

The equivalent direct command is:

```sh
cd build
./grendelSim cosmic ../macros/cosmic_muon.mac
```

The macro preserves the propagated surface-muon angular and energy distributions from the source snapshot.

## Beam-muon workflow

Entry point: `run_beam.sh`. It builds the project and runs one event from `macros/beam_muon.mac` using `inputData/muon_hit_4_vecs_large.txt`:

```sh
./run_beam.sh
```

The equivalent direct command is:

```sh
cd build
./grendelSim beam ../macros/beam_muon.mac
```

Beam particle type, event offset, rescaling, and four-vector path are configured by `inputData/config/particles.ini`.

## Tracker geometry

The upper tunnel tracker consists of four tracker stations. The wall-adjacent station begins 1 mm from the tunnel wall, preserving the previous wall clearance. The other stations have edge-to-edge clear distances of 12 cm, 24 cm, and 36 cm from the inward face of the wall station. Each station contains a 1.5 cm phi-segmented scintillator plane, a 1 mm air gap, and a 1.5 cm longitudinally segmented scintillator plane, for a 3.1 cm envelope. Both orientations have a nominal 1 cm channel pitch; the longitudinal planes use native Geant4 replicas for efficient navigation.

Sensitive-hit copy numbers preserve the original stations: `1000 + phiBin` and `10000 + zBin` for the wall station, and `2000 + phiBin` and `20000 + zBin` for the 24 cm station. The new 12 cm and 36 cm stations use layer prefixes 3 and 5 respectively: phi bases 3000 and 5000, and longitudinal bases 30000 and 50000. Lower tunnel scintillator pieces remain copy numbers 0--2.

## Output

Both workflows write a seed-tagged `Sim_<id>grendelSim.root` file in the build directory by default; `/run/fname` replaces that prefix. The `Events` tree uses standalone schema version 3 with one row per simulated event. No GRENDEL headers, shared library, ROOT dictionary, PCM, or rootmap is needed to read it.

The event record contains only `schemaVersion`, `runID`, `eventID`, `processID`, `eventWeight`, and `kaonCavern`. Selected particle collections use the prefixes `muon_`, `gamma_`, `neutron_`, `electron_`, and `kaon_`. Each stores PDG, track, and parent IDs, initial and final copy numbers, times, kinetic energies, positions where available, total track length, processes, and volume names. Elements at the same vector index describe the same track. Which particle species are collected is controlled in `grTrackingAction`.

The `scint_` collection is the reusable detector-interface record. A record is written at every sensitive-volume entry and exit. Each stores track ID, parent ID, PDG ID, copy number, boundary kinetic energy, global time, position, unit direction vector, creator process, origin volume, and `scint_isEntering` (`1` for entry, `0` for exit). Matching records by event, track ID, and copy number permits entry-minus-exit energy scoring; unmatched entries represent particles that stop or terminate inside, and unmatched exits identify particles created inside the sensitive volume. The name is retained for compatibility, but these are sensitive-detector-volume crossings used for downstream geometry and detector-response studies.

Branch names carry units: energy is MeV, positions are metres, sensitive-volume hit times are ns, and selected-track times are seconds. Persisted floating-point vectors use `Float_t`; Geant4 calculations remain double precision. At 100 m, coordinate precision is about 0.001 cm. The writer performs one tree fill per event and uses LZ4 level 4 compression and 50 MiB autoflush clusters to prioritize production CPU.

Schema version 4 represents entry and exit boundaries as complete independent records rather than sparse half-records. Energy scoring must pair the relevant entry and exit records; no subtraction is performed by the writer. It also removes all optical-photon, PMT, crystal, detector-trigger, aggregate energy-deposit, and detector-category branches. It also removes specialized track quantities such as accumulated deposits, interaction counters, momentum duplicates, cavern crossing coordinates, and optical-photon tracks. Optical transport physics and PMT geometry are not changed by this output cleanup.

## Layout

- `grendelSim.cc`: executable entry point and workflow selection
- `include/`, `src/`: GRENDEL geometry, physics, generators, actions, detector response, and standalone ROOT output writer
- `macros/`: cosmic, beam, and interactive visualization macros
- `inputData/config/`: active detector, scintillator, PMT, and beam-generator configuration
- `inputData/muon_hit_4_vecs_large.txt`: retained beam-muon input
- `OpticalData/`: optical spectra and PMT efficiency tables used by detector construction

## Provenance

This repository was extracted from `rtschmitz/milliQanSim`, branch `GRENDEL_G11`, pinned source commit `65b5e9d8b8b608408191706dce2b686c55b6be1e`. The standalone baseline is commit `077b38b545beac4684ed0c13f9e21a891d0182f5`; its intentionally new Git history means its SHA does not match the source commit.

The source repository and its branches are provenance only and are not modified by this repository's workflow.

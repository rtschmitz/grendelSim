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

## Output

Both workflows write a seed-tagged `Sim_<id>grendelSim.root` file in the build directory by default; `/run/fname` replaces that prefix. The `Events` tree uses standalone schema version 3 with one row per simulated event. No GRENDEL headers, shared library, ROOT dictionary, PCM, or rootmap is needed to read it.

The event record contains only `schemaVersion`, `runID`, `eventID`, `processID`, `eventWeight`, and `kaonCavern`. Selected particle collections use the prefixes `muon_`, `gamma_`, `neutron_`, `electron_`, and `kaon_`. Each stores PDG, track, and parent IDs, initial and final copy numbers, times, kinetic energies, positions where available, total track length, processes, and volume names. Elements at the same vector index describe the same track. Which particle species are collected is controlled in `grTrackingAction`.

The `scint_` collection is the reusable detector-interface record. A record is written at every sensitive-volume entry and exit. Each stores track ID, parent ID, PDG ID, copy number, boundary kinetic energy, global time, position, unit direction vector, creator process, origin volume, and `scint_isEntering` (`1` for entry, `0` for exit). Matching records by event, track ID, and copy number permits entry-minus-exit energy scoring; unmatched entries represent particles that stop or terminate inside, and unmatched exits identify particles created inside the sensitive volume. The name is retained for compatibility, but these are sensitive-detector-volume crossings used for downstream geometry and detector-response studies.

Branch names carry units: energy is MeV, positions are metres, sensitive-volume hit times are ns, and selected-track times are seconds. Persisted floating-point vectors use `Float_t`; Geant4 calculations remain double precision. At 100 m, coordinate precision is about 0.001 cm. The writer performs one tree fill per event and uses LZ4 level 4 compression and 50 MiB autoflush clusters to prioritize production CPU.

Schema version 4 represents entry and exit boundaries as complete independent records rather than sparse half-records. Energy scoring must pair the relevant entry and exit records; no subtraction is performed by the writer. It also removes all optical-photon, PMT, crystal, detector-trigger, aggregate energy-deposit, and detector-category branches. It also removes specialized track quantities such as accumulated deposits, interaction counters, momentum duplicates, cavern crossing coordinates, and optical-photon tracks. Optical transport physics and PMT geometry are not changed by this output cleanup.

## Layout

- `grendelSim.cc`: executable entry point and workflow selection
- `include/`, `src/`: GRENDEL geometry, physics, generators, actions, detector response, and standalone ROOT output writer
- `macros/`: the two supported Geant4 run macros
- `inputData/config/`: active detector, scintillator, PMT, and beam-generator configuration
- `inputData/muon_hit_4_vecs_large.txt`: retained beam-muon input
- `OpticalData/`: optical spectra and PMT efficiency tables used by detector construction

## Provenance

This repository was extracted from `rtschmitz/milliQanSim`, branch `GRENDEL_G11`, pinned source commit `65b5e9d8b8b608408191706dce2b686c55b6be1e`. The standalone baseline is commit `077b38b545beac4684ed0c13f9e21a891d0182f5`; its intentionally new Git history means its SHA does not match the source commit.

The source repository and its branches are provenance only and are not modified by this repository's workflow.

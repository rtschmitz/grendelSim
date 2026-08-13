# grendelSim

`grendelSim` is the standalone Geant4 10 simulation for the GRENDEL detector. It retains two supported muon workflows: a propagated cosmic-muon spectrum generated with Geant4 GPS and a beam-muon workflow generated from an in-repository four-vector table.

## Prerequisites

- Geant4 10 with the high-precision data sets required by the `Shielding`-derived physics configuration
- ROOT 6 with Cling and Physics components
- Boost headers
- CMake 3.16 or newer and a C++11 compiler

This compatibility branch targets Geant4 10; use `main` for Geant4 11. Configure the shell environment for the Geant4 10 and ROOT installations before running CMake. Visualization support is enabled by default; pass `-DGRENDEL_WITH_UIVIS=OFF` for batch-only installations.

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

Unless `/run/fname` supplies an output directory, both workflows create `grendelSim.root` in the build directory. The ROOT tree remains `Events` and its branch remains `ROOTEvent`; physical fields and data types are unchanged.

The standalone rename changes the output file from `MilliQan.root` to `grendelSim.root` and the project-owned ROOT class names from the `mq*` prefix to `gr*` (for example, `mqROOTEvent` to `grROOTEvent`, and the particle/hit record classes likewise). Existing analysis that names those C++ dictionary types must be updated; the tree name, branch name, member meanings, and stored types are otherwise preserved.

## Layout

- `grendelSim.cc`: executable entry point and workflow selection
- `include/`, `src/`: GRENDEL geometry, physics, generators, actions, detector response, and ROOT event model
- `macros/`: the two supported Geant4 run macros
- `inputData/config/`: active detector, scintillator, PMT, and beam-generator configuration
- `inputData/muon_hit_4_vecs_large.txt`: retained beam-muon input
- `OpticalData/`: optical spectra and PMT efficiency tables used by detector construction

## Provenance

This repository was extracted from `rtschmitz/milliQanSim`, branch `GRENDEL_G11`, pinned source commit `65b5e9d8b8b608408191706dce2b686c55b6be1e`. The Geant4 10 compatibility changes are ported from the known-working `GRENDEL` branch at commit `2d4530324e92ef635c8cec052cc9c706bcfc74e8`. The standalone baseline is commit `077b38b545beac4684ed0c13f9e21a891d0182f5`; its intentionally new Git history means its SHA does not match the source commit.

The source repository and its branches are provenance only and are not modified by this repository's workflow.

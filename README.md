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

Both workflows write a seed-tagged `Sim_<id>grendelSim.root` file in the build directory by default; `/run/fname` replaces that prefix. The `Events` tree uses standalone schema version 1: one row per simulated event, scalar event branches, and parallel `std::vector` branches for variable-length track and hit collections. No GRENDEL headers, shared library, ROOT dictionary, PCM, or rootmap is needed to read it.

Branch prefixes identify collections: `muon_`, `gamma_`, `neutron_`, `electron_`, `mcp_`, `photon_`, `scint_`, and `pmt_`. Elements at the same vector index describe the same track or hit. Branch names carry units where applicable: energy is MeV except optical-photon and PMT quantities explicitly suffixed `_eV`; positions are metres; scintillator and PMT times are ns; all track times are seconds. `schemaVersion` must be checked by downstream readers before interpreting a file.

The writer uses LZ4 level 4 compression and 50 MiB autoflush clusters to minimize production CPU and support efficient bulk reads. The output contains only ROOT fundamental types, `std::string`, and standard-library vectors.

This schema replaces the legacy `ROOTEvent` custom-object branch. In addition to eliminating its analysis-library dependency, it records event flags directly and removes redundant stored collection counts; use vector lengths for multiplicities. The old `grROOTEvent`, `gr*RHit`, `ClassDef`, dictionary, and shared-library interfaces are intentionally not available on this branch.

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

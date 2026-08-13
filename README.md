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

Both workflows write a seed-tagged `Sim_<id>grendelSim.root` file in the build directory by default; `/run/fname` replaces that prefix. Schema version 3 is standalone and contains three ordinary ROOT TTrees with scalar branches. No GRENDEL headers, shared library, dictionary, PCM, rootmap, or custom decoding is required. The trees can be read directly with ROOT, RDataFrame, or uproot.

- `Events` has one row per event: schema and run identifiers, process ID, event weight, `kaonCavern`, and the numbers of selected tracks and sensitive-volume entries.
- `Tracks` has one row per explicitly selected particle track. `pdgID` identifies the species, replacing separate mostly empty per-species branch groups. It stores track and parent IDs, initial and final copy numbers, times, kinetic energies, positions, total track length, processes, and volumes. Electron final positions are `NaN` because the tracking action does not collect them.
- `Hits` has one row each time a particle enters a sensitive volume. It stores run/event, track/parent, PDG and copy identifiers; incoming kinetic energy, global time, position, unit direction vector, creator process, and origin volume. Entry direction is included so the boundary phase space can be propagated into alternate downstream detector models.

Branch names carry units: energies are MeV, positions are metres, hit times are ns, and selected-track times are seconds. Floating-point values use standard ROOT `Float_t` branches. At a 100 m coordinate magnitude their precision is about 0.001 cm, comfortably finer than the required 0.1 cm resolution. IDs remain ordinary 32-bit integers. LZ4 level 4 and large autoflush clusters favor simulation throughput.

Schema version 3 eliminates the duplicate exit record for every sensitive-volume crossing and the invalid `scint_energyDeposit_MeV` value previously formed by subtracting two independent records. `Hits.kineticEnergy_MeV` now states the actual stored quantity. Exit coordinates, exit time, and exit energy are intentionally omitted because entry phase space is the reusable detector interface. The schema also removes vector-container overhead and converts output doubles to floats; simulation calculations remain double precision.

Optical-photon, PMT, crystal, detector-trigger, aggregate energy-deposit, detector-category, and specialized track branches remain excluded. Optical transport physics and PMT geometry are not changed by this output cleanup.

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

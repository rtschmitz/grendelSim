# Resources

All measurements use the 4M-event cosmic sample (`build/grendelSim.root`,
curved 110 m tunnel, all four tracker stations simulated) with the default
settings and tracklet finding in stations 0 and 2.

## File sizes

| Stage | Output | Events saved | Size | per saved event | per raw event | at 10¹¹ raw events |
|---|---|---|---|---|---|---|
| raw | Geant4 `Events` | 4,000,000 | 9.36 GB | | 2.3 kB | 234 TB |
| 1 | cells | 1,971,825 (any deposit) | 3.03 GB | 1,537 B | 758 B | 76 TB |
| 2 | hits | 1,971,825 | 0.60 GB | 302 B | 150 B | 15 TB |
| 3 | tracklets | 1,453,343 (≥ 1 tracklet) | 0.72 GB | 493 B | 179 B | 18 TB |
| 4 | vertices | 404,527 (≥ 2 tracklets) | 0.31 GB | 761 B | 77 B | 7.7 TB |
| 5 | cutflow | 404,527 | 2 MB | 4.9 B | 0.5 B | 50 GB |

Each stage is self-contained, so earlier stages can be deleted once the
later ones exist.

What takes the space:

| cells (stage 1) | share | hits (stage 2) | share |
|---|---|---|---|
| positions | 45% | truth / smeared position | 16% / 16% |
| E, t, element code | 21% | contributor lists | 17% |
| contributor lists | 18% | phi / z energies | 9% |
| station, sublayer, chord, channels | 11% | cluster sizes | 8% |
| dominant truth | 4% | station, chord, channels | 8% |
| | | truth / smeared time | 4.5% / 4.6% |

Keeping the smeared position and time next to the truth ones costs 21% of
the hits file. 98% of cells have a single contributor.

**Compression.** Rounding (0.1 mm, 1 ps) plus ZSTD level 9 makes the cells
file 17% smaller and 2.5× faster to read than unrounded values at
uproot's default compression, and adds
about 2% to the run time. `--zstd-level 3` writes 15% faster and the files
are 10% larger.

## Run time and memory

Full sample: 6 jobs in parallel, one per sixth of the events (667k raw
events each):

| Stage | Wall time per job | per raw event |
|---|---|---|
| 1 cells | 110 s | 0.17 ms |
| 2 hits | 40–48 s | 0.07 ms |
| 3 tracklets | 16 s | 0.024 ms |
| 4 vertices | 5 s | 0.008 ms |
| 5 cutflow | < 1 s (all files) | — |
| **total** | | **0.27 ms** |

The simulation runs at about 40 Hz, i.e. 25 ms per event. The full
post-processing chain therefore adds about 1% of the simulation time:
about 0.9 core-years for 10¹¹ events, against about 80 for the
simulation.

Single job on the first 100k raw events, including about 3 s of Python and
numba start-up per stage:

| Stage | Wall time | Peak memory |
|---|---|---|
| 1 | 12.0 s | 2.9 GB |
| 2 | 4.2 s | 2.1 GB |
| 3 | 7.4 s | 0.5 GB |
| 4 | 4.4 s | 0.4 GB |
| 5 | 0.3 s | 0.07 GB |

Memory depends on the chunk size (`--step-size`, default 50k events,
100k for stage 5), not on the file size. Lower `--step-size` if a grid
slot has less than 3 GB.

## Event flow (stations 0 and 2)

| Step | Events | low mass | high mass |
|---|---|---|---|
| simulated | 4,000,000 | | |
| ≥ 1 tracklet | 1,453,343 | | |
| ≥ 2 tracklets | 404,527 | 179,295 | 225,232 |
| no veto | 166,490 | 2,457 | 164,033 |
| exactly 2 tracklets | 136,685 | 1,750 | 134,935 |
| no free hits | 136,431 | 1,596 | 134,835 |
| not through-going | 8,039 | 1,516 | 6,523 |
| track timing | 212 | 156 | 56 |
| separation | 165 | 147 | 18 |
| decay side | 10 | 0 | 10 |
| fiducial | 5 | 0 | 5 |
| DCA | 1 | 0 | 1 |
| vertex dt | 0 | 0 | 0 |

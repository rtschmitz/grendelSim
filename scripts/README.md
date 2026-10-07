# GRENDEL background post-processing

These scripts turn the raw Geant4 output of `grendelSim` (scintillator
boundary crossings) into tracker hits, tracklets, a vertex per event and a
background-selection cutflow. Each stage reads the previous stage's files
and writes a self-contained ROOT file (classic TTrees). An event can be
followed through every stage by its `eventID`.

```
raw Events ──1──▶ cells ──2──▶ hits ──3──▶ tracklets ──4──▶ vertices ──5──▶ cutflow
            1 cm deposits   phi+z MIP   straight      best DCA        per-event
            + truth         hits,       crossings,    vertex          pass flags
                            smeared     stations 0,2                  + table
```

| Stage | What it builds | Events kept |
|---|---|---|
| 1 | energy deposits on a 1 cm grid ("cells"), with every contributing particle | any deposit |
| 2 | tracker hits: a phi and a z cluster of one station, matched in space and time, ≥ 1.5 MeV each; per-event veto counts | all from stage 1 |
| 3 | tracklets: straight, speed-of-light crossings of the tracker on one side of the tunnel | ≥ 1 tracklet |
| 4 | the best vertex (closest approach of two tracklets) and the quantities the selection uses | ≥ 2 tracklets |
| 5 | the 13-cut background selection: per-event flags and a cutflow table | all from stage 4 |

Stage 2 makes hits in all four tracker stations. By default stage 3 finds
tracklets with the two-layer design (stations 0 = wall and 2 = 24 cm)
but keeps every station's hits in its output, so other configurations need
only stages 3–5 rerun.

## Documentation

* [`docs/cuts.md`](docs/cuts.md): every cut used to build cells, hits,
  tracklets and vertices, with its value, justification and supporting
  plots, plus the stage-5 selection.
* [`docs/outputs.md`](docs/outputs.md): branch-by-branch contents of every
  output, and the conventions (units, stations, channels, truth).
* [`docs/resources.md`](docs/resources.md): file sizes, run time and memory,
  projections to 10¹¹ events, and the event flow on the 4M-event sample.

## Requirements

Python ≥ 3.9 with `numpy`, `numba`, `awkward` ≥ 2, `uproot` ≥ 5, `pandas`,
plus `matplotlib` for the studies. Tested with uproot 5.7, awkward 2.9,
numba 0.60, and ROOT 6.24 for reading.

## Running

Each script processes the files it is given and writes one output. Jobs
can be split however you like: stage 1 by entry range of the raw file,
later stages one job per input file. Results don't depend on the split,
including the smearing.

```bash
python3 scripts/stage1_consolidate.py grendelSim.root cells_0.root --entry-start 0 --entry-stop 1000000
python3 scripts/stage2_trackerhits.py cells_0.root     --output hits_0.root
python3 scripts/stage3_tracklets.py   hits_0.root      --output tracklets_0.root
python3 scripts/stage4_vertices.py    tracklets_0.root --output vertices_0.root
python3 scripts/stage5_cutflow.py     'vertices_*.root' --output cutflow.root   # all files, one table
```

Inputs accept several files or quoted globs. Every option is listed by
`--help`, and the values used are stored in each output's `meta` tree.
Event counts (`nEventsProcessed`) carry through to the stage-5 table. An
existing output file is replaced.

## Files

| File | Purpose |
|---|---|
| `stage1_consolidate.py` | raw crossings → cells. Pairs each particle's entry/exit crossings into segments with net deposit KE_in − KE_out (secondaries' energy is subtracted from the parent), spreads them along the path into 1 cm tracker cells / 10 cm veto cells, and keeps E > 1 keV with all contributors. |
| `stage2_trackerhits.py` | cells → tracker hits. Touching, time-consistent sublayer clusters, 1.5 MeV MIP threshold, one-to-one phi–z matching (closest cells < 2.5 cm, \|Δt\| < 3 ns), 0.7 ns / 3 mm reproducible smearing, truth and smeared values stored; veto counts per element. |
| `stage3_tracklets.py` | hits → tracklets. Seeds by angle, timing and same-side crossing, extends and fits a line, picks the non-overlapping set, then fits time, direction, through-going partner and truth. `--truth` uses unsmeared hits. |
| `stage4_vertices.py` | tracklets → best vertex. DCA of every tracklet pair, position relative to the tracker and tunnel, decay side, timing, separations, collinearity and pointing to the CMS IP. |
| `stage5_cutflow.py` | vertices → per-event pass flags (cumulative and N−1 mask), low/high-mass region and the cutflow table; prints the table. |
| `geometry.py` | tunnel geometry mirroring `src/grDetectorConstruction.cc`: chords, cross-section, station depths, global ↔ tunnel coordinates, CMS IP. Numba-accelerated. |
| `clustering.py` | numba kernels for stage 2: touching-cell grouping, closest-cell distance, one-to-one pairing. |
| `tunnel_coords.py` | adds continuous tunnel coordinates (arc length, perimeter, depth, cross-section x/y) for every position in a cells/hits file as a friend tree. |
| `studies/cut_studies.py` | regenerates the plots and numbers in `docs/cuts.md` from a stage-1 and a stage-2 file. |
| `docs/` | documentation and `plots/`. |

## Geometry notes

* The geometry in `geometry.py` must match the simulation that produced
  the raw file. If `src/grDetectorConstruction.cc` changes (tunnel shape,
  station gaps, layer thicknesses), update `geometry.py` to match.
* A volume ID doesn't tell you where a hit is: z sublayers are single
  volumes along the tunnel, and phi strip IDs repeat in every chord. This
  is why stage 1 bins deposits by position.
* The perimeter coordinate follows the mitred offset of the arch polyline
  at each point's own depth, so it is continuous across facet corners and
  clusters don't split there.
* The CMS IP (`geometry.IP`) is an estimate from the GRENDEL paper's site
  drawing (see `docs/cuts.md`).

## Validation

* The cleaned scripts reproduce the full-sample outputs bit for bit on an
  independent 100k-event rerun of all five stages.
* Stage 1: pass-through particles are never attributed (0 of 176k).
  Contributor energies sum to the cell energy, and the muon cluster energy
  peaks at the expected 3.0 MeV.
* Stage 2: the smeared hit time has σ = 0.49 ns and the position 3 mm per
  direction. The output is identical for any chunking.
* Stage 3: constructed parallel, crossed and muon-plus-delta-ray events
  give the intended tracklets.

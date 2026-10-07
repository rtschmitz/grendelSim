# Output files

Every file is a ROOT file with classic TTrees, readable by ROOT 6.24+ and
uproot. Each file has a `meta` tree with one entry: the event counts and
every setting used. Collections are jagged branches with a counter: `ncell`
for `cell_*`, `nhit` for `hit_*`, and so on.

## Conventions

* **Units:** m, ns, MeV. Positions are in the Geant4 global frame (y is
  vertical; the tunnel lies in the x–z plane).
* **Station** (tracker layer): 0 = at the wall, 1 = 12 cm, 2 = 24 cm,
  3 = 36 cm inward. **Sublayer:** 0 = phi (outer; measures position around
  the arch), 1 = z (inner; measures position along the tunnel).
* **Chord:** one of the 15 straight tunnel segments (0–14) along the
  curved, 110 m centreline.
* **Channels:** 1 cm cells numbered along the axis (`chanS` = arc length /
  1 cm, 0–10999) and around the arch (`chanPerim` = distance along the
  surface from the right wall at mid height / 1 cm, 0–653). Veto cells are
  10 cm: `chanS` along the axis, and `chanPerim` across the floor or up
  the wall.
* **Precision:** positions are stored to 0.1 mm and times to 1 ps;
  energies are float32. Times are float32, like the raw Geant4 output, so
  deposits more than about 1 ms after the event start have only 0.1–0.5 ns
  precision.
* **Truth:** Geant4 track ID and PDG ID. A particle is listed as a
  contributor only if it deposited nonzero energy in that object. The
  "dominant" particle is the first (largest) contributor. No cut uses
  truth.
* **eventID** is the entry number in the raw file, so the same event can be
  matched across every stage.

Continuous tunnel coordinates (arc length, perimeter, depth) are not stored.
Use `tunnel_coords.py` to write them as a friend tree, or call
`geometry.tunnel_coordinates(x, y, z)` directly.

---

## Stage 1 — `cells`

One entry per event with at least one cell.

| Branch | Type | Meaning |
|---|---|---|
| `eventID`, `nRawHits` | int32 | event; raw boundary crossings in it |
| `ncell` | int32 | number of cells |
| `cell_det` | int16 | detector element code (table below) |
| `cell_station`, `cell_sublayer` | int16 | tracker station 0–3 and sublayer 0/1 (−1 for veto cells) |
| `cell_chord`, `cell_chanS`, `cell_chanPerim` | int16 | tunnel segment and channels |
| `cell_E` | float32 | net energy deposited [MeV] |
| `cell_t` | float32 | earliest deposit time [ns] |
| `cell_x/y/z` | float32 | energy-weighted position [m] |
| `cell_trk`, `cell_pdg` | int32 | dominant particle |
| `cell_nContrib` | int32 | contributors per cell |
| `ncell_contrib` | int32 | contributors in the event |
| `cell_contrib_trk/pdg` | int32 | each contributor |
| `cell_contrib_E` | float32 | its deposit in the cell [MeV] |

Contributor lists are flat per event in cell order. The first
`cell_nContrib[0]` entries belong to cell 0, and so on. Within a cell they
run from the highest energy down.

| `cell_det` | Element |
|---|---|
| 10·st + 0 / 10·st + 1 | phi / z sublayer of station `st` |
| 200 | floor veto (Geant4 copy 100) |
| 210 / 211 | right / left lower-wall veto (copies 110 / 111) |
| 220/221, 230/231, 240/241 | right / left wall extensions of stations 1, 2, 3 |

`meta`: `nEventsProcessed`, `nRawHits`, `nCells`, `trackerCell_m`,
`vetoCell_m`, `minEdep_MeV`, `minContribEdep_MeV`, precisions.

## Stage 2 — `hits`

One entry per event of the cells file.

| Branch | Type | Meaning |
|---|---|---|
| `eventID` | int32 | |
| `smearSeed` | uint32 | seed of this event's smearing |
| `veto_n_floor`, `veto_n_wallR/L` | int32 | veto cells above threshold: floor, lower walls |
| `veto_n_ext{1,2,3}{R,L}` | int32 | same for the wall extensions of stations 1–3 |
| `veto_tmin`, `veto_maxE` | float32 | earliest time / largest energy of those cells (−1 / 0 if none) |
| `nhit` | int32 | tracker hits |
| `hit_station`, `hit_chord` | int16 | station; segment of the truth position |
| `hit_chanS`, `hit_chanPerim` | int16 | seed channel of the z / phi cluster |
| `hit_Ephi`, `hit_Ez` | float32 | energy of the phi / z cluster [MeV] |
| `hit_t`, `hit_tTruth` | float32 | measured (smeared) / true time [ns] |
| `hit_x/y/z` | float32 | truth position: midpoint of the two cluster centres [m] |
| `hit_xSmear/ySmear/zSmear` | float32 | smeared position [m] |
| `hit_dmin` | float32 | phi–z closest-cell distance [m] |
| `hit_nCellPhi/Z` | int16 | cells per cluster |
| `hit_widthPerimPhi/Z`, `hit_widthSPhi/Z` | int16 | cluster extent around the arch / along the axis, in channels |
| `hit_trk`, `hit_pdg` / `hit_trk_z`, `hit_pdg_z` | int32 | dominant particle of the phi / z cluster |
| `hit_nContrib`, `nhit_contrib` | int32 | contributors per hit / per event |
| `hit_contrib_trk/pdg` | int32 | each distinct contributor |
| `hit_contrib_Ephi/Ez` | float32 | its energy in the phi / z cluster [MeV] |

**Smearing** is a pure function of (`smearSeed`, quantity, index of the
object within the event). `smearSeed` is a splitmix64 hash of (`--seed`,
`eventID`). The output therefore doesn't depend on how jobs are split, and
any single event can be re-smeared.

`meta`: `nEventsProcessed`, `nEventsWithCells`, `nClusters`, `nHits`,
`stationMask`, `baseSeed` and every threshold.

## Stage 3 — `tracklets`

Events with ≥ 1 tracklet. Contains every stage-2 branch (hits of **all**
stations) plus:

| Branch | Type | Meaning |
|---|---|---|
| `hit_nTracklets` | int16 | tracklets using the hit (−1: station not used for tracking) |
| `ntrkl` | int32 | tracklets |
| `trkl_nHits` | int16 | hits on the tracklet |
| `trkl_x/y/z`, `trkl_dx/dy/dz` | float32 | line: hit centroid and unit direction, from the innermost hit towards the outermost |
| `trkl_t0` | float32 | mean hit time [ns] |
| `trkl_maxRes`, `trkl_rms` | float32 | hit residuals to the line [m] |
| `trkl_invBeta`, `trkl_invBetaErr` | float32 | fitted 1/β along the direction (> 0: moving outward) |
| `trkl_chi2Out`, `trkl_chi2In` | float32 | time χ² for outgoing / incoming at c |
| `trkl_outgoing` | int16 | chi2Out < chi2In |
| `trkl_primary` | int16 | 1 = in the non-overlapping set; 0 = added for a leftover hit |
| `trkl_nShared`, `trkl_nConflict` | int16 | hits shared with other tracklets / number of tracklets sharing them |
| `trkl_partner` | int16 | index of the through-going partner (−1: none) |
| `trkl_mergeRes`, `trkl_mergeInvBeta` | float32 | common-line residual and 1/β with the partner |
| `trkl_truthTrk`, `trkl_truthPdg`, `trkl_truthEFrac` | | dominant particle and its share of the tracklet energy |
| `ntrkl_hit`, `trkl_hit_idx` | int16 | hits of each tracklet (`trkl_nHits` per tracklet), as indices into the event's `hit_*` arrays |

`meta`: counts, `stationMask` (stations used for finding), `usesTruth` and
every tolerance.

## Stage 4 — `vertices`

Events with ≥ 2 tracklets. Contains every stage-3 branch plus one best vertex
per event:

| Branch | Meaning |
|---|---|
| `vtx_nCand` | tracklet pairs considered |
| `vtx_i1`, `vtx_i2` | the two tracklets (indices into `trkl_*`) |
| `vtx_dca`, `vtx_x/y/z`, `vtx_chord` | closest approach [m]; its midpoint; segment |
| `vtx_dInner` | signed distance inside the innermost tracking station (> 0: fiducial) [m] |
| `vtx_dWall` | signed distance inside the nominal tunnel surface [m] |
| `vtx_L1`, `vtx_L2` | vertex position along each tracklet relative to its innermost hit (≤ 0: inward) [m] |
| `vtx_decaySide` | both L ≤ 0 |
| `vtx_t1`, `vtx_t2`, `vtx_dt`, `vtx_tChi2` | emission time at the vertex of each tracklet, difference, common-time χ² |
| `vtx_nHits` | hits on the two tracklets |
| `vtx_openAngle` | angle between the outward directions [rad] |
| `vtx_sepInner`, `vtx_sepOuter` | hit separation at the innermost / outermost common station [m] |
| `vtx_collMax` | max residual of all hits to one line [m] |
| `vtx_pointAngle`, `vtx_distIP` | angle between the summed direction and IP→vertex [rad]; distance to the IP [m] |
| `vtx_nShared`, `vtx_sameTruth` | shared hits; same dominant particle |

`meta`: `nEventsProcessed`, `nEventsIn`, `nEvents`, `nCandidatePairs`, IP,
`stationMask`, `usesTruth`.

## Stage 5 — `cutflow`

One entry per stage-4 event. No hits, tracklets or vertices are stored;
join on `eventID` to look at them.

| Branch | Type | Meaning |
|---|---|---|
| `eventID` | int32 | |
| `pass_noVeto` … `pass_pointing` | bool | passes this cut and all before it (13 flags, in cut order) |
| `nPassed` | int8 | consecutive cuts passed (0–13) |
| `passMask` | int16 | bit k−1 set if cut k passes on its own (for N−1 studies) |
| `region` | int8 | 0 = low mass (outer separation < 10 cm), 1 = high mass |

Tree `table`: `step`, `nAll`, `nLow`, `nHigh`. Step −2 is simulated,
−1 is ≥ 1 tracklet, 0 is ≥ 2 tracklets, and 1–13 are the cuts. String
`cutNames` and the `meta` tree record the settings.

## Reading

```python
import uproot
t = uproot.open("vertices_0.root")["vertices"]
ev = t.arrays(["eventID", "hit_station", "hit_t", "trkl_nHits", "vtx_dca"])
```

```cpp
TFile f("vertices_0.root"); auto t = (TTree*)f.Get("vertices");
t->Draw("vtx_dca", "vtx_dInner > 0");
```

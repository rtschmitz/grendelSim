#!/usr/bin/env python3
"""Stage 5: background selection cutflow, one result per event.

Input: stage-4 vertex files (events with >= 2 tracklets and their best
vertex; tracklets built from the stations chosen in stage 3, by default the
two-layer design 0 and 24 cm).  Every event is evaluated against the cuts,
in order:

  1  no veto            no veto cell above threshold in the floor, the lower
                        walls and the wall extensions of the innermost
                        tracking station (veto_n_* from stage 2)
  2  2 tracklets        exactly two tracklets
  3  no free hits       no good hit of the tracking stations without a
                        tracklet (hit_nTracklets == 0)
  4  not through-going  neither tracklet has a collinear partner
  5  track timing       each tracklet's outward speed-of-light chi2 / dof <
                        --track-chi2 (3.92 = the |dt - d/c| < 1.4 ns window
                        for two hits of 0.5 ns)
  6  separation         --sep-min < inner-station separation < --sep-max
  7  decay side         the vertex lies inward of both tracklets
  8  fiducial           vertex inside the inner tracker (dInner > 0)
  9  DCA                < --dca
 10  vertex dt          |t1 - t2| at the vertex < --vtx-dt
 11  parallel           opening angle < --par-angle => outer sep < --par-sep
 12  collinearity       outer sep > --coll-sep => collinearity > --coll-min
 13  pointing           < --point-low (outer sep < --sr-split) else < --point-high

Output (no hits, tracklets or vertices):
  tree "cutflow", one entry per input event: eventID; one flag per stage,
      pass_noVeto ... pass_pointing (true if the event passes that cut and
      all cuts before it); nPassed (number of consecutive cuts passed,
      0-13); passMask (bit k-1 set if cut k passes on its own, for N-1
      studies); region (0 = low mass: outer separation < --sr-split,
      1 = high mass).
  tree "table", one entry per cutflow step: step, nAll, nLow, nHigh
      (events remaining; step -2 = simulated, -1 = >= 1 tracklet,
      0 = >= 2 tracklets, 1..13 = cuts).
  string "cutNames" and tree "meta" with all settings.
"""
import argparse
import glob
import os
import sys

import awkward as ak
import numpy as np
import uproot

CUTS = ["no veto", "exactly 2 tracklets", "no unassigned good hits", "no through-going tracklet",
        "track timing", "separation", "decay side", "inside inner tracker", "DCA", "vertex dt",
        "parallel", "collinearity", "pointing"]
STAGE_BRANCHES = ["pass_noVeto", "pass_twoTracklets", "pass_noFreeHits", "pass_notThroughGoing",
                  "pass_trackTiming", "pass_separation", "pass_decaySide", "pass_fiducial", "pass_dca",
                  "pass_vertexDt", "pass_parallel", "pass_collinearity", "pass_pointing"]
VETO_EXT = {1: ("ext1R", "ext1L"), 2: ("ext2R", "ext2L"), 3: ("ext3R", "ext3L")}
KEYS = ["eventID", "ntrkl", "veto_n_floor", "veto_n_wallR", "veto_n_wallL"] + \
       ["veto_n_" + k for v in VETO_EXT.values() for k in v] + \
       ["hit_nTracklets", "trkl_partner", "trkl_nHits", "trkl_chi2Out"] + \
       ["vtx_" + k for k in ("sepInner", "sepOuter", "decaySide", "dInner", "dca", "dt", "openAngle",
                             "collMax", "pointAngle")]


def evaluate(a, innermost, args):
    """Boolean array (events x cuts) of each cut on its own, and the region."""
    veto = sum(a["veto_n_" + k].to_numpy() for k in ("floor", "wallR", "wallL") + VETO_EXT.get(innermost, ()))
    free = ak.sum(a["hit_nTracklets"] == 0, axis=1).to_numpy()
    through = ak.any(a["trkl_partner"] >= 0, axis=1).to_numpy()
    ndf = np.maximum(a["trkl_nHits"] - 1, 1)
    timing = ak.all(a["trkl_chi2Out"] / ndf < args.track_chi2, axis=1).to_numpy()
    v = {k: a["vtx_" + k].to_numpy() for k in ("sepInner", "sepOuter", "decaySide", "dInner", "dca", "dt",
                                               "openAngle", "collMax", "pointAngle")}
    low = v["sepOuter"] < args.sr_split
    P = np.zeros((len(a), len(CUTS)), bool)
    P[:, 0] = veto == 0
    P[:, 1] = a["ntrkl"].to_numpy() == 2
    P[:, 2] = free == 0
    P[:, 3] = ~through
    P[:, 4] = timing
    P[:, 5] = (v["sepInner"] > args.sep_min) & (v["sepInner"] < args.sep_max)
    P[:, 6] = v["decaySide"] == 1
    P[:, 7] = v["dInner"] > 0
    P[:, 8] = v["dca"] < args.dca
    P[:, 9] = np.abs(v["dt"]) < args.vtx_dt
    P[:, 10] = ~((v["openAngle"] < args.par_angle) & (v["sepOuter"] >= args.par_sep))
    P[:, 11] = ~((v["sepOuter"] > args.coll_sep) & (v["collMax"] <= args.coll_min))
    P[:, 12] = np.where(low, v["pointAngle"] < args.point_low, v["pointAngle"] < args.point_high)
    return P, low


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("inputs", nargs="+", help="stage-4 vertex files (globs allowed)")
    ap.add_argument("--output", required=True)
    ap.add_argument("--track-chi2", type=float, default=3.92, help="max outward chi2/dof per tracklet")
    ap.add_argument("--sep-min", type=float, default=0.01)
    ap.add_argument("--sep-max", type=float, default=10.0)
    ap.add_argument("--dca", type=float, default=0.10)
    ap.add_argument("--vtx-dt", type=float, default=1.0)
    ap.add_argument("--par-angle", type=float, default=0.1)
    ap.add_argument("--par-sep", type=float, default=0.30)
    ap.add_argument("--coll-sep", type=float, default=0.16)
    ap.add_argument("--coll-min", type=float, default=0.096)
    ap.add_argument("--sr-split", type=float, default=0.10)
    ap.add_argument("--point-low", type=float, default=0.050)
    ap.add_argument("--point-high", type=float, default=0.800)
    ap.add_argument("--zstd-level", type=int, default=9)
    ap.add_argument("--step-size", type=int, default=100000)
    args = ap.parse_args()
    files = sorted(sum((glob.glob(p) for p in args.inputs), []))
    if os.path.exists(args.output):
        os.remove(args.output)
    fout = uproot.recreate(args.output, compression=uproot.ZSTD(args.zstd_level))
    nproc = n1 = 0
    table = np.zeros((len(CUTS) + 1, 3), np.int64)        # row 0 = input (>= 2 tracklets)
    first = True
    station_mask = 0
    for fn in files:
        f = uproot.open(fn)
        m = f["meta"].arrays(library="np")
        nproc += int(m["nEventsProcessed"][0])
        n1 += int(m["nEventsIn"][0])
        station_mask = int(m["stationMask"][0])
        innermost = max(s for s in range(4) if station_mask >> s & 1)
        for a in f["vertices"].iterate(KEYS, step_size=args.step_size, library="ak"):
            P, low = evaluate(a, innermost, args)
            cum = np.cumprod(P, axis=1).astype(bool)
            table[0] += [len(a), low.sum(), (~low).sum()]
            for k in range(len(CUTS)):
                table[k + 1] += [cum[:, k].sum(), (cum[:, k] & low).sum(), (cum[:, k] & ~low).sum()]
            data = {"eventID": a["eventID"].to_numpy().astype(np.int32)}
            for k, b in enumerate(STAGE_BRANCHES):
                data[b] = cum[:, k]
            data["nPassed"] = cum.sum(axis=1).astype(np.int8)
            data["passMask"] = (P * (1 << np.arange(len(CUTS)))).sum(axis=1).astype(np.int16)
            data["region"] = (~low).astype(np.int8)
            if first:
                fout.mktree("cutflow", {k: v.dtype for k, v in data.items()})
                first = False
            fout["cutflow"].extend(data)
    tab = {"step": np.arange(-2, len(CUTS) + 1).astype(np.int32),
           "nAll": np.r_[nproc, n1, table[:, 0]].astype(np.int64),
           "nLow": np.r_[-1, -1, table[:, 1]].astype(np.int64),
           "nHigh": np.r_[-1, -1, table[:, 2]].astype(np.int64)}
    fout.mktree("table", {k: v.dtype for k, v in tab.items()})
    fout["table"].extend(tab)
    names = ["simulated", ">= 1 tracklet", ">= 2 tracklets"] + CUTS
    fout["cutNames"] = ";".join(names)
    meta = {k: np.array([v]) for k, v in vars(args).items() if isinstance(v, (int, float)) and not isinstance(v, bool)}
    meta["stationMask"] = np.array([station_mask], np.int64)
    fout.mktree("meta", {k: v.dtype for k, v in meta.items()})
    fout["meta"].extend(meta)
    fout.close()
    print(f"{'step':34s} {'all':>9s} {'low mass':>9s} {'high mass':>9s}")
    for i, nm in enumerate(names):
        lo = "" if i < 2 else tab["nLow"][i]
        hi = "" if i < 2 else tab["nHigh"][i]
        print(f"{nm:34s} {tab['nAll'][i]:9d} {lo!s:>9s} {hi!s:>9s}")


if __name__ == "__main__":
    main()

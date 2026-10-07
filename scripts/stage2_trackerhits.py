#!/usr/bin/env python3
"""Stage 2: cells -> sublayer clusters -> tracker hits, plus veto summary.

Input: stage-1 cells file(s).  For every event and every tracker station
selected with --stations (default all four: 0 = wall, 1 = 12 cm,
2 = 24 cm, 3 = 36 cm):

1. Clusters.  Cells of one sublayer that touch (shared edge or corner on
   the 1 cm grid) are grouped.  The cluster's reference time is the time of
   its highest-energy cell; cells more than --dt-match from it are removed
   and re-grouped among themselves, so late, unrelated deposits form their
   own clusters.
2. Cluster quantities: summed energy, energy-weighted position, true time
   (earliest cell), measured time (true + Gaussian, --t-smear per
   sublayer), and every contributing particle (energy summed over cells).
3. Clusters with E >= --cluster-emin (MIP candidates) are kept.
4. In each station, phi and z clusters are paired one-to-one, closest
   first, if their closest cells are < --max-dist apart and their measured
   times differ by < --dt-match.
5. Tracker hit: truth position = midpoint of the two cluster centres;
   smeared position = truth moved by Gaussian --pos-smear along the tunnel
   and around the arch; time = mean of the two measured times; truth
   time = mean of the two true times; energy = sum; size of the two clusters
   (cells, widths); unique contributors with their phi and z energies.
   Only the hits are written; the clusters themselves are not stored.

Smearing is reproducible: each event's random stream is seeded from
(--seed, eventID), stored as smearSeed.  Branches, units and conventions
are documented in docs/outputs.md.
"""
import argparse
import glob
import os
import sys
import time

import awkward as ak
import numpy as np
import pandas as pd
import uproot

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import clustering as C  # noqa: E402
import geometry as G  # noqa: E402

# veto elements: stage-1 detector code -> branch suffix
VETO_ELEMENTS = {200: "floor", 210: "wallR", 211: "wallL", 220: "ext1R", 221: "ext1L",
                 230: "ext2R", 231: "ext2L", 240: "ext3R", 241: "ext3L"}
POS_FIELDS = ("x", "y", "z", "xSmear", "ySmear", "zSmear")
TIME_FIELDS = ("t", "tTruth")
SMALL_INT_FIELDS = ("station", "chord", "chanS", "chanPerim", "nCellPhi", "nCellZ", "widthPerimPhi", "widthPerimZ", "widthSPhi", "widthSZ")


def read_chunk(arr):
    """Flatten a chunk of the stage-1 cells tree."""
    n = ak.num(arr["cell_E"]).to_numpy()
    cells = {"ev": np.repeat(np.arange(len(n)), n)}
    for k in ("det", "chanS", "chanPerim", "E", "t", "x", "y", "z"):
        v = ak.flatten(arr["cell_" + k]).to_numpy()
        cells[k] = v.astype(np.int64) if k in ("det", "chanS", "chanPerim") else v.astype(np.float64)
    nc = ak.flatten(arr["cell_nContrib"]).to_numpy().astype(np.int64)
    contrib = {"cell": np.repeat(np.arange(len(nc)), nc),
               "trk": ak.flatten(arr["cell_contrib_trk"]).to_numpy().astype(np.int64),
               "pdg": ak.flatten(arr["cell_contrib_pdg"]).to_numpy().astype(np.int64),
               "E": ak.flatten(arr["cell_contrib_E"]).to_numpy().astype(np.float64)}
    return cells, contrib, len(n)


def time_consistent_clusters(c, dt_cut):
    """Cluster label per cell: touching cells, out-of-time cells split off."""
    n = len(c["E"])
    label = -np.ones(n, np.int64)
    active = np.arange(n)
    offset = 0
    while len(active):
        g = C.touching_groups(c["ev"][active], c["det"][active], c["chanS"][active], c["chanPerim"][active])
        # reference time = time of the highest-energy cell of each group
        tref = C.group_seed_time(g, c["E"][active], c["t"][active])
        ok = np.abs(c["t"][active] - tref) < dt_cut
        label[active[ok]] = g[ok] + offset
        offset += len(active)
        active = active[~ok]
    return label


# ---------------------------------------------------------------------------
# Reproducible, vectorised random numbers.  Each draw is a pure function of
# (event seed, stream, draw index): a splitmix64 hash gives two uniforms and
# Box-Muller turns them into a standard normal.  Results therefore do not
# depend on chunking, job splitting or processing order.
# ---------------------------------------------------------------------------
STREAM_TIME, STREAM_POS_S, STREAM_POS_PERIM = 1, 2, 3


def splitmix64(x):
    x = np.asarray(x, dtype=np.uint64)
    with np.errstate(over="ignore"):
        x = x + np.uint64(0x9E3779B97F4A7C15)
        x = (x ^ (x >> np.uint64(30))) * np.uint64(0xBF58476D1CE4E5B9)
        x = (x ^ (x >> np.uint64(27))) * np.uint64(0x94D049BB133111EB)
    return x ^ (x >> np.uint64(31))


def event_seeds(base, eids):
    """32-bit seed per event from the base seed and the event ID."""
    with np.errstate(over="ignore"):
        k = (np.uint64(base) << np.uint64(32)) ^ np.asarray(eids, dtype=np.uint64)
    return (splitmix64(k) & np.uint64(0xFFFFFFFF)).astype(np.uint32)


def gaussians(seed, stream, index):
    """Standard normals for draws (event seed, stream, index within the event)."""
    with np.errstate(over="ignore"):
        k = ((np.asarray(seed, dtype=np.uint64) << np.uint64(32))
             ^ (np.uint64(stream) << np.uint64(24)) ^ np.asarray(index, dtype=np.uint64))
        h1 = splitmix64(k << np.uint64(1))
        h2 = splitmix64((k << np.uint64(1)) | np.uint64(1))
    u1 = ((h1 >> np.uint64(11)).astype(np.float64) + 0.5) / 2.0**53
    u2 = ((h2 >> np.uint64(11)).astype(np.float64) + 0.5) / 2.0**53
    return np.sqrt(-2.0 * np.log(u1)) * np.cos(2.0 * np.pi * u2)


def rank_in_event(ev):
    """0, 1, 2, ... within each event, for objects sorted by event."""
    counts = np.bincount(ev)
    return np.arange(len(ev)) - np.repeat(np.cumsum(counts) - counts, counts)


def gather(starts, counts, objs):
    """Row indices of objects' contiguous row ranges, and each row's object position."""
    cnt = counts[objs]
    first = np.repeat(np.cumsum(cnt) - cnt, cnt)
    rows = np.repeat(starts[objs], cnt) + (np.arange(int(cnt.sum())) - first)
    return rows, np.repeat(np.arange(len(objs)), cnt)


def process(cells, contrib, nev, eids, args):
    out = {}
    # ---------------- veto summary per element ----------------
    vm = (cells["det"] >= 200) & (cells["E"] > args.veto_emin)
    for code, name in VETO_ELEMENTS.items():
        out["veto_n_" + name] = np.bincount(cells["ev"][vm & (cells["det"] == code)], minlength=nev).astype(np.int32)
    tmin = np.full(nev, np.inf); np.minimum.at(tmin, cells["ev"][vm], cells["t"][vm])
    emax = np.zeros(nev); np.maximum.at(emax, cells["ev"][vm], cells["E"][vm])
    out["veto_tmin"] = np.where(np.isfinite(tmin), tmin, -1.0).astype(np.float32)
    out["veto_maxE"] = emax.astype(np.float32)

    # ---------------- clusters ----------------
    # tracker cells of the selected stations, in stage-1 order
    T = np.where((cells["det"] < 100) & np.isin(cells["det"] // 10, args.stations))[0]
    c = {k: v[T] for k, v in cells.items()}
    lab = time_consistent_clusters(c, args.dt_match)
    # canonical cluster order: by first cell (stage-1 order), hence by event
    ulab, first_cell, inv = np.unique(lab, return_index=True, return_inverse=True)
    order = np.argsort(first_cell, kind="stable")
    cid = np.empty(len(ulab), np.int64); cid[order] = np.arange(len(ulab))
    cell_cl = cid[inv]
    ncl = len(ulab)
    E = np.bincount(cell_cl, c["E"], ncl)
    cl = {"ev": np.zeros(ncl, np.int64), "det": np.zeros(ncl, np.int64), "E": E}
    cl["ev"][cell_cl] = c["ev"]; cl["det"][cell_cl] = c["det"]
    for k in ("x", "y", "z"):
        cl[k] = np.bincount(cell_cl, c["E"] * c[k], ncl) / E
    tt = np.full(ncl, np.inf); np.minimum.at(tt, cell_cl, c["t"]); cl["tTruth"] = tt
    cl["ncell"] = np.bincount(cell_cl, minlength=ncl)
    # channels of each cluster's highest-energy cell
    o = np.lexsort((-c["E"], cell_cl))
    seed = o[np.r_[True, cell_cl[o][1:] != cell_cl[o][:-1]]]
    cl["seedChanS"] = np.empty(ncl, np.int64); cl["seedChanS"][cell_cl[seed]] = c["chanS"][seed]
    cl["seedChanPerim"] = np.empty(ncl, np.int64); cl["seedChanPerim"][cell_cl[seed]] = c["chanPerim"][seed]
    for k, name in (("chanS", "ns"), ("chanPerim", "np")):
        idx = c[k]
        lo = np.full(ncl, np.iinfo(np.int64).max); hi = np.full(ncl, np.iinfo(np.int64).min)
        np.minimum.at(lo, cell_cl, idx); np.maximum.at(hi, cell_cl, idx)
        cl[name] = hi - lo + 1
    # cluster contributors: unique particles, energy summed over the cluster's cells
    tpos = -np.ones(len(cells["E"]), np.int64); tpos[T] = np.arange(len(T))
    rows = tpos[contrib["cell"]] >= 0
    rcl = cell_cl[tpos[contrib["cell"][rows]]]
    key = rcl * 2**31 + contrib["trk"][rows]
    uk, rinv = np.unique(key, return_inverse=True)
    cE = np.bincount(rinv, contrib["E"][rows], len(uk))
    cpdg = np.zeros(len(uk), np.int64); cpdg[rinv] = contrib["pdg"][rows]
    o = np.lexsort((-cE, uk // 2**31))
    ctab = {"cl": (uk // 2**31)[o], "trk": (uk % 2**31)[o], "pdg": cpdg[o], "E": cE[o]}
    ccount = np.bincount(ctab["cl"], minlength=ncl)
    cstart = np.cumsum(ccount) - ccount
    has = ccount > 0
    top = np.minimum(cstart, max(len(ctab["cl"]) - 1, 0))
    cl["trk"] = np.where(has, ctab["trk"][top], 0) if len(ctab["cl"]) else np.zeros(ncl, np.int64)
    cl["pdg"] = np.where(has, ctab["pdg"][top], 0) if len(ctab["cl"]) else np.zeros(ncl, np.int64)
    tot = np.bincount(ctab["cl"], ctab["E"], ncl)
    cl["purity"] = np.where(tot > 0, ctab["E"][top] / np.where(tot > 0, tot, 1), 0.0) if len(ctab["cl"]) else np.zeros(ncl)
    cl["nContrib"] = ccount

    # ---------------- per-event seeds and time smearing ----------------
    seeds = event_seeds(args.seed, eids)
    out["smearSeed"] = seeds
    # one draw per cluster, indexed by its rank in the event's canonical order
    tsm = args.t_smear * gaussians(seeds[cl["ev"]], STREAM_TIME, rank_in_event(cl["ev"]))
    cl["t"] = cl["tTruth"] + tsm

    # ---------------- MIP candidates and phi/z matching ----------------
    keep = np.where(cl["E"] >= args.cluster_emin)[0]          # MIP candidates, canonical order
    # cell ranges per cluster for the closest-cell distance
    corder = np.argsort(cell_cl, kind="stable")
    ce = np.cumsum(cl["ncell"]); cs = ce - cl["ncell"]
    cx, cy, cz = c["x"][corder], c["y"][corder], c["z"][corder]
    st = cl["det"] // 10; sub = cl["det"] % 10
    kp = keep[sub[keep] == 0]; kz = keep[sub[keep] == 1]
    P = pd.DataFrame({"ev": cl["ev"][kp], "st": st[kp], "a": kp}).merge(
        pd.DataFrame({"ev": cl["ev"][kz], "st": st[kz], "b": kz}), on=["ev", "st"])
    pa = P["a"].to_numpy(np.int64); pb = P["b"].to_numpy(np.int64)
    ok = np.abs(cl["t"][pa] - cl["t"][pb]) < args.dt_match
    ok &= sum((cl[k][pa] - cl[k][pb]) ** 2 for k in ("x", "y", "z")) < args.prefilter ** 2
    pa, pb = pa[ok], pb[ok]
    dmin = C.min_cell_distance(pa, pb, cs, ce, cx, cy, cz)
    ok = dmin < args.max_dist
    pa, pb, dmin = pa[ok], pb[ok], dmin[ok]
    o = np.lexsort((pb, pa, dmin, cl["ev"][pa]))
    take = C.greedy_pairs(o, pa, pb, ncl)
    hp, hz, hd = pa[take], pb[take], dmin[take]
    # canonical hit order: event, station, phi cluster
    o = np.lexsort((hp, st[hp], cl["ev"][hp]))
    hp, hz, hd = hp[o], hz[o], hd[o]
    nh = len(hp)

    # ---------------- hits ----------------
    hit = {"ev": cl["ev"][hp], "station": st[hp], "Ephi": cl["E"][hp], "Ez": cl["E"][hz], "dmin": hd,
           "t": 0.5 * (cl["t"][hp] + cl["t"][hz]), "tTruth": 0.5 * (cl["tTruth"][hp] + cl["tTruth"][hz]),
           "trk": cl["trk"][hp], "pdg": cl["pdg"][hp], "trk_z": cl["trk"][hz], "pdg_z": cl["pdg"][hz]}
    for k in ("x", "y", "z"):
        hit[k] = 0.5 * (cl[k][hp] + cl[k][hz])
    # discrete tunnel quantities: segment of the truth position; channel along
    # the axis from the z cluster and around the arch from the phi cluster
    hit["chord"] = G.chord_of(hit["x"], hit["z"]).astype(np.int64) if nh else np.zeros(0, np.int64)
    hit["chanS"] = cl["seedChanS"][hz]
    hit["chanPerim"] = cl["seedChanPerim"][hp]
    # position smearing (--pos-smear) along the tunnel and around the arch
    hcount = np.bincount(hit["ev"], minlength=nev)
    hrank = rank_in_event(hit["ev"]) if nh else np.zeros(0, np.int64)
    ds = args.pos_smear * gaussians(seeds[hit["ev"]], STREAM_POS_S, hrank)
    dp = args.pos_smear * gaussians(seeds[hit["ev"]], STREAM_POS_PERIM, hrank)
    # move the truth point along the local tunnel direction (measured by the
    # z sublayer) and around the arch (measured by the phi sublayer)
    if nh:
        u_s, u_p = G.surface_directions(hit["x"], hit["y"], hit["z"])
        xs = np.stack([hit["x"], hit["y"], hit["z"]], 1) + ds[:, None] * u_s + dp[:, None] * u_p
        hit["xSmear"], hit["ySmear"], hit["zSmear"] = xs[:, 0], xs[:, 1], xs[:, 2]
    else:
        for k in ("xSmear", "ySmear", "zSmear"):
            hit[k] = np.zeros(0)
    # unique contributors per hit with their phi and z energies
    rp, op = gather(cstart, ccount, hp)
    rz, oz = gather(cstart, ccount, hz)
    hid = np.concatenate([op, oz])
    rows = np.concatenate([rp, rz])
    isz = np.concatenate([np.zeros(len(rp), bool), np.ones(len(rz), bool)])
    key = hid * 2**31 + ctab["trk"][rows]
    uk, rinv = np.unique(key, return_inverse=True)
    eP = np.bincount(rinv, np.where(isz, 0.0, ctab["E"][rows]), len(uk))
    eZ = np.bincount(rinv, np.where(isz, ctab["E"][rows], 0.0), len(uk))
    hpdg = np.zeros(len(uk), np.int64); hpdg[rinv] = ctab["pdg"][rows]
    o = np.lexsort((-(eP + eZ), uk // 2**31))
    htab = {"hit": (uk // 2**31)[o], "trk": (uk % 2**31)[o], "pdg": hpdg[o], "Ephi": eP[o], "Ez": eZ[o]}
    hit["nContrib"] = np.bincount(htab["hit"], minlength=nh)

    # ---------------- sizes of the two constituent clusters ----------------
    hit["nCellPhi"] = cl["ncell"][hp]; hit["nCellZ"] = cl["ncell"][hz]
    hit["widthPerimPhi"] = cl["np"][hp]; hit["widthPerimZ"] = cl["np"][hz]
    hit["widthSPhi"] = cl["ns"][hp]; hit["widthSZ"] = cl["ns"][hz]

    # ---------------- assemble jagged output ----------------
    def jag(d, evs, fields, counts):
        r = {}
        for f in fields:
            v = np.asarray(d[f])
            if f in POS_FIELDS:
                v = np.round(v / args.pos_precision) * args.pos_precision
            elif f in TIME_FIELDS:
                v = np.round(v / args.time_precision) * args.time_precision
            if v.dtype.kind == "f":
                v = v.astype(np.float32)
            elif f in SMALL_INT_FIELDS:
                v = np.minimum(v, np.iinfo(np.int16).max).astype(np.int16)
            else:
                v = v.astype(np.int32)
            r[f] = ak.unflatten(v, counts)
        return ak.zip(r)

    out["hit"] = jag(hit, hit["ev"], ("station", "chord", "chanS", "chanPerim", "Ephi", "Ez", "t", "tTruth",
                                      "x", "y", "z", "xSmear", "ySmear", "zSmear", "dmin", "nCellPhi", "nCellZ", "widthPerimPhi", "widthPerimZ",
                                      "widthSPhi", "widthSZ", "trk", "pdg", "trk_z", "pdg_z", "nContrib"),
                     hcount)
    out["hit_contrib"] = jag(htab, None, ("trk", "pdg", "Ephi", "Ez"),
                             np.bincount(hit["ev"][htab["hit"]], minlength=nev) if nh else np.zeros(nev, int))
    return out, len(keep), nh


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("inputs", nargs="+", help="stage-1 cells files (globs allowed)")
    ap.add_argument("--output", required=True)
    ap.add_argument("--stations", default="0,1,2,3",
                    help="tracker stations to form hits in (0 = wall, 1 = 12 cm, 2 = 24 cm, 3 = 36 cm)")
    ap.add_argument("--cluster-emin", type=float, default=1.5, help="MIP threshold per sublayer cluster [MeV]")
    ap.add_argument("--dt-match", type=float, default=3.0,
                    help="timing window [ns]: phi/z matching (measured times) and cell-to-cluster consistency (true times)")
    ap.add_argument("--max-dist", type=float, default=0.025, help="max closest-cell distance phi-z [m]")
    ap.add_argument("--prefilter", type=float, default=1.0, help="max centroid distance considered [m]")
    ap.add_argument("--t-smear", type=float, default=0.7, help="time resolution per sublayer [ns]")
    ap.add_argument("--pos-smear", type=float, default=0.003, help="hit position resolution [m]")
    ap.add_argument("--veto-emin", type=float, default=0.1, help="veto cell threshold [MeV]")
    ap.add_argument("--seed", type=int, default=12345, help="base seed; per-event seed from (seed, eventID)")
    ap.add_argument("--pos-precision", type=float, default=1e-4)
    ap.add_argument("--time-precision", type=float, default=1e-3)
    ap.add_argument("--zstd-level", type=int, default=9)
    ap.add_argument("--step-size", type=int, default=50000)
    args = ap.parse_args()
    args.stations = [int(x) for x in args.stations.split(",")]
    files = sorted(sum((glob.glob(p) for p in args.inputs), []))
    # uproot.recreate does not truncate an existing file (uproot 5.7):
    # remove it first so no stale bytes remain
    if os.path.exists(args.output):
        os.remove(args.output)
    fout = uproot.recreate(args.output, compression=uproot.ZSTD(args.zstd_level))
    first = True
    nproc = nev_tot = ncl_tot = nh_tot = 0
    cell_size = None
    t0 = time.time()
    for fn in files:
        f = uproot.open(fn)
        meta = f["meta"].arrays(library="np")
        nproc += int(meta["nEventsProcessed"][0])
        cell_size = float(meta["trackerCell_m"][0])
        for arr in f["cells"].iterate(step_size=args.step_size, library="ak"):
            cells, contrib, nev = read_chunk(arr)
            eids = arr["eventID"].to_numpy()
            out, nkept, nh = process(cells, contrib, nev, eids, args)
            data = {"eventID": eids.astype(np.int32), **out}
            if first:
                fout.mktree("hits", {k: (v.type.content if isinstance(v, ak.Array) else v.dtype)
                                     for k, v in data.items()})
                first = False
            fout["hits"].extend(data)
            nev_tot += nev; ncl_tot += nkept; nh_tot += nh
            print(f"[stage2] {fn}: {nev_tot} events, {ncl_tot} clusters, {nh_tot} hits, {time.time() - t0:.0f}s",
                  flush=True)
    meta = {"nEventsProcessed": np.array([nproc], np.int64), "nEventsWithCells": np.array([nev_tot], np.int64),
            "nClusters": np.array([ncl_tot], np.int64), "nHits": np.array([nh_tot], np.int64),
            "clusterEmin_MeV": np.array([args.cluster_emin]), "dtMatch_ns": np.array([args.dt_match]),
            "maxDist_m": np.array([args.max_dist]), "tSmear_ns": np.array([args.t_smear]),
            "posSmear_m": np.array([args.pos_smear]), "vetoEmin_MeV": np.array([args.veto_emin]),
            "baseSeed": np.array([args.seed], np.int64),
            "stationMask": np.array([sum(1 << st for st in args.stations)], np.int64), "cell_m": np.array([cell_size]),
            "posPrecision_m": np.array([args.pos_precision]), "timePrecision_ns": np.array([args.time_precision])}
    fout.mktree("meta", {k: v.dtype for k, v in meta.items()})
    fout["meta"].extend(meta)
    fout.close()


if __name__ == "__main__":
    main()

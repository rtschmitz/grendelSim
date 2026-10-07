#!/usr/bin/env python3
"""Stage 1: raw scintillator boundary crossings -> 1 cm energy cells.

Each raw hit is one particle entering or leaving a sensitive volume, with its
kinetic energy at the boundary.  Per event:

1. Crossings (neutrinos excluded) are ordered by detector element, particle
   and time, an exit before an entry at equal times, and paired into
   segments: entry -> exit deposits KE_in - KE_out along the straight
   entry -> exit line; an entry without exit (particle stopped) deposits
   KE_in; an exit without entry (particle created inside) is subtracted from
   its parent's segment in that element (else the nearest segment within
   5 cm).
2. Deposits are spread along their path in 5 mm steps and binned into cells
   of (element, channel along the tunnel, channel around the arch): 1 cm
   for tracker sublayers, 10 cm for vetoes.
3. Cell: summed energy, earliest time, energy-weighted position; kept if
   E > --min-edep.  Every particle with a deposit > --min-contrib-edep in
   the cell is listed with its energy.

Output trees "cells" and "meta"; branches in docs/outputs.md.
"""
import argparse
import os
import sys
import time

import awkward as ak
import numba as nb
import numpy as np
import uproot

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geometry as G  # noqa: E402

NEUTRINOS = (12, -12, 14, -14, 16, -16)
SMALL_INT_FIELDS = ("det", "station", "sublayer", "chord", "chanS", "chanPerim")

def det_code(copy):
    """Detector element: 10*station + sublayer (0 phi, 1 z) for the tracker,
    100 + Geant4 copy number for the vetoes, -1 otherwise."""
    copy = np.asarray(copy)
    out = np.full(copy.shape, -1, dtype=np.int32)
    for st in range(4):
        out[(copy >= G.PHI_BASE[st]) & (copy < G.PHI_BASE[st] + 1000)] = st * 10
        out[copy == G.Z_ID[st]] = st * 10 + 1
    veto = (copy >= 100) & (copy < 200)
    out[veto] = 100 + copy[veto]
    return out


@nb.njit(cache=True)
def _segments(offsets, track, parent, det, ent, ke, t, x, y, z):
    """Deposit segments per event (module docstring, step 1).  Returns event,
    element, deposit, time, start and end points, track ID, raw-hit index."""
    nmax = len(track)
    sev = np.empty(nmax, np.int64)
    sdet = np.empty(nmax, np.int32)
    sde = np.empty(nmax, np.float64)
    st_ = np.empty(nmax, np.float64)
    sx0 = np.empty(nmax, np.float64); sy0 = np.empty(nmax, np.float64); sz0 = np.empty(nmax, np.float64)
    sx1 = np.empty(nmax, np.float64); sy1 = np.empty(nmax, np.float64); sz1 = np.empty(nmax, np.float64)
    strk = np.empty(nmax, np.int32)
    shit = np.empty(nmax, np.int64)
    ns = 0
    nev = len(offsets) - 1
    for ev in range(nev):
        a = offsets[ev]
        b = offsets[ev + 1]
        n = b - a
        if n == 0:
            continue
        # crossings are pre-sorted by (event, element, track, time, exit first):
        # leaving one strip and entering the next share a time
        idx = np.arange(a, b)
        first_seg = ns
        open_i = -1
        for ii in range(n):
            j = idx[ii]
            if open_i >= 0 and (det[j] != det[open_i] or track[j] != track[open_i]):
                # previous track/volume ended without exit: stopped inside
                sev[ns] = ev; sdet[ns] = det[open_i]; sde[ns] = ke[open_i]; st_[ns] = t[open_i]
                sx0[ns] = x[open_i]; sy0[ns] = y[open_i]; sz0[ns] = z[open_i]
                sx1[ns] = x[open_i]; sy1[ns] = y[open_i]; sz1[ns] = z[open_i]
                strk[ns] = track[open_i]; shit[ns] = open_i; ns += 1
                open_i = -1
            if ent[j] == 1:
                if open_i >= 0:
                    # re-entry without recorded exit: close previous as stopped
                    sev[ns] = ev; sdet[ns] = det[open_i]; sde[ns] = ke[open_i]; st_[ns] = t[open_i]
                    sx0[ns] = x[open_i]; sy0[ns] = y[open_i]; sz0[ns] = z[open_i]
                    sx1[ns] = x[open_i]; sy1[ns] = y[open_i]; sz1[ns] = z[open_i]
                    strk[ns] = track[open_i]; shit[ns] = open_i; ns += 1
                open_i = j
            else:
                if open_i >= 0:
                    sev[ns] = ev; sdet[ns] = det[j]; sde[ns] = ke[open_i] - ke[j]; st_[ns] = t[open_i]
                    sx0[ns] = x[open_i]; sy0[ns] = y[open_i]; sz0[ns] = z[open_i]
                    sx1[ns] = x[j]; sy1[ns] = y[j]; sz1[ns] = z[j]
                    strk[ns] = track[j]; shit[ns] = j; ns += 1
                    open_i = -1
                else:
                    # created inside: negative point deposit (attached below)
                    sev[ns] = ev; sdet[ns] = det[j]; sde[ns] = -ke[j]; st_[ns] = t[j]
                    sx0[ns] = x[j]; sy0[ns] = y[j]; sz0[ns] = z[j]
                    sx1[ns] = x[j]; sy1[ns] = y[j]; sz1[ns] = z[j]
                    strk[ns] = -track[j] - 1  # mark as created-inside
                    shit[ns] = j; ns += 1
        if open_i >= 0:
            sev[ns] = ev; sdet[ns] = det[open_i]; sde[ns] = ke[open_i]; st_[ns] = t[open_i]
            sx0[ns] = x[open_i]; sy0[ns] = y[open_i]; sz0[ns] = z[open_i]
            sx1[ns] = x[open_i]; sy1[ns] = y[open_i]; sz1[ns] = z[open_i]
            strk[ns] = track[open_i]; shit[ns] = open_i; ns += 1
        # attach created-inside negatives to the parent's segment in the same
        # volume (closest in space); otherwise to the closest segment in the
        # same volume within 5 cm; otherwise keep as is.
        for s in range(first_seg, ns):
            if strk[s] >= 0:
                continue
            j = shit[s]
            par = parent[j]
            best = -1
            bestd = 1e30
            bestp = False
            for q in range(first_seg, ns):
                if q == s or strk[q] < 0 or sdet[q] != sdet[s]:
                    continue
                mx = 0.5 * (sx0[q] + sx1[q]); my = 0.5 * (sy0[q] + sy1[q]); mz = 0.5 * (sz0[q] + sz1[q])
                d = (mx - sx0[s]) ** 2 + (my - sy0[s]) ** 2 + (mz - sz0[s]) ** 2
                isp = strk[q] == par
                if isp and (not bestp or d < bestd):
                    best = q; bestd = d; bestp = True
                elif (not bestp) and d < bestd:
                    best = q; bestd = d
            if best >= 0 and (bestp or bestd < 0.05 ** 2):
                sde[best] += sde[s]
                sde[s] = 0.0
            strk[s] = -strk[s] - 1
    return (sev[:ns], sdet[:ns], sde[:ns], st_[:ns], sx0[:ns], sy0[:ns], sz0[:ns],
            sx1[:ns], sy1[:ns], sz1[:ns], strk[:ns], shit[:ns])


def consolidate(arrays, step=0.005, tracker_cell=0.01, veto_cell=0.10, min_edep=0.0, min_contrib=0.0):
    copy = arrays["scint_copyNo"]
    counts = ak.num(copy).to_numpy()
    offsets = np.concatenate([[0], np.cumsum(counts)]).astype(np.int64)
    f = lambda k: ak.flatten(arrays[k]).to_numpy()
    cp = f("scint_copyNo"); pdg = f("scint_pdgID")
    det = det_code(cp)
    keep = (det >= 0) & ~np.isin(pdg, NEUTRINOS)
    # rebuild offsets after masking
    evidx = np.repeat(np.arange(len(counts)), counts)
    evk = evidx[keep]
    offsets = np.concatenate([[0], np.cumsum(np.bincount(evk, minlength=len(counts)))]).astype(np.int64)
    trk = f("scint_trackID")[keep]; par = f("scint_parentID")[keep]
    ent = f("scint_isEntering")[keep]; ke = f("scint_kineticEnergy_MeV")[keep].astype(np.float64)
    t = f("scint_time_ns")[keep].astype(np.float64)
    x = f("scint_x_m")[keep].astype(np.float64); y = f("scint_y_m")[keep].astype(np.float64)
    z = f("scint_z_m")[keep].astype(np.float64)
    pdgk = pdg[keep]; detk = det[keep]
    # order crossings by (event, det, track, time, exits before entries)
    o = np.lexsort((ent, t, trk.astype(np.int64), detk.astype(np.int64), evk))
    trk, par, ent, ke, t, x, y, z, pdgk, detk = (v[o] for v in (trk, par, ent, ke, t, x, y, z, pdgk, detk))
    (sev, sdet, sde, st, x0, y0, z0, x1, y1, z1, strk, shit) = _segments(
        offsets, trk, par, detk, ent, ke, t, x, y, z)
    m = sde != 0.0
    sev, sdet, sde, st, x0, y0, z0, x1, y1, z1, strk, shit = (
        v[m] for v in (sev, sdet, sde, st, x0, y0, z0, x1, y1, z1, strk, shit))
    spdg = pdgk[shit]
    # spread each segment along its path in sub-steps of <= `step`
    L = np.sqrt((x1 - x0) ** 2 + (y1 - y0) ** 2 + (z1 - z0) ** 2)
    nsub = np.maximum(1, np.ceil(L / step).astype(np.int64))
    rep = np.repeat(np.arange(len(L)), nsub)
    kk = np.arange(len(rep)) - np.repeat(np.cumsum(nsub) - nsub, nsub)
    frac = (kk + 0.5) / nsub[rep]
    px = x0[rep] + frac * (x1 - x0)[rep]
    py = y0[rep] + frac * (y1 - y0)[rep]
    pz = z0[rep] + frac * (z1 - z0)[rep]
    pe = sde[rep] / nsub[rep]
    pt = st[rep]
    pdet = sdet[rep]
    pev = sev[rep]
    ptrk = strk[rep]; ppdg = spdg[rep]
    chord, s, lx, ly = G.to_local(px, py, pz)
    perim, depth = G.perimeter_coords(lx, ly)
    is_trk = pdet < 100
    cell = np.where(is_trk, tracker_cell, veto_cell)
    # veto: use the lateral coordinate appropriate to the element
    lat = np.where(is_trk, perim, np.where(pdet == 200, lx, ly))
    i_s = np.floor(s / cell).astype(np.int64)
    i_p = np.floor((lat + 10.0) / cell).astype(np.int64)
    key = ((pev.astype(np.int64) * 512 + pdet) * 200000 + i_s) * 4096 + i_p
    uk, inv = np.unique(key, return_inverse=True)
    nc = len(uk)
    E = np.bincount(inv, weights=pe, minlength=nc)
    wpos = np.where(pe > 0, pe, 0.0)
    W = np.bincount(inv, weights=wpos, minlength=nc)
    Wsafe = np.where(W > 0, W, 1.0)
    cx = np.bincount(inv, weights=wpos * px, minlength=nc) / Wsafe
    cy = np.bincount(inv, weights=wpos * py, minlength=nc) / Wsafe
    cz = np.bincount(inv, weights=wpos * pz, minlength=nc) / Wsafe
    tmin = np.full(nc, np.inf)
    np.minimum.at(tmin, inv, pt)
    # dominant contributor (truth) per cell: max positive deposit by track
    tk = np.unique(inv.astype(np.int64) * 2**31 + ptrk.astype(np.int64), return_inverse=True)
    trE = np.bincount(tk[1], weights=wpos, minlength=len(tk[0]))
    trCell = tk[0] // 2**31
    trID = (tk[0] % 2**31).astype(np.int32)
    o = np.lexsort((-trE, trCell))
    firsts = o[np.concatenate([[True], trCell[o][1:] != trCell[o][:-1]])]
    # PDG ID of each (cell, track): a track has a single PDG ID
    tr_pdg = np.zeros(len(trE), np.int32); tr_pdg[tk[1]] = ppdg
    dom_trk = np.zeros(nc, np.int32); dom_trk[trCell[firsts]] = trID[firsts]
    dom_pdg = np.zeros(nc, np.int32); dom_pdg[trCell[firsts]] = tr_pdg[firsts]
    ev_of_cell = (uk // 4096 // 200000 // 512)
    det_of_cell = ((uk // 4096 // 200000) % 512).astype(np.int32)
    good = E > min_edep
    # discrete tunnel quantities: channel numbers along the tunnel axis and
    # around the arch (veto: across the floor / up the wall), tracker layer,
    # sublayer and tunnel segment (chord)
    det_g = det_of_cell[good]
    csize = np.where(det_g < 100, tracker_cell, veto_cell)
    out = dict(chanS=((uk // 4096) % 200000)[good],
               chanPerim=(uk % 4096)[good] - np.round(10.0 / csize).astype(np.int64),
               station=np.where(det_g < 100, det_g // 10, -1),
               sublayer=np.where(det_g < 100, det_g % 10, -1),
               chord=G.chord_of(cx[good], cz[good]).astype(np.int64))
    out.update(ev=ev_of_cell[good], det=det_g, E=E[good], t=tmin[good],
               x=cx[good], y=cy[good], z=cz[good], trk=dom_trk[good], pdg=dom_pdg[good])
    # every contributing particle per cell: the energy that track deposited in
    # the cell, kept only if strictly positive (> min_contrib).  A particle
    # crossing with identical entry and exit energy has zero deposit (its
    # segment is dropped before binning) and is never listed.  Ordered by
    # cell, then by decreasing energy.
    keep = good[trCell] & (trE > min_contrib)
    newidx = np.cumsum(good) - 1
    o2 = np.lexsort((-trE, trCell))
    o2 = o2[keep[o2]]
    out["c_cell"] = newidx[trCell[o2]]
    out["c_trk"] = trID[o2]
    out["c_pdg"] = tr_pdg[o2]
    out["c_E"] = trE[o2]
    out["nContrib"] = np.bincount(out["c_cell"], minlength=int(good.sum())).astype(np.int32)
    return out, len(cp)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("input")
    ap.add_argument("output")
    ap.add_argument("--tree", default=None, help="tree name (default: highest Events cycle)")
    ap.add_argument("--entry-start", type=int, default=0)
    ap.add_argument("--entry-stop", type=int, default=None)
    ap.add_argument("--step-size", type=int, default=50000)
    ap.add_argument("--tracker-cell", type=float, default=0.01, help="tracker cell size [m]")
    ap.add_argument("--min-edep", type=float, default=1e-3, help="keep cells with E > this [MeV]")
    ap.add_argument("--min-contrib-edep", type=float, default=0.0,
                    help="list a particle as a cell contributor if its deposit there is > this [MeV]")
    ap.add_argument("--veto-cell", type=float, default=0.10, help="veto cell size [m]")
    ap.add_argument("--pos-precision", type=float, default=1e-4,
                    help="positions are rounded to this [m] before writing (default 0.1 mm)")
    ap.add_argument("--time-precision", type=float, default=1e-3,
                    help="times are rounded to this [ns] before writing (default 1 ps)")
    ap.add_argument("--zstd-level", type=int, default=9, help="ZSTD compression level of the output")
    args = ap.parse_args()

    fin = uproot.open(args.input)
    tname = args.tree or max((k for k in fin.keys() if k.startswith("Events")),
                             key=lambda k: int(k.split(";")[1]))
    tree = fin[tname]
    branches = ["eventID", "scint_trackID", "scint_parentID", "scint_copyNo", "scint_pdgID",
                "scint_isEntering", "scint_kineticEnergy_MeV", "scint_time_ns",
                "scint_x_m", "scint_y_m", "scint_z_m"]
    stop = args.entry_stop or tree.num_entries
    # uproot.recreate does not truncate an existing file (uproot 5.7):
    # remove it first so no stale bytes remain
    if os.path.exists(args.output):
        os.remove(args.output)
    fout = uproot.recreate(args.output, compression=uproot.ZSTD(args.zstd_level))
    first = True
    ntot = nwritten = nraw = ncell = 0
    t0 = time.time()
    for arrays in tree.iterate(branches, entry_start=args.entry_start, entry_stop=stop,
                               step_size=args.step_size, library="ak"):
        nev = len(arrays)
        nonempty = ak.num(arrays["scint_copyNo"]) > 0
        sub = arrays[nonempty]
        if len(sub):
            cells, nr = consolidate(sub, tracker_cell=args.tracker_cell, veto_cell=args.veto_cell,
                                    min_edep=args.min_edep, min_contrib=args.min_contrib_edep)
        else:
            cells, nr = {k: np.zeros(0) for k in ("ev",)}, 0
        nraw += nr
        counts = np.bincount(cells["ev"].astype(np.int64), minlength=len(sub)) if len(sub) else np.zeros(0, int)
        has = counts > 0
        evid = sub["eventID"].to_numpy()[has]
        nhits_raw = ak.num(sub["scint_copyNo"]).to_numpy()[has]
        jag = {}
        if len(sub):
            # cells come out of consolidate() already ordered by event
            assert np.all(np.diff(cells["ev"]) >= 0)
            # store at physics-relevant precision (far below the detector
            # resolution) so the values compress well; energies are exact
            for k in ("det", "station", "sublayer", "chord", "chanS", "chanPerim",
                      "E", "t", "x", "y", "z", "trk", "pdg", "nContrib"):
                arr = cells[k]
                if k in ("x", "y", "z"):
                    arr = np.round(arr / args.pos_precision) * args.pos_precision
                elif k == "t":
                    arr = np.round(arr / args.time_precision) * args.time_precision
                if arr.dtype == np.float64:
                    arr = arr.astype(np.float32)
                elif k in SMALL_INT_FIELDS:
                    arr = arr.astype(np.int16)
                jag[k] = ak.unflatten(arr, counts[has])
            ccounts = np.bincount(cells["ev"][cells["c_cell"]].astype(np.int64), minlength=len(sub))[has]
            cjag = {}
            for k in ("trk", "pdg", "E"):
                arr = cells["c_" + k]
                arr = arr.astype(np.float32) if arr.dtype == np.float64 else arr.astype(np.int32)
                cjag[k] = ak.unflatten(arr, ccounts)
        if len(evid):
            # a classic TTree (readable by any ROOT version); the record
            # fields become branches cell_<field> sharing the counter "ncell",
            # and cell_contrib_<field> sharing "ncell_contrib"
            data = {"eventID": evid.astype(np.int32), "nRawHits": nhits_raw.astype(np.int32),
                    "cell": ak.zip(jag), "cell_contrib": ak.zip(cjag)}
            if first:
                fout.mktree("cells", {k: (v.type.content if isinstance(v, ak.Array) else v.dtype)
                                      for k, v in data.items()})
                first = False
            fout["cells"].extend(data)
        ntot += nev
        nwritten += len(evid)
        ncell += int(counts.sum()) if len(sub) else 0
        print(f"[stage1] {ntot}/{stop - args.entry_start} events, kept {nwritten}, "
              f"raw hits {nraw}, cells {ncell}, {time.time() - t0:.0f}s", flush=True)
    if first:  # no event had a deposit: still write an (empty) cells tree
        fout.mktree("cells", {"eventID": "int32", "nRawHits": "int32"})
    meta = {"nEventsProcessed": np.array([ntot], np.int64), "nRawHits": np.array([nraw], np.int64),
            "nCells": np.array([ncell], np.int64),
            "trackerCell_m": np.array([args.tracker_cell]), "vetoCell_m": np.array([args.veto_cell]),
            "minEdep_MeV": np.array([args.min_edep]), "minContribEdep_MeV": np.array([args.min_contrib_edep]),
            "posPrecision_m": np.array([args.pos_precision]),
            "timePrecision_ns": np.array([args.time_precision])}
    fout.mktree("meta", {k: v.dtype for k, v in meta.items()})
    fout["meta"].extend(meta)
    fout.close()


if __name__ == "__main__":
    main()

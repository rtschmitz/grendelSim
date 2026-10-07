#!/usr/bin/env python3
"""Stage 4: tracklets -> best vertex per event (DCA vertexing).

Events of the stage-3 file with at least two tracklets are kept.  Every
pair of their tracklets is a vertex candidate; one vertex is kept per event,
the best by
  1. its two tracklets share no hits (if any pair qualifies),
  2. then inside the tunnel (dWall > 0) with the smallest DCA,
  3. else the smallest DCA anywhere.
vtx_nCand gives the number of pairs considered.  For the chosen pair:

* DCA: closest approach of the two fitted lines; vertex = midpoint.
* Position: tunnel segment, signed distance to the inner surface of the
  innermost tracker station used for tracking (dInner > 0 inside the
  fiducial region: inside that station on the arch / upper walls, inside the
  matching wall extensions below mid-wall height, above the floor
  scintillator, within the tunnel ends) and to the nominal tunnel surface
  (dWall > 0 inside the tunnel).
* Decay side: position of the vertex along each tracklet's outward
  direction relative to its innermost hit (<= 0: the vertex lies inward of
  the hits); decaySide = both <= 0.
* Timing: emission time at the vertex of each tracklet, averaged over its
  hits (t_hit - |hit - vertex| / c), their difference, and the chi2 of all
  hits of both tracklets w.r.t. their common mean (sigma = --t-sigma).
* Geometry: opening angle of the outward directions, separations of the two
  tracklets' hits at their innermost and outermost common station, and
  collinearity (max residual of all their hits to one line).
* Pointing: angle between the summed outward directions and the IP -> vertex
  direction (IP estimate: geometry.IP), and the distance to the IP.
* Bookkeeping: the two tracklets' indices, number of hits they share, truth
  (same dominant particle).

Output: all stage-3 content of every event plus the vtx_* branches (one
value per event).  Hit positions and times are the ones used for tracking
(smeared unless the stage-3 file was made with --truth).
"""
import argparse
import glob
import os
import sys
import time

import awkward as ak
import numba as nb
import numpy as np
import uproot

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geometry as G  # noqa: E402

C_M_NS = 0.299792458
MAXST = 4


@nb.njit(cache=True)
def _maxres(x, y, z, idx, n):
    mx = 0.0; my = 0.0; mz = 0.0
    for j in range(n):
        mx += x[idx[j]]; my += y[idx[j]]; mz += z[idx[j]]
    mx /= n; my /= n; mz /= n
    M = np.zeros((3, 3))
    for j in range(n):
        i = idx[j]
        v = np.array([x[i] - mx, y[i] - my, z[i] - mz])
        M += np.outer(v, v)
    w, V = np.linalg.eigh(M)
    d = V[:, 2]
    r = 0.0
    for j in range(n):
        i = idx[j]
        v0 = x[i] - mx; v1 = y[i] - my; v2 = z[i] - mz
        pr = v0 * d[0] + v1 * d[1] + v2 * d[2]
        r = max(r, v0 * v0 + v1 * v1 + v2 * v2 - pr * pr)
    return np.sqrt(max(r, 0.0))


@nb.njit(cache=True)
def vertex_pairs(tl_first, tl_n, tl_hits, tl_line, tl_truth, x, y, z, t, ip, t_sigma):
    """All tracklet pairs of every event.  tl_first/tl_n: tracklet range per
    event; tl_hits: global hit index per station (-1 none); tl_line: centroid
    and outward unit direction.  Returns pair indices (local) and values."""
    npair = 0
    for e in range(len(tl_n)):
        npair += tl_n[e] * (tl_n[e] - 1) // 2
    ev = np.empty(npair, np.int64)
    ia = np.empty(npair, np.int64)
    ib = np.empty(npair, np.int64)
    out = np.zeros((npair, 20))
    idx = np.empty(2 * MAXST, np.int64)
    p = 0
    for e in range(len(tl_n)):
        f = tl_first[e]
        for i in range(f, f + tl_n[e]):
            for j in range(i + 1, f + tl_n[e]):
                c1 = tl_line[i, 0:3]; d1 = tl_line[i, 3:6]
                c2 = tl_line[j, 0:3]; d2 = tl_line[j, 3:6]
                w0 = c1 - c2
                b = d1[0] * d2[0] + d1[1] * d2[1] + d1[2] * d2[2]
                dd = d1[0] * w0[0] + d1[1] * w0[1] + d1[2] * w0[2]
                ee = d2[0] * w0[0] + d2[1] * w0[1] + d2[2] * w0[2]
                den = 1.0 - b * b
                if den < 1e-12:
                    s1 = 0.0; s2 = ee
                else:
                    s1 = (b * ee - dd) / den
                    s2 = (ee - b * dd) / den
                q1 = c1 + s1 * d1
                q2 = c2 + s2 * d2
                V = 0.5 * (q1 + q2)
                dca = np.sqrt(((q1 - q2) ** 2).sum())
                # decay side: vertex position along each outward direction
                # relative to the tracklet's innermost hit
                Ls = np.zeros(2)
                for k, tl in enumerate((i, j)):
                    inner = -1
                    for s in range(MAXST - 1, -1, -1):
                        if tl_hits[tl, s] >= 0:
                            inner = tl_hits[tl, s]
                            break
                    d = tl_line[tl, 3:6]
                    Ls[k] = (V[0] - x[inner]) * d[0] + (V[1] - y[inner]) * d[1] + (V[2] - z[inner]) * d[2]
                # emission times and timing chi2
                te = np.zeros(2)
                nh = np.zeros(2)
                for k, tl in enumerate((i, j)):
                    for s in range(MAXST):
                        h = tl_hits[tl, s]
                        if h >= 0:
                            dist = np.sqrt((x[h] - V[0]) ** 2 + (y[h] - V[1]) ** 2 + (z[h] - V[2]) ** 2)
                            te[k] += t[h] - dist / C_M_NS
                            nh[k] += 1
                tall = (te[0] + te[1]) / (nh[0] + nh[1])
                chi = 0.0
                for tl in (i, j):
                    for s in range(MAXST):
                        h = tl_hits[tl, s]
                        if h >= 0:
                            dist = np.sqrt((x[h] - V[0]) ** 2 + (y[h] - V[1]) ** 2 + (z[h] - V[2]) ** 2)
                            chi += ((t[h] - dist / C_M_NS - tall) / t_sigma) ** 2
                te /= nh
                # separations at the innermost / outermost common station
                sep_in = -1.0; sep_out = -1.0
                for s in range(MAXST):
                    h1 = tl_hits[i, s]; h2 = tl_hits[j, s]
                    if h1 >= 0 and h2 >= 0:
                        dsep = np.sqrt((x[h1] - x[h2]) ** 2 + (y[h1] - y[h2]) ** 2 + (z[h1] - z[h2]) ** 2)
                        if sep_out < 0:
                            sep_out = dsep
                        sep_in = dsep
                # shared hits and collinearity of all distinct hits
                m = 0
                nshared = 0
                for s in range(MAXST):
                    h1 = tl_hits[i, s]; h2 = tl_hits[j, s]
                    if h1 >= 0:
                        idx[m] = h1; m += 1
                    if h2 >= 0:
                        if h2 == h1:
                            nshared += 1
                        else:
                            idx[m] = h2; m += 1
                coll = _maxres(x, y, z, idx, m) if m >= 3 else 0.0
                # opening angle and pointing
                cosop = min(1.0, max(-1.0, b))
                sd = d1 + d2
                los = V - ip
                nsd = np.sqrt((sd ** 2).sum()); nlos = np.sqrt((los ** 2).sum())
                cpt = (sd * los).sum() / (nsd * nlos) if nsd > 0 and nlos > 0 else 1.0
                ev[p] = e; ia[p] = i - f; ib[p] = j - f
                out[p, 0] = dca; out[p, 1] = V[0]; out[p, 2] = V[1]; out[p, 3] = V[2]
                out[p, 4] = Ls[0]; out[p, 5] = Ls[1]
                out[p, 6] = te[0]; out[p, 7] = te[1]; out[p, 8] = chi
                out[p, 9] = np.arccos(cosop); out[p, 10] = sep_in; out[p, 11] = sep_out
                out[p, 12] = coll; out[p, 13] = np.arccos(min(1.0, max(-1.0, cpt))); out[p, 14] = nlos
                out[p, 15] = nshared; out[p, 16] = 1.0 if tl_truth[i] == tl_truth[j] else 0.0
                out[p, 17] = nh[0] + nh[1]
                p += 1
    return ev, ia, ib, out


def boundary_distances(x, y, z, inner_depth):
    """Signed distances (> 0 inside) to the inner tracker surface and to the
    nominal tunnel surface, plus the tunnel segment."""
    tc = G.tunnel_coordinates(x, y, z)
    upper = tc["ly"] >= G.MID_WALL_Y
    floor_top = G.FLOOR_Y + G.WALL_GAP + 0.02
    d_in = np.where(upper, tc["depth"] - inner_depth,
                    np.minimum(G.HALF_FLOOR - inner_depth - np.abs(tc["lx"]), tc["ly"] - floor_top))
    d_wall = np.where(upper, tc["depth"], np.minimum(G.HALF_FLOOR - np.abs(tc["lx"]), tc["ly"] - G.FLOOR_Y))
    ends = np.minimum(tc["s"], G.TUNNEL_LENGTH - tc["s"])
    return np.minimum(d_in, ends), np.minimum(d_wall, ends), tc["chord"]


ROUND = {"dca": 1e-5, "x": 1e-4, "y": 1e-4, "z": 1e-4, "L1": 1e-4, "L2": 1e-4, "t1": 1e-3, "t2": 1e-3,
         "dt": 1e-3, "tChi2": 1e-3, "openAngle": 1e-5, "sepInner": 1e-4, "sepOuter": 1e-4, "collMax": 1e-5,
         "pointAngle": 1e-5, "distIP": 1e-4, "dInner": 1e-4, "dWall": 1e-4}


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("inputs", nargs="+", help="stage-3 tracklet files (globs allowed)")
    ap.add_argument("--output", required=True)
    ap.add_argument("--t-sigma", type=float, default=0.5, help="time resolution per hit [ns]")
    ap.add_argument("--zstd-level", type=int, default=9)
    ap.add_argument("--step-size", type=int, default=50000)
    args = ap.parse_args()
    files = sorted(sum((glob.glob(p) for p in args.inputs), []))
    if os.path.exists(args.output):
        os.remove(args.output)
    fout = uproot.recreate(args.output, compression=uproot.ZSTD(args.zstd_level))
    first = True
    nproc = nin = nev_tot = nvtx_tot = 0
    t0 = time.time()
    for fn in files:
        f = uproot.open(fn)
        m = f["meta"].arrays(library="np")
        nproc += int(m["nEventsProcessed"][0])
        truth = bool(m["usesTruth"][0])
        stations = [s for s in range(MAXST) if int(m["stationMask"][0]) >> s & 1]
        inner_depth = G.station_phi_depth(max(stations)) + G.TRACKER_T
        tree = f["tracklets"]
        keys = tree.keys()
        for arr in tree.iterate(keys, step_size=args.step_size, library="ak"):
            nin += len(arr)
            arr = arr[arr["ntrkl"] >= 2]          # a vertex needs two tracklets
            nev = len(arr)
            if nev == 0:
                continue
            fl = lambda k: ak.flatten(arr[k]).to_numpy()
            pk = ("hit_x", "hit_y", "hit_z") if truth else ("hit_xSmear", "hit_ySmear", "hit_zSmear")
            x, y, z = (fl(k).astype(np.float64) for k in pk)
            t = fl("hit_tTruth" if truth else "hit_t").astype(np.float64)
            st = fl("hit_station").astype(np.int64)
            hoff = np.concatenate([[0], np.cumsum(arr["nhit"].to_numpy())])[:-1]
            ntl = arr["ntrkl"].to_numpy().astype(np.int64)
            tl_first = np.cumsum(ntl) - ntl
            tl_ev = np.repeat(np.arange(nev), ntl)
            nh = fl("trkl_nHits").astype(np.int64)
            gh = fl("trkl_hit_idx").astype(np.int64) + np.repeat(hoff[tl_ev], nh)
            tl_hits = np.full((len(nh), MAXST), -1, np.int64)
            tl_hits[np.repeat(np.arange(len(nh)), nh), st[gh]] = gh
            line = np.stack([fl("trkl_" + k).astype(np.float64) for k in ("x", "y", "z", "dx", "dy", "dz")], 1)
            # re-normalise the stored (rounded) directions
            line[:, 3:6] /= np.linalg.norm(line[:, 3:6], axis=1)[:, None]
            pev, pa, pb, P = vertex_pairs(tl_first, ntl, tl_hits, line, fl("trkl_truthTrk").astype(np.int64),
                                          x, y, z, t, G.IP.astype(np.float64), args.t_sigma)
            d_in, d_wall, chord = boundary_distances(P[:, 1], P[:, 2], P[:, 3], inner_depth)
            vtx = {"i1": pa, "i2": pb, "dca": P[:, 0], "x": P[:, 1], "y": P[:, 2], "z": P[:, 3],
                   "chord": chord, "dInner": d_in, "dWall": d_wall, "L1": P[:, 4], "L2": P[:, 5],
                   "decaySide": ((P[:, 4] <= 0) & (P[:, 5] <= 0)).astype(np.int64),
                   "t1": P[:, 6], "t2": P[:, 7], "dt": P[:, 6] - P[:, 7], "tChi2": P[:, 8],
                   "nHits": P[:, 17], "openAngle": P[:, 9], "sepInner": P[:, 10], "sepOuter": P[:, 11],
                   "collMax": P[:, 12], "pointAngle": P[:, 13], "distIP": P[:, 14], "nShared": P[:, 15],
                   "sameTruth": P[:, 16]}
            # best vertex per event: tracklets sharing no hits first, then
            # inside the tunnel with the smallest DCA, else the smallest DCA
            ncand = np.bincount(pev, minlength=nev)
            o = np.lexsort((vtx["dca"], vtx["dWall"] <= 0, vtx["nShared"] > 0, pev))
            best = o[np.r_[True, pev[o][1:] != pev[o][:-1]]]
            assert np.array_equal(pev[best], np.arange(nev))
            rec = {"nCand": ncand.astype(np.int32)}
            for k, v in vtx.items():
                v = np.asarray(v)[best]
                if k in ROUND:
                    v = (np.round(v / ROUND[k]) * ROUND[k]).astype(np.float32)
                else:
                    v = v.astype(np.int16)
                rec[k] = v
            data = {k: arr[k].to_numpy() for k in keys if not k.startswith(("hit_", "nhit", "trkl_", "ntrkl"))}
            data["hit"] = ak.zip({k[4:]: arr[k] for k in keys if k.startswith("hit_") and not k.startswith("hit_contrib_")})
            data["hit_contrib"] = ak.zip({k[12:]: arr[k] for k in keys if k.startswith("hit_contrib_")})
            data["trkl"] = ak.zip({k[5:]: arr[k] for k in keys if k.startswith("trkl_") and not k.startswith("trkl_hit_")})
            data["trkl_hit"] = ak.zip({k[9:]: arr[k] for k in keys if k.startswith("trkl_hit_")})
            data.update({"vtx_" + k: v for k, v in rec.items()})
            if first:
                fout.mktree("vertices", {k: (v.type.content if isinstance(v, ak.Array) else v.dtype)
                                         for k, v in data.items()})
                first = False
            fout["vertices"].extend(data)
            nev_tot += nev; nvtx_tot += len(pev)
            print(f"[stage4] {fn}: {nev_tot} events, {nvtx_tot} candidate pairs, {time.time() - t0:.0f}s", flush=True)
    meta = {"nEventsProcessed": np.array([nproc], np.int64), "nEventsIn": np.array([nin], np.int64),
            "nEvents": np.array([nev_tot], np.int64),
            "nCandidatePairs": np.array([nvtx_tot], np.int64), "tSigma_ns": np.array([args.t_sigma]),
            "ip_x_m": np.array([G.IP[0]]), "ip_y_m": np.array([G.IP[1]]), "ip_z_m": np.array([G.IP[2]]),
            "stationMask": np.array([int(m["stationMask"][0])], np.int64),
            "usesTruth": np.array([int(truth)], np.int64)}
    fout.mktree("meta", {k: v.dtype for k, v in meta.items()})
    fout["meta"].extend(meta)
    fout.close()


if __name__ == "__main__":
    main()

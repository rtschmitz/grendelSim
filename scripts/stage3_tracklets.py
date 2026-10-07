#!/usr/bin/env python3
"""Stage 3: tracker hits -> tracklets.

A tracklet is one straight crossing of the tracker layers on one side of the
tunnel: at most one hit per station, no hit shared between tracklets.
For every event of the stage-2 hits file:

1. Seeds: pairs of hits in different stations (of --stations) whose
   crossing angle w.r.t. the layer normal is below --max-angle (i.e. hit
   distance <= station depth gap / cos(max-angle)) and whose times fit a
   particle at the speed of light: | |dt| - d/c | < --dt-window.
   The segment between them must stay in the tracker band: its midpoint is
   at most --max-excursion deeper than the deeper station (one crossing on
   one side, not a path through the open tunnel).
2. Extension: in every other station the hit closest to the seed line
   within --hit-tol is added, if its time also fits and it passes the same
   angle and band conditions w.r.t. both seed hits.
3. Fit: straight line through the hits (principal axis); the candidate is
   kept if every hit is within --hit-tol of it and it has >= --min-hits.
4. Selection: candidates whose hits all belong to another candidate are
   dropped.  Among candidates competing for the same hits, the set of
   non-overlapping tracklets is chosen that covers the most hits, then uses
   the fewest tracklets, then has the smallest total length (e.g. two outer
   and two inner hits give the two uncrossed tracklets).  A hit left
   without a tracklet may still get one from the best remaining candidate
   containing it, even if that shares hits (trkl_primary = 0).
5. Per tracklet: line (centroid, unit direction pointing from the innermost
   to the outermost station), time fit t = t0 + invBeta * L / c along that
   direction (invBeta > 0: moving outward), chi2 of the outward and inward
   speed-of-light hypotheses, the 'outgoing' flag (chi2Out < chi2In), the
   best collinear partner tracklet sharing no hits (through-going relation,
   common-line residual < --merge-tol) with its residual and implied 1/beta,
   the number of hits shared with other tracklets and of tracklets it shares
   hits with (conflicts), and truth.

Events with >= --min-tracklets (default 1) tracklets are written with all their
stage-2 content.  Branches: docs/outputs.md.
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


# ---------------------------------------------------------------------------
# numba kernels
# ---------------------------------------------------------------------------
@nb.njit(cache=True)
def _line_fit(x, y, z, idx, n):
    mx = 0.0; my = 0.0; mz = 0.0
    for j in range(n):
        i = idx[j]
        mx += x[i]; my += y[i]; mz += z[i]
    mx /= n; my /= n; mz /= n
    M = np.zeros((3, 3))
    for j in range(n):
        i = idx[j]
        v0 = x[i] - mx; v1 = y[i] - my; v2 = z[i] - mz
        M[0, 0] += v0 * v0; M[0, 1] += v0 * v1; M[0, 2] += v0 * v2
        M[1, 1] += v1 * v1; M[1, 2] += v1 * v2; M[2, 2] += v2 * v2
    M[1, 0] = M[0, 1]; M[2, 0] = M[0, 2]; M[2, 1] = M[1, 2]
    w, V = np.linalg.eigh(M)
    d0 = V[0, 2]; d1 = V[1, 2]; d2 = V[2, 2]
    maxr = 0.0; ss = 0.0
    for j in range(n):
        i = idx[j]
        v0 = x[i] - mx; v1 = y[i] - my; v2 = z[i] - mz
        pr = v0 * d0 + v1 * d1 + v2 * d2
        r2 = max(0.0, v0 * v0 + v1 * v1 + v2 * v2 - pr * pr)
        ss += r2
        if r2 > maxr:
            maxr = r2
    return mx, my, mz, d0, d1, d2, np.sqrt(maxr), np.sqrt(ss / n)


@nb.njit(cache=True)
def _excursion_ok(i, j, x, y, z, st, depth, max_exc, geo):
    """The straight segment between hits i and j stays in the tracker band:
    its midpoint is at most max_exc deeper than the deeper of the two
    stations (a single crossing, not a path through the open tunnel)."""
    dm = G.depth_inside(0.5 * (x[i] + x[j]), 0.5 * (y[i] + y[j]), 0.5 * (z[i] + z[j]),
                        geo[0], geo[1], geo[2], geo[3], geo[4], geo[5], geo[6], geo[7], geo[8], geo[9])
    return dm <= max(depth[st[i]], depth[st[j]]) + max_exc


@nb.njit(cache=True)
def _select(cand, nh, length, score, a0, n, max_exact=12):
    """Choose tracklets among the candidates of one event.

    1. Drop candidates whose hits all belong to another candidate.
    2. In each group of candidates connected by shared hits, choose the set
       of non-overlapping candidates that covers the most hits, then uses the
       fewest tracklets, then has the smallest total length (exhaustive for
       groups of up to max_exact candidates, greedy beyond).
    3. A hit used by no chosen tracklet may still get one: the best remaining
       candidate containing it is added (it may share hits; primary = 0).
    Returns the selected candidate indices and their primary flags.
    """
    nc = len(nh)
    order = np.argsort(score, kind="mergesort")           # more hits first
    # hit -> candidates (event-local hit index)
    cnt = np.zeros(n + 1, np.int64)
    for c in range(nc):
        for s in range(MAXST):
            if cand[c, s] >= 0:
                cnt[cand[c, s] - a0 + 1] += 1
    start = np.cumsum(cnt)
    fill = start[:-1].copy()
    lst = np.empty(start[-1], np.int64)
    for c in range(nc):
        for s in range(MAXST):
            if cand[c, s] >= 0:
                h = cand[c, s] - a0
                lst[fill[h]] = c
                fill[h] += 1
    # 1. sub-tracklets and duplicates (a superset is ranked earlier)
    keep = np.zeros(nc, np.bool_)
    for o in order:
        h0 = -1
        for s in range(MAXST):
            if cand[o, s] >= 0:
                h0 = cand[o, s] - a0
                break
        dropped = False
        for k in range(start[h0], start[h0 + 1]):
            q = lst[k]
            if q == o or not keep[q]:
                continue
            inside = True
            for s in range(MAXST):
                if cand[o, s] >= 0 and cand[q, s] != cand[o, s]:
                    inside = False
                    break
            if inside:
                dropped = True
                break
        if not dropped:
            keep[o] = True
    # 2. groups of kept candidates connected by shared hits
    par = np.arange(nc)
    for h in range(n):
        first = -1
        for k in range(start[h], start[h + 1]):
            q = lst[k]
            if not keep[q]:
                continue
            if first < 0:
                first = q
            else:
                a = q
                while par[a] != a:
                    a = par[a]
                b = first
                while par[b] != b:
                    b = par[b]
                if a != b:
                    par[max(a, b)] = min(a, b)
    root = np.empty(nc, np.int64)
    for c in range(nc):
        r = c
        while par[r] != r:
            r = par[r]
        root[c] = r
    chosen = np.zeros(nc, np.bool_)
    members = np.empty(nc, np.int64)
    for r0 in range(nc):
        if not keep[r0] or root[r0] != r0:
            continue
        k = 0
        for o in order:                                    # members, best first
            if keep[o] and root[o] == r0:
                members[k] = o
                k += 1
        if k <= max_exact:
            conf = np.zeros(k, np.int64)
            for i in range(k):
                for j in range(k):
                    if i == j:
                        continue
                    share = False
                    for s in range(MAXST):
                        hi = cand[members[i], s]
                        if hi >= 0 and hi == cand[members[j], s]:
                            share = True
                    if share:
                        conf[i] |= 1 << j
            best = 0
            bcov = -1; bcnt = 0; blen = 0.0
            for mask in range(1, 1 << k):
                ok = True
                cov = 0; cn = 0; ln = 0.0
                for i in range(k):
                    if mask >> i & 1:
                        if conf[i] & mask:
                            ok = False
                            break
                        cov += nh[members[i]]; cn += 1; ln += length[members[i]]
                if not ok:
                    continue
                if (cov > bcov or (cov == bcov and (cn < bcnt or (cn == bcnt and ln < blen - 1e-12)))):
                    best = mask; bcov = cov; bcnt = cn; blen = ln
            for i in range(k):
                if best >> i & 1:
                    chosen[members[i]] = True
        else:
            # greedy: more hits first, then shorter; no overlaps
            used = np.zeros(n, np.bool_)
            g = members[:k].copy()
            key = np.empty(k)
            for i in range(k):
                key[i] = -1000.0 * nh[g[i]] + length[g[i]]
            for i in np.argsort(key, kind="mergesort"):
                c = g[i]
                free = True
                for s in range(MAXST):
                    if cand[c, s] >= 0 and used[cand[c, s] - a0]:
                        free = False
                if free:
                    chosen[c] = True
                    for s in range(MAXST):
                        if cand[c, s] >= 0:
                            used[cand[c, s] - a0] = True
    # 3. hits left without a tracklet
    covered = np.zeros(n, np.bool_)
    for c in range(nc):
        if chosen[c]:
            for s in range(MAXST):
                if cand[c, s] >= 0:
                    covered[cand[c, s] - a0] = True
    extra = np.zeros(nc, np.bool_)
    key = np.empty(nc)
    for c in range(nc):
        key[c] = -1000.0 * nh[c] + length[c]
    for c in np.argsort(key, kind="mergesort"):
        if not keep[c] or chosen[c]:
            continue
        new = False
        for s in range(MAXST):
            if cand[c, s] >= 0 and not covered[cand[c, s] - a0]:
                new = True
        if new:
            extra[c] = True
            for s in range(MAXST):
                if cand[c, s] >= 0:
                    covered[cand[c, s] - a0] = True
    nsel = 0
    for c in range(nc):
        if chosen[c] or extra[c]:
            nsel += 1
    sel = np.empty(nsel, np.int64)
    prim = np.empty(nsel, np.int64)
    q = 0
    for c in order:
        if chosen[c] or extra[c]:
            sel[q] = c
            prim[q] = 1 if chosen[c] else 0
            q += 1
    return sel, prim


@nb.njit(cache=True)
def find_tracklets(offs, x, y, z, t, st, usable, depth, cos_max, dt_win, hit_tol, min_hits, max_exc, geo):
    """Returns hit_tl (number of tracklets using each hit; -1 if its station
    is not used), and per tracklet: event and hits by station (-1 padded)."""
    nh_all = len(x)
    hit_tl = np.zeros(nh_all, np.int64)
    for i in range(nh_all):
        if not usable[i]:
            hit_tl[i] = -1
    cap = 4 * nh_all + 16
    tl_ev = np.empty(cap, np.int64)
    tl_prim = np.empty(cap, np.int64)
    tl_hits = np.full((cap, MAXST), -1, np.int64)
    ntl = 0
    for e in range(len(offs) - 1):
        a0 = offs[e]
        n = offs[e + 1] - a0
        if n < min_hits:
            continue
        ncmax = n * n
        cand = np.full((ncmax, MAXST), -1, np.int64)
        score = np.empty(ncmax)
        clen = np.empty(ncmax)
        cnh = np.empty(ncmax, np.int64)
        nc = 0
        members = np.empty(MAXST, np.int64)
        for ia in range(a0, a0 + n):
            if not usable[ia]:
                continue
            for ib in range(a0, a0 + n):
                if not usable[ib] or st[ib] <= st[ia]:
                    continue
                dx = x[ib] - x[ia]; dy = y[ib] - y[ia]; dz = z[ib] - z[ia]
                dist = np.sqrt(dx * dx + dy * dy + dz * dz)
                if dist < 1e-9 or dist * cos_max > depth[st[ib]] - depth[st[ia]]:
                    continue
                tres = abs(abs(t[ib] - t[ia]) - dist / C_M_NS)
                if tres > dt_win:
                    continue
                if not _excursion_ok(ia, ib, x, y, z, st, depth, max_exc, geo):
                    continue
                ux = dx / dist; uy = dy / dist; uz = dz / dist
                for s in range(MAXST):
                    members[s] = -1
                members[st[ia]] = ia; members[st[ib]] = ib
                for s in range(MAXST):
                    if members[s] >= 0:
                        continue
                    best = -1; bestr = hit_tol
                    for k in range(a0, a0 + n):
                        if not usable[k] or st[k] != s:
                            continue
                        v0 = x[k] - x[ia]; v1 = y[k] - y[ia]; v2 = z[k] - z[ia]
                        pr = v0 * ux + v1 * uy + v2 * uz
                        r = np.sqrt(max(0.0, v0 * v0 + v1 * v1 + v2 * v2 - pr * pr))
                        if r >= bestr:
                            continue
                        dk = np.sqrt(v0 * v0 + v1 * v1 + v2 * v2)
                        if dk * cos_max > abs(depth[s] - depth[st[ia]]):
                            continue
                        if abs(abs(t[k] - t[ia]) - dk / C_M_NS) > dt_win:
                            continue
                        if not (_excursion_ok(k, ia, x, y, z, st, depth, max_exc, geo)
                                and _excursion_ok(k, ib, x, y, z, st, depth, max_exc, geo)):
                            continue
                        best = k; bestr = r
                    if best >= 0:
                        members[s] = best
                m = 0
                idx = np.empty(MAXST, np.int64)
                for s in range(MAXST):
                    if members[s] >= 0:
                        idx[m] = members[s]; m += 1
                if m < min_hits:
                    continue
                fit = _line_fit(x, y, z, idx, m)
                if fit[6] > hit_tol:
                    continue
                for s in range(MAXST):
                    cand[nc, s] = members[s]
                score[nc] = -1000.0 * m + 100.0 * fit[7] + dist + 0.1 * tres
                # length: outermost to innermost hit
                h0 = idx[0]; h1 = idx[m - 1]
                clen[nc] = np.sqrt((x[h1] - x[h0]) ** 2 + (y[h1] - y[h0]) ** 2 + (z[h1] - z[h0]) ** 2)
                cnh[nc] = m
                nc += 1
        if nc == 0:
            continue
        sel, prim = _select(cand[:nc], cnh[:nc], clen[:nc], score[:nc], a0, n)
        for q in range(len(sel)):
            o = sel[q]
            if ntl == len(tl_ev):
                grow_ev = np.empty(2 * len(tl_ev), np.int64); grow_ev[:ntl] = tl_ev; tl_ev = grow_ev
                grow_p = np.empty(2 * len(tl_prim), np.int64); grow_p[:ntl] = tl_prim; tl_prim = grow_p
                grow_h = np.full((2 * len(tl_hits), MAXST), -1, np.int64); grow_h[:ntl] = tl_hits; tl_hits = grow_h
            for s in range(MAXST):
                h = cand[o, s]
                tl_hits[ntl, s] = h
                if h >= 0:
                    hit_tl[h] += 1
            tl_ev[ntl] = e
            tl_prim[ntl] = prim[q]
            ntl += 1
    return hit_tl, tl_ev[:ntl], tl_hits[:ntl], tl_prim[:ntl]


@nb.njit(cache=True)
def tracklet_properties(tl_hits, x, y, z, t, st, t_sigma):
    n = len(tl_hits)
    out = np.zeros((n, 15))
    idx = np.empty(MAXST, np.int64)
    for j in range(n):
        m = 0
        for s in range(MAXST):
            if tl_hits[j, s] >= 0:
                idx[m] = tl_hits[j, s]; m += 1
        cx, cy, cz, d0, d1, d2, maxr, rms = _line_fit(x, y, z, idx, m)
        # orient from the innermost (highest station) to the outermost hit
        inner = idx[m - 1]; outer = idx[0]
        if (x[outer] - x[inner]) * d0 + (y[outer] - y[inner]) * d1 + (z[outer] - z[inner]) * d2 < 0:
            d0 = -d0; d1 = -d1; d2 = -d2
        sl = 0.0; sll = 0.0; slt = 0.0; tm = 0.0
        for q in range(m):
            i = idx[q]
            tm += t[i]
        tm /= m
        for q in range(m):
            i = idx[q]
            L = (x[i] - cx) * d0 + (y[i] - cy) * d1 + (z[i] - cz) * d2
            sll += L * L; slt += L * (t[i] - tm)
        inv_beta = C_M_NS * slt / sll if sll > 0 else 0.0
        inv_beta_err = t_sigma * C_M_NS / np.sqrt(sll) if sll > 0 else 0.0
        chi_out = 0.0; chi_in = 0.0
        for q in range(m):
            i = idx[q]
            L = (x[i] - cx) * d0 + (y[i] - cy) * d1 + (z[i] - cz) * d2
            chi_out += ((t[i] - tm - L / C_M_NS) / t_sigma) ** 2
            chi_in += ((t[i] - tm + L / C_M_NS) / t_sigma) ** 2
        out[j, 0] = m; out[j, 1] = cx; out[j, 2] = cy; out[j, 3] = cz
        out[j, 4] = d0; out[j, 5] = d1; out[j, 6] = d2
        out[j, 7] = tm; out[j, 8] = maxr; out[j, 9] = rms
        out[j, 10] = chi_out; out[j, 11] = chi_in
        out[j, 12] = inv_beta; out[j, 13] = inv_beta_err
    return out


@nb.njit(cache=True)
def through_going(tl_ev, tl_hits, props, x, y, z, merge_tol):
    """Best collinear partner (index within the event) per tracklet."""
    n = len(tl_ev)
    partner = np.full(n, -1, np.int64)
    res = np.full(n, -1.0)
    ib = np.zeros(n)
    idx = np.empty(2 * MAXST, np.int64)
    first = 0
    while first < n:
        last = first
        while last < n and tl_ev[last] == tl_ev[first]:
            last += 1
        for i in range(first, last):
            for j in range(first, last):
                if i == j:
                    continue
                shared = False
                for s1 in range(MAXST):
                    if tl_hits[i, s1] >= 0 and tl_hits[i, s1] == tl_hits[j, s1]:
                        shared = True
                if shared:
                    continue
                m = 0
                for s in range(MAXST):
                    if tl_hits[i, s] >= 0:
                        idx[m] = tl_hits[i, s]; m += 1
                    if tl_hits[j, s] >= 0:
                        idx[m] = tl_hits[j, s]; m += 1
                r = _line_fit(x, y, z, idx, m)[6]
                if r < merge_tol and (partner[i] < 0 or r < res[i]):
                    dist = np.sqrt((props[i, 1] - props[j, 1]) ** 2 + (props[i, 2] - props[j, 2]) ** 2
                                   + (props[i, 3] - props[j, 3]) ** 2)
                    partner[i] = j - first
                    res[i] = r
                    ib[i] = C_M_NS * abs(props[i, 7] - props[j, 7]) / dist if dist > 0 else 0.0
        first = last
    return partner, res, ib


# ---------------------------------------------------------------------------
def truth(tl_hits, contrib_start, hit_ncontrib, c_trk, c_pdg, c_e):
    """Dominant particle per tracklet by energy over its hits' contributors."""
    ntl = len(tl_hits)
    tl_of, hit_of = np.nonzero(tl_hits >= 0)
    hits = tl_hits[tl_of, hit_of]
    cnt = hit_ncontrib[hits]
    rows = np.repeat(contrib_start[hits], cnt) + (np.arange(int(cnt.sum())) - np.repeat(np.cumsum(cnt) - cnt, cnt))
    tl = np.repeat(tl_of, cnt)
    if len(rows) == 0:
        return np.zeros(ntl, np.int64), np.zeros(ntl, np.int64), np.zeros(ntl)
    key = tl * 2**31 + c_trk[rows]
    uk, inv = np.unique(key, return_inverse=True)
    E = np.bincount(inv, c_e[rows], len(uk))
    pdg = np.zeros(len(uk), np.int64); pdg[inv] = c_pdg[rows]
    o = np.lexsort((-E, uk // 2**31))
    firsts = o[np.r_[True, (uk // 2**31)[o][1:] != (uk // 2**31)[o][:-1]]]
    tot = np.bincount(uk // 2**31, E, ntl)
    ttrk = np.zeros(ntl, np.int64); tpdg = np.zeros(ntl, np.int64); frac = np.zeros(ntl)
    tli = uk[firsts] // 2**31
    ttrk[tli] = uk[firsts] % 2**31; tpdg[tli] = pdg[firsts]
    frac[tli] = E[firsts] / np.where(tot[tli] > 0, tot[tli], 1)
    return ttrk, tpdg, frac


ROUND = {"x": 1e-4, "y": 1e-4, "z": 1e-4, "dx": 1e-5, "dy": 1e-5, "dz": 1e-5, "t0": 1e-3,
         "maxRes": 1e-5, "rms": 1e-5, "chi2Out": 1e-3, "chi2In": 1e-3, "invBeta": 1e-4, "invBetaErr": 1e-4,
         "mergeRes": 1e-5, "mergeInvBeta": 1e-4, "truthEFrac": 1e-4}
SMALL = ("nHits", "outgoing", "primary", "nShared", "nConflict", "partner")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("inputs", nargs="+", help="stage-2 hits files (globs allowed)")
    ap.add_argument("--output", required=True)
    ap.add_argument("--stations", default="0,2",
                    help="tracker stations used for tracklet finding (default: the two-layer design, 0 and 24 cm); "
                         "hits of all stations are kept in the output")
    ap.add_argument("--truth", action="store_true", help="use MC-truth hit positions and times instead of smeared")
    ap.add_argument("--max-angle", type=float, default=85.0, help="max crossing angle w.r.t. the layer normal [deg]")
    ap.add_argument("--dt-window", type=float, default=2.5, help="| |dt| - d/c | window [ns]")
    ap.add_argument("--hit-tol", type=float, default=0.025, help="max hit-to-line distance [m]")
    ap.add_argument("--min-hits", type=int, default=2)
    ap.add_argument("--max-excursion", type=float, default=0.20,
                    help="max depth of a hit pair's midpoint beyond the deeper station [m] (same-side crossing)")
    ap.add_argument("--merge-tol", type=float, default=0.05, help="through-going relation: common-line residual [m]")
    ap.add_argument("--t-sigma", type=float, default=0.5, help="time resolution per hit for the chi2 [ns]")
    ap.add_argument("--min-tracklets", type=int, default=1, help="write events with at least this many tracklets")
    ap.add_argument("--zstd-level", type=int, default=9)
    ap.add_argument("--step-size", type=int, default=50000)
    args = ap.parse_args()
    stations = [int(s) for s in args.stations.split(",")]
    depth = np.array([G.station_phi_depth(s) + G.TRACKER_T / 2 for s in range(MAXST)])
    cos_max = np.cos(np.radians(args.max_angle))
    files = sorted(sum((glob.glob(p) for p in args.inputs), []))
    if os.path.exists(args.output):
        os.remove(args.output)
    fout = uproot.recreate(args.output, compression=uproot.ZSTD(args.zstd_level))
    first = True
    nproc = nin = nout = ntl_tot = 0
    t0 = time.time()
    for fn in files:
        f = uproot.open(fn)
        nproc += int(f["meta"]["nEventsProcessed"].array(library="np")[0])
        tree = f["hits"]
        keys = tree.keys()
        for arr in tree.iterate(keys, step_size=args.step_size, library="ak"):
            nev = len(arr)
            nin += nev
            counts = arr["nhit"].to_numpy().astype(np.int64)
            offs = np.concatenate([[0], np.cumsum(counts)])
            fl = lambda k: ak.flatten(arr[k]).to_numpy()
            pk = ("hit_x", "hit_y", "hit_z") if args.truth else ("hit_xSmear", "hit_ySmear", "hit_zSmear")
            x, y, z = (fl(k).astype(np.float64) for k in pk)
            t = fl("hit_tTruth" if args.truth else "hit_t").astype(np.float64)
            st = fl("hit_station").astype(np.int64)
            usable = np.isin(st, stations)
            hit_tl, tl_ev, tl_hits, tl_prim = find_tracklets(offs, x, y, z, t, st, usable, depth, cos_max,
                                                    args.dt_window, args.hit_tol, args.min_hits,
                                                    args.max_excursion, G.GEOM_ARRAYS)
            ntl = len(tl_ev)
            P = tracklet_properties(tl_hits, x, y, z, t, st, args.t_sigma)
            partner, mres, mib = through_going(tl_ev, tl_hits, P, x, y, z, args.merge_tol)
            # truth from the hits' contributors
            nco = fl("hit_nContrib").astype(np.int64)
            cstart = np.cumsum(nco) - nco
            ttrk, tpdg, tfrac = truth(tl_hits, cstart, nco, fl("hit_contrib_trk").astype(np.int64),
                                      fl("hit_contrib_pdg").astype(np.int64),
                                      (fl("hit_contrib_Ephi") + fl("hit_contrib_Ez")).astype(np.float64))
            # hits shared with another tracklet
            used = np.where(tl_hits >= 0, hit_tl[np.maximum(tl_hits, 0)], 0)
            nshared = np.sum((tl_hits >= 0) & (used > 1), axis=1)
            # number of other tracklets sharing at least one hit
            tli, sli = np.nonzero(tl_hits >= 0)
            hh = tl_hits[tli, sli]
            o = np.argsort(hh, kind="stable"); hh, tli = hh[o], tli[o]
            grp = np.r_[0, np.nonzero(np.diff(hh))[0] + 1, len(hh)]
            pairs = [np.stack(np.meshgrid(tli[a:b], tli[a:b]), -1).reshape(-1, 2)
                     for a, b in zip(grp[:-1], grp[1:]) if b - a > 1]
            nconf = np.zeros(ntl, np.int64)
            if pairs:
                pp = np.concatenate(pairs); pp = pp[pp[:, 0] != pp[:, 1]]
                up = np.unique(pp[:, 0] * (ntl + 1) + pp[:, 1])
                nconf = np.bincount(up // (ntl + 1), minlength=ntl)
            trk = {"nHits": P[:, 0], "x": P[:, 1], "y": P[:, 2], "z": P[:, 3], "dx": P[:, 4], "dy": P[:, 5],
                   "dz": P[:, 6], "t0": P[:, 7], "maxRes": P[:, 8], "rms": P[:, 9], "chi2Out": P[:, 10],
                   "chi2In": P[:, 11], "invBeta": P[:, 12], "invBetaErr": P[:, 13],
                   "outgoing": (P[:, 10] < P[:, 11]).astype(np.int64), "primary": tl_prim, "nShared": nshared, "nConflict": nconf, "partner": partner,
                   "mergeRes": mres, "mergeInvBeta": mib, "truthTrk": ttrk, "truthPdg": tpdg, "truthEFrac": tfrac}
            ntl_ev = np.bincount(tl_ev, minlength=nev)
            sel = ntl_ev >= args.min_tracklets
            nout += int(sel.sum()); ntl_tot += int(ntl_ev[sel].sum())
            if not sel.any():
                continue
            tsel = sel[tl_ev]
            rec = {}
            for k, v in trk.items():
                v = np.asarray(v)[tsel]
                if k in ROUND:
                    v = (np.round(v / ROUND[k]) * ROUND[k]).astype(np.float32)
                elif k in SMALL:
                    v = v.astype(np.int16)
                else:
                    v = v.astype(np.int32)
                rec[k] = ak.unflatten(v, ntl_ev[sel])
            a = arr[sel]
            hit_fields = {k[4:]: a[k] for k in keys if k.startswith("hit_") and not k.startswith("hit_contrib_")}
            hit_fields["nTracklets"] = ak.unflatten(hit_tl.astype(np.int16), counts)[sel]
            data = {k: a[k].to_numpy() for k in keys if not k.startswith(("hit_", "nhit"))}
            data["hit"] = ak.zip(hit_fields)
            data["hit_contrib"] = ak.zip({k[12:]: a[k] for k in keys if k.startswith("hit_contrib_")})
            data["trkl"] = ak.zip(rec)
            # hits of each tracklet: indices into the event's hit list, by station
            ts = tl_hits[tsel]
            hit_ev_first = offs[:-1]
            loc = np.where(ts >= 0, ts - hit_ev_first[tl_ev[tsel]][:, None], -1)
            flat = loc[loc >= 0]
            data["trkl_hit"] = ak.zip({"idx": ak.unflatten(flat.astype(np.int16),
                                                           np.bincount(tl_ev[tsel][np.nonzero(loc >= 0)[0]],
                                                                       minlength=nev)[sel])})
            if first:
                fout.mktree("tracklets", {k: (v.type.content if isinstance(v, ak.Array) else v.dtype)
                                          for k, v in data.items()})
                first = False
            fout["tracklets"].extend(data)
            print(f"[stage3] {fn}: {nin} events in, {nout} written, {ntl_tot} tracklets, {time.time() - t0:.0f}s",
                  flush=True)
    meta = {"nEventsProcessed": np.array([nproc], np.int64), "nEventsIn": np.array([nin], np.int64),
            "nEventsWritten": np.array([nout], np.int64), "nTracklets": np.array([ntl_tot], np.int64),
            "stationMask": np.array([sum(1 << s for s in stations)], np.int64),
            "usesTruth": np.array([int(args.truth)], np.int64), "maxAngle_deg": np.array([args.max_angle]),
            "dtWindow_ns": np.array([args.dt_window]), "hitTol_m": np.array([args.hit_tol]),
            "minHits": np.array([args.min_hits], np.int64), "maxExcursion_m": np.array([args.max_excursion]), "mergeTol_m": np.array([args.merge_tol]),
            "tSigma_ns": np.array([args.t_sigma]), "minTracklets": np.array([args.min_tracklets], np.int64)}
    fout.mktree("meta", {k: v.dtype for k, v in meta.items()})
    fout["meta"].extend(meta)
    fout.close()


if __name__ == "__main__":
    main()

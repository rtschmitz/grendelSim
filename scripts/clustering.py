"""Clustering kernels for stage 2 (cells -> sublayer clusters -> tracker hits)."""
import numba as nb
import numpy as np

@nb.njit(cache=True)
def _find(par, i):
    r = i
    while par[r] != r:
        r = par[r]
    while par[i] != r:
        nxt = par[i]
        par[i] = r
        i = nxt
    return r


@nb.njit(cache=True)
def _union_neighbours(keys_sorted, nb_keys):
    n = len(keys_sorted)
    par = np.arange(n)
    # cells with the same channel numbers touch (cannot happen for cells of
    # one element, kept for safety)
    for i in range(1, n):
        if keys_sorted[i] == keys_sorted[i - 1]:
            a = _find(par, i)
            b = _find(par, i - 1)
            if a != b:
                par[max(a, b)] = min(a, b)
    for j in range(nb_keys.shape[1]):
        q = nb_keys[:, j]
        pos = np.searchsorted(keys_sorted, q)
        for i in range(n):
            p = pos[i]
            if p < n and keys_sorted[p] == q[i]:
                a = _find(par, i)
                b = _find(par, p)
                if a != b:
                    par[max(a, b)] = min(a, b)
    for i in range(n):
        par[i] = _find(par, i)
    return par


def touching_groups(ev, det, chan_s, chan_perim):
    """Label groups of touching cells per (event, detector element).

    Cells touch if their channel numbers along the tunnel (chan_s) and
    around the arch (chan_perim) each differ by at most 1 (shared edge or
    corner).  Returns one label per cell, unique across events and elements.
    """
    i_s = chan_s.astype(np.int64)
    i_p = chan_perim.astype(np.int64) + 2048     # keep the packed key positive
    base = (ev.astype(np.int64) * 64 + det) * 100000
    key = (base + i_s) * 4096 + i_p
    order = np.argsort(key, kind="stable")
    ks = key[order]
    # forward neighbours: (s, p+1), (s+1, p-1), (s+1, p), (s+1, p+1)
    nbk = np.stack([(base + i_s) * 4096 + i_p + 1,
                    (base + i_s + 1) * 4096 + i_p - 1,
                    (base + i_s + 1) * 4096 + i_p,
                    (base + i_s + 1) * 4096 + i_p + 1], axis=1)[order]
    par_sorted = _union_neighbours(ks, nbk)
    labels = np.empty(len(key), np.int64)
    labels[order] = order[par_sorted]
    return labels


@nb.njit(cache=True)
def min_cell_distance(pairs_a, pairs_b, cstart, cend, cx, cy, cz):
    """Closest cell-to-cell 3D distance for each cluster pair.

    cstart/cend index each cluster's contiguous cell range in cx/cy/cz.
    """
    out = np.empty(len(pairs_a))
    for k in range(len(pairs_a)):
        a = pairs_a[k]
        b = pairs_b[k]
        best = 1e30
        for i in range(cstart[a], cend[a]):
            for j in range(cstart[b], cend[b]):
                d = (cx[i] - cx[j]) ** 2 + (cy[i] - cy[j]) ** 2 + (cz[i] - cz[j]) ** 2
                if d < best:
                    best = d
        out[k] = np.sqrt(best)
    return out


@nb.njit(cache=True)
def group_seed_time(g, E, t):
    """Time of the highest-energy cell of each group (first such cell on ties).

    g: group label per cell, with values in [0, len(g)).  Returns the
    reference time of each cell's group.
    """
    n = len(g)
    bestE = np.full(n, -np.inf)
    bestT = np.zeros(n)
    for i in range(n):
        k = g[i]
        if E[i] > bestE[k]:
            bestE[k] = E[i]
            bestT[k] = t[i]
    out = np.empty(n)
    for i in range(n):
        out[i] = bestT[g[i]]
    return out


@nb.njit(cache=True)
def greedy_pairs(order, pa, pb, nobj):
    """One-to-one assignment: walk candidate pairs in `order`, accept a pair
    if neither object is used yet.  Returns a mask over the pairs."""
    used = np.zeros(nobj, np.bool_)
    take = np.zeros(len(pa), np.bool_)
    for i in order:
        a = pa[i]
        b = pb[i]
        if used[a] or used[b]:
            continue
        used[a] = True
        used[b] = True
        take[i] = True
    return take

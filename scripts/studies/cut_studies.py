#!/usr/bin/env python3
"""Plots and numbers behind the cell, hit and tracklet construction cuts.

Reads stage-1 cells and stage-2 hits files and writes one plot per study to
--out, printing the efficiencies quoted in docs/cuts.md:

  stage1_cell_energy.png   tracker-cell energy by dominant particle
  stage2_mip_threshold.png sublayer-cluster energy, muons vs e/gamma
  stage2_phiz_distance.png phi-z closest-cell and centroid distance (muons)
  stage2_phiz_timing.png   phi-z measured time difference, with late
                           deposits injected
  stage3_tolerances.png    crossing angle, timing, line residual, midpoint
                           excursion and through-going residual of muon
                           crossings (smeared hits)

usage:
  python3 scripts/studies/cut_studies.py --cells cells_0.root --hits hits_0.root
"""
import argparse
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import awkward as ak  # noqa: E402
import numpy as np  # noqa: E402
import pandas as pd  # noqa: E402
import uproot  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(HERE))
import clustering as C  # noqa: E402
import geometry as G  # noqa: E402
import stage2_trackerhits as S2  # noqa: E402

C_M_NS = 0.299792458
MU, ELE, GAM = "muon", "electron", "photon"


def species(pdg):
    a = np.abs(pdg)
    return np.where(a == 13, MU, np.where(a == 11, ELE, np.where(a == 22, GAM, "other")))


def frac(mask, sel):
    return 100.0 * np.count_nonzero(mask & sel) / max(np.count_nonzero(sel), 1)


def save(fig, out, name):
    fig.tight_layout()
    fig.savefig(os.path.join(out, name), dpi=130)
    plt.close(fig)
    print(f"  -> {name}")


# ---------------------------------------------------------------------------
def load_cells(fn, nmax):
    arr = uproot.open(fn)["cells"].arrays(entry_stop=nmax)
    cells, contrib, _ = S2.read_chunk(arr)
    cells["pdg"] = ak.flatten(arr["cell_pdg"]).to_numpy()
    return cells, contrib


def study_cell_energy(cells, out):
    print("stage 1: tracker-cell energy")
    T = cells["det"] < 100
    E, sp = cells["E"][T], species(cells["pdg"][T])
    fig, ax = plt.subplots(1, 2, figsize=(11, 4))
    bins = np.logspace(-5, 2, 141)
    for s in (MU, ELE, GAM):
        ax[0].hist(E[sp == s], bins, histtype="step", label=s)
    ax[0].axvline(1e-3, color="k", ls="--", label="1 keV floor")
    ax[0].set(xscale="log", yscale="log", xlabel="cell energy [MeV]", ylabel="cells",
              title="tracker cells (1 cm) by dominant particle")
    ax[0].legend()
    thr = np.logspace(-5, 0, 101)
    Es = np.sort(E); cumE = np.cumsum(Es[::-1])[::-1]
    k = np.searchsorted(Es, thr, side="right")
    ax[1].plot(thr, 100 * (len(Es) - k) / len(Es), label="cells kept")
    ax[1].plot(thr, 100 * np.r_[cumE, 0][k] / Es.sum(), label="energy kept")
    ax[1].axvline(1e-3, color="k", ls="--")
    ax[1].set(xscale="log", xlabel="cell threshold [MeV]", ylabel="%", ylim=(0, 101),
              title="tracker cells above threshold")
    ax[1].legend()
    for t in (1e-3, 1e-2):
        print(f"  E > {1e3 * t:g} keV: {frac(E > t, E >= 0):.2f}% of cells, "
              f"{100 * E[E > t].sum() / E.sum():.2f}% of energy")
    save(fig, out, "stage1_cell_energy.png")


# ---------------------------------------------------------------------------
def build_clusters(cells, contrib, dt_cut):
    """Time-consistent sublayer clusters as in stage 2, with dominant particle."""
    T = np.where(cells["det"] < 100)[0]
    c = {k: v[T] for k, v in cells.items()}
    lab = S2.time_consistent_clusters(c, dt_cut)
    _, cl_of = np.unique(lab, return_inverse=True)
    ncl = cl_of.max() + 1
    E = np.bincount(cl_of, c["E"], ncl)
    cl = pd.DataFrame({"E": E})
    cl["ev"] = np.zeros(ncl, np.int64); cl.loc[cl_of, "ev"] = c["ev"]
    det = np.zeros(ncl, np.int64); det[cl_of] = c["det"]
    cl["st"], cl["sub"] = det // 10, det % 10
    for k in ("x", "y", "z"):
        cl[k] = np.bincount(cl_of, c["E"] * c[k], ncl) / E
    tt = np.full(ncl, np.inf); np.minimum.at(tt, cl_of, c["t"]); cl["tTruth"] = tt
    pos = -np.ones(len(cells["E"]), np.int64); pos[T] = np.arange(len(T))
    rows = pos[contrib["cell"]] >= 0
    d = pd.DataFrame({"cl": cl_of[pos[contrib["cell"][rows]]], "trk": contrib["trk"][rows],
                      "pdg": contrib["pdg"][rows], "E": contrib["E"][rows]})
    d = d.groupby(["cl", "trk"], sort=False).agg(pdg=("pdg", "first"), E=("E", "sum")).reset_index()
    d = d.sort_values(["cl", "E"], ascending=[True, False]).drop_duplicates("cl")
    cl["trk"] = 0; cl["pdg"] = 0
    cl.loc[d["cl"].to_numpy(), "trk"] = d["trk"].to_numpy()
    cl.loc[d["cl"].to_numpy(), "pdg"] = d["pdg"].to_numpy()
    order = np.argsort(cl_of, kind="stable")
    ce = np.cumsum(np.bincount(cl_of, minlength=ncl)); cs = ce - np.bincount(cl_of, minlength=ncl)
    cellxyz = (c["x"][order], c["y"][order], c["z"][order])
    return cl, cs, ce, cellxyz


def study_mip_threshold(cl, out, emin):
    print("stage 2: MIP threshold")
    sp = species(cl["pdg"].to_numpy()); E = cl["E"].to_numpy()
    fig, ax = plt.subplots(figsize=(6.5, 4.2))
    bins = np.linspace(0, 8, 161)
    for s in (MU, ELE, GAM):
        ax.hist(E[sp == s], bins, histtype="step", density=True, label=s)
    ax.axvline(emin, color="r", ls="--", label=f"{emin} MeV threshold")
    ax.set(yscale="log", xlabel="sublayer cluster energy [MeV]", ylabel="normalised",
           title="time-consistent sublayer clusters, by dominant particle")
    ax.legend()
    h, b = np.histogram(E[sp == MU], bins)
    print(f"  muon cluster peak {0.5 * (b[np.argmax(h)] + b[np.argmax(h) + 1]):.2f} MeV")
    for thr in (0.5, 1.0, emin, 2.0):
        print(f"  E >= {thr:.1f} MeV kept: muon {frac(E >= thr, sp == MU):.1f}%, "
              f"electron {frac(E >= thr, sp == ELE):.1f}%, photon {frac(E >= thr, sp == GAM):.1f}%")
    save(fig, out, "stage2_mip_threshold.png")


def true_pairs(cl, cs, ce, cellxyz, emin):
    """phi and z clusters of the same muon in the same station."""
    m = cl[(cl["E"] >= emin) & (np.abs(cl["pdg"]) == 13)].reset_index()
    p = m[m["sub"] == 0].merge(m[m["sub"] == 1], on=["ev", "st", "trk"], suffixes=("_p", "_z"))
    pa, pb = p["index_p"].to_numpy(np.int64), p["index_z"].to_numpy(np.int64)
    p["dmin"] = C.min_cell_distance(pa, pb, cs, ce, *cellxyz)
    p["dcen"] = np.sqrt(sum((p[k + "_p"] - p[k + "_z"]) ** 2 for k in "xyz"))
    p["dtTrue"] = p["tTruth_p"] - p["tTruth_z"]
    return p.sort_values("dmin").drop_duplicates("index_p").drop_duplicates("index_z")


def study_phiz_distance(p, out, cut):
    print("stage 2: phi-z distance")
    fig, ax = plt.subplots(figsize=(6.5, 4.2))
    x = np.linspace(0, 0.10, 201)
    for k, lab in (("dmin", "closest cells"), ("dcen", "cluster centroids")):
        v = np.sort(p[k].to_numpy())
        ax.plot(100 * x, 100 * np.searchsorted(v, x, side="right") / len(v), label=lab)
    ax.axvline(100 * cut, color="r", ls="--", label=f"{100 * cut:.1f} cm cut")
    ax.set(xlabel="phi-z distance [cm]", ylabel="muon phi-z pairs within [%]", ylim=(0, 101),
           title="same-muon phi/z clusters, same station")
    ax.legend()
    for c in (0.02, cut):
        print(f"  < {100 * c:.1f} cm: closest cells {frac(p['dmin'] < c, p['dmin'] >= 0):.1f}%, "
              f"centroids {frac(p['dcen'] < c, p['dcen'] >= 0):.1f}%")
    save(fig, out, "stage2_phiz_distance.png")


def study_phiz_timing(p, out, sigma, window, seed):
    print("stage 2: phi-z timing")
    rng = np.random.default_rng(seed)
    dt = p["dtTrue"].to_numpy() + sigma * (rng.standard_normal(len(p)) - rng.standard_normal(len(p)))
    fig, ax = plt.subplots(1, 2, figsize=(11, 4))
    bins = np.linspace(-15, 15, 241)
    for shift, lab in ((0, "same muon"), (5, "+5 ns late deposit"), (10, "+10 ns late deposit")):
        ax[0].hist(dt + shift, bins, histtype="step", label=lab)
    ax[0].axvspan(-window, window, color="r", alpha=0.1, label=f"|dt| < {window} ns")
    ax[0].set(yscale="log", xlabel="measured t(phi) - t(z) [ns]", ylabel="pairs",
              title=f"smeared, {sigma} ns per sublayer")
    ax[0].legend(fontsize=8)
    w = np.linspace(0.5, 6, 111)
    for shift, lab in ((0, "same muon"), (5, "+5 ns"), (10, "+10 ns")):
        ax[1].plot(w, [frac(np.abs(dt + shift) < x, np.ones(len(dt), bool)) for x in w], label=lab)
    ax[1].axvline(window, color="r", ls="--")
    ax[1].set(xlabel="window [ns]", ylabel="pairs accepted [%]")
    ax[1].legend()
    a = np.abs(p["dtTrue"].to_numpy())
    print(f"  true |dt|: 99% < {np.quantile(a, 0.99):.2f} ns, 99.9% < {np.quantile(a, 0.999):.2f} ns")
    for x in (2.5, window, 3.5):
        print(f"  window {x} ns: same muon {frac(np.abs(dt) < x, a >= 0):.2f}%, "
              f"+5 ns {frac(np.abs(dt + 5) < x, a >= 0):.2f}%, +10 ns {frac(np.abs(dt + 10) < x, a >= 0):.3f}%")
    save(fig, out, "stage2_phiz_timing.png")


# ---------------------------------------------------------------------------
def muon_crossings(fn, nmax):
    """Smeared hits of muons, grouped into crossings of the tracker band.

    A crossing is a run of the muon's hits (in true-time order) whose
    station number changes monotonically; a reversal starts a new one."""
    cols = ["hit_station", "hit_xSmear", "hit_ySmear", "hit_zSmear", "hit_t", "hit_tTruth", "hit_trk",
            "hit_trk_z", "hit_pdg"]
    arr = uproot.open(fn)["hits"].arrays(cols, entry_stop=nmax)
    a = pd.DataFrame({c[4:]: ak.flatten(arr[c]).to_numpy() for c in cols})
    a["ev"] = np.repeat(np.arange(len(arr)), ak.num(arr["hit_station"]).to_numpy())
    a = a[(np.abs(a["pdg"]) == 13) & (a["trk"] == a["trk_z"])]
    a = a.sort_values(["ev", "trk", "tTruth"]).reset_index(drop=True)
    same = (a["ev"].diff() == 0) & (a["trk"].diff() == 0)
    step = np.sign(a["station"].diff()).where(same, 0)
    prev = step.shift().where(same, 0)
    new = ~same | (step == 0) | ((prev != 0) & (step != prev))
    a["crossing"] = np.cumsum(new)
    return a


def line_residual(p0, p1, q):
    d = p1 - p0
    d /= np.linalg.norm(d, axis=1)[:, None]
    r = q - p0
    return np.linalg.norm(r - (r * d).sum(1)[:, None] * d, axis=1)


def study_tracklets(hits, out, a_cut, dt_cut, tol, exc_cut, merge_tol):
    print("stage 3: tracklet tolerances (muon crossings, stations 0 and 2)")
    h = hits
    xyz = ["xSmear", "ySmear", "zSmear"]
    s0 = h[h["station"] == 0]; s2 = h[h["station"] == 2]; s1 = h[h["station"] == 1]
    pr = s0.merge(s2, on=["ev", "trk"], suffixes=("0", "2"))
    pr["same"] = pr["crossing0"] == pr["crossing2"]
    P0 = pr[[k + "0" for k in xyz]].to_numpy(); P2 = pr[[k + "2" for k in xyz]].to_numpy()
    d = P2 - P0
    L = np.linalg.norm(d, axis=1)
    u_s, u_p = G.surface_directions(*P0.T)
    n = np.cross(u_s, u_p)
    pr["angle"] = np.degrees(np.arccos(np.clip(np.abs((d * n).sum(1)) / L, 0, 1)))
    pr["dtres"] = np.abs(np.abs(pr["t2"] - pr["t0"]) - L / C_M_NS)
    mid = 0.5 * (P0 + P2)
    deeper = G.station_phi_depth(2) + G.TRACKER_T / 2
    pr["exc"] = G.tunnel_coordinates(*mid.T)["depth"] - deeper
    same = pr[pr["same"]]
    # station-1 hit of the same crossing w.r.t. the station 0-2 line
    r1 = same.merge(s1[["ev", "trk", "crossing"] + xyz], left_on=["ev", "trk", "crossing0"],
                    right_on=["ev", "trk", "crossing"])
    res1 = line_residual(r1[[k + "0" for k in xyz]].to_numpy(), r1[[k + "2" for k in xyz]].to_numpy(),
                         r1[xyz].to_numpy())
    # two crossings of one muon (entry and exit): residual to their common line
    sc = same.sort_values(["ev", "trk", "t0"])
    g = sc.groupby(["ev", "trk"]).filter(lambda df: len(df) == 2)
    A, B = g.iloc[0::2], g.iloc[1::2]
    pts = np.stack([A[[k + "0" for k in xyz]].to_numpy(), A[[k + "2" for k in xyz]].to_numpy(),
                    B[[k + "0" for k in xyz]].to_numpy(), B[[k + "2" for k in xyz]].to_numpy()], 1)
    cen = pts.mean(1, keepdims=True)
    _, _, vt = np.linalg.svd(pts - cen)
    dirn = vt[:, 0, :][:, None, :]
    rr = pts - cen
    mres = np.linalg.norm(rr - (rr * dirn).sum(2)[..., None] * dirn, axis=2).max(1)

    fig, ax = plt.subplots(2, 3, figsize=(14, 7.5))
    ax = ax.ravel()
    ax[0].hist(same["angle"], np.linspace(0, 90, 91), histtype="step")
    ax[0].axvline(a_cut, color="r", ls="--")
    ax[0].set(yscale="log", xlabel="crossing angle to layer normal [deg]", ylabel="muon crossings",
              title=f"angle < {a_cut:g} deg")
    ax[1].hist(same["dtres"], np.linspace(0, 6, 121), histtype="step")
    ax[1].axvline(dt_cut, color="r", ls="--")
    ax[1].set(yscale="log", xlabel="| |dt| - d/c | [ns]", title=f"timing window {dt_cut:g} ns")
    ax[2].hist(100 * res1, np.linspace(0, 6, 121), histtype="step")
    ax[2].axvline(100 * tol, color="r", ls="--")
    ax[2].set(yscale="log", xlabel="station-1 hit to 0-2 line [cm]", title=f"hit tolerance {100 * tol:g} cm")
    bins = np.linspace(-0.3, 1.5, 91)
    ax[3].hist(pr.loc[pr["same"], "exc"], bins, histtype="step", density=True, label="same crossing")
    ax[3].hist(pr.loc[~pr["same"], "exc"], bins, histtype="step", density=True, label="different crossings")
    ax[3].axvline(exc_cut, color="r", ls="--")
    ax[3].set(yscale="log", xlabel="pair midpoint beyond deeper station [m]", ylabel="normalised",
              title=f"excursion < {100 * exc_cut:g} cm")
    ax[3].legend(fontsize=8)
    ax[4].hist(100 * mres, np.linspace(0, 10, 101), histtype="step")
    ax[4].axvline(100 * merge_tol, color="r", ls="--")
    ax[4].set(yscale="log", xlabel="max hit residual to common line [cm]",
              title=f"through-going residual < {100 * merge_tol:g} cm")
    ax[5].axis("off")
    ax[5].text(0, 0.5, "muon crossings of stations 0 and 2,\nsmeared hits (3 mm, 0.7 ns per sublayer)\n"
               "red: stage-3 defaults", fontsize=11, va="center")
    print(f"  angle < {a_cut}: {frac(same['angle'] < a_cut, same['angle'] >= 0):.2f}%")
    print(f"  timing < {dt_cut} ns: {frac(same['dtres'] < dt_cut, same['dtres'] >= 0):.2f}%")
    print(f"  station-1 residual < {100 * tol} cm: {frac(res1 < tol, res1 >= 0):.2f}%")
    print(f"  excursion < {100 * exc_cut} cm: same crossing {frac(pr['exc'] < exc_cut, pr['same']):.2f}%, "
          f"different crossings {frac(pr['exc'] < exc_cut, ~pr['same']):.1f}% "
          f"(> 2 m apart: {frac(pr['exc'] < exc_cut, ~pr['same'] & (L > 2)):.1f}%)")
    print(f"  through-going residual < {100 * merge_tol} cm: {frac(mres < merge_tol, mres >= 0):.2f}%")
    save(fig, out, "stage3_tolerances.png")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--cells", required=True, help="stage-1 cells file")
    ap.add_argument("--hits", required=True, help="stage-2 hits file")
    ap.add_argument("--out", default=os.path.join(os.path.dirname(HERE), "docs", "plots"))
    ap.add_argument("--max-events", type=int, default=100000)
    ap.add_argument("--seed", type=int, default=1)
    args = ap.parse_args()
    os.makedirs(args.out, exist_ok=True)
    cells, contrib = load_cells(args.cells, args.max_events)
    study_cell_energy(cells, args.out)
    cl, cs, ce, cellxyz = build_clusters(cells, contrib, 3.0)
    study_mip_threshold(cl, args.out, 1.5)
    p = true_pairs(cl, cs, ce, cellxyz, 1.5)
    study_phiz_distance(p, args.out, 0.025)
    study_phiz_timing(p, args.out, 0.7, 3.0, args.seed)
    hits = muon_crossings(args.hits, args.max_events)
    study_tracklets(hits, args.out, 85.0, 2.5, 0.025, 0.20, 0.05)


if __name__ == "__main__":
    main()

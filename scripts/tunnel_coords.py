#!/usr/bin/env python3
"""Add continuous tunnel coordinates for the positions in a cells or hits file.

The post-processing files store positions only as global x/y/z (plus
discrete channel / layer / segment numbers).  This tool finds every position
triplet <prefix>x<suffix>, <prefix>y<suffix>, <prefix>z<suffix> in the tree
(e.g. cell_x/y/z, hit_x/y/z, hit_xSmear/ySmear/zSmear) and writes, for each,
<prefix>{s,perim,depth,lx,ly}<suffix> to a companion tree with the same
entries and the same collection counters (ncell, nhit, ...):

  s      arc length along the tunnel centerline [m]
  perim  position around the arch along the nominal tunnel surface [m]
  depth  inward distance from the nominal surface [m]
  lx, ly position in the tunnel cross-section [m]

usage:
  python3 scripts/tunnel_coords.py hits_0.root --output hits_0_tunnel.root
  # ROOT:  t->AddFriend("hits_tunnel", "hits_0_tunnel.root");
  # Python: geometry.tunnel_coordinates(x, y, z) does the same on arrays.
"""
import argparse
import os
import re
import sys

import awkward as ak
import numpy as np
import uproot

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import geometry as G  # noqa: E402

FIELDS = ("s", "perim", "depth", "lx", "ly")


def find_triplets(names):
    """[(collection, prefix, suffix)] for every x/y/z triplet."""
    out = []
    for n in names:
        m = re.match(r"^([A-Za-z]+)_x(.*)$", n)
        if m and f"{m[1]}_y{m[2]}" in names and f"{m[1]}_z{m[2]}" in names:
            out.append((m[1], m[1] + "_", m[2]))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("input")
    ap.add_argument("--output", required=True)
    ap.add_argument("--tree", default=None, help="tree name (default: cells or hits, whichever exists)")
    ap.add_argument("--step-size", type=int, default=100000)
    ap.add_argument("--zstd-level", type=int, default=9)
    ap.add_argument("--precision", type=float, default=1e-4, help="round coordinates to this [m]")
    args = ap.parse_args()
    fin = uproot.open(args.input)
    tname = args.tree or next(t for t in ("cells", "hits") if t in fin)
    tree = fin[tname]
    trip = find_triplets(set(tree.keys()))
    if not trip:
        sys.exit(f"no x/y/z triplets found in {tname}")
    print("position triplets:", [f"{p}x{s}" for _, p, s in trip])
    if os.path.exists(args.output):
        os.remove(args.output)
    fout = uproot.recreate(args.output, compression=uproot.ZSTD(args.zstd_level))
    branches = ["eventID"] + [f"{p}{c}{s}" for _, p, s in trip for c in "xyz"]
    first = True
    for arr in tree.iterate(branches, step_size=args.step_size, library="ak"):
        cols = {}
        for coll, p, suf in trip:
            counts = ak.num(arr[f"{p}x{suf}"]).to_numpy()
            xyz = [ak.flatten(arr[f"{p}{c}{suf}"]).to_numpy() for c in "xyz"]
            tc = G.tunnel_coordinates(*xyz)
            for k in FIELDS:
                v = (np.round(tc[k] / args.precision) * args.precision).astype(np.float32)
                cols.setdefault(coll, {})[k + suf] = ak.unflatten(v, counts)
        data = {"eventID": arr["eventID"].to_numpy().astype(np.int32)}
        data.update({coll: ak.zip(f) for coll, f in cols.items()})
        if first:
            fout.mktree(f"{tname}_tunnel", {k: (v.type.content if isinstance(v, ak.Array) else v.dtype)
                                            for k, v in data.items()})
            first = False
        fout[f"{tname}_tunnel"].extend(data)
    fout.close()
    print(f"wrote {tname}_tunnel to {args.output}")


if __name__ == "__main__":
    main()

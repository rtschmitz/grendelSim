#!/usr/bin/env python3
"""Run all five stages on one raw grendelSim file.

Writes, in --outdir (default: the current directory):
  <name>_cells.root, <name>_hits.root, <name>_tracklets.root,
  <name>_vertices.root, <name>_cutflow.root
where <name> is the input file name without .root.  Every event of the
input is processed unless --entry-start / --entry-stop are given.  Extra
options for a stage are passed as one quoted string, e.g.
  --stage3-args "--stations 0,1,2,3"

usage:
  python3 scripts/run_chain.py grendelSim.root --outdir out/
"""
import argparse
import os
import shlex
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
STAGES = [("stage1_consolidate.py", "cells"), ("stage2_trackerhits.py", "hits"),
          ("stage3_tracklets.py", "tracklets"), ("stage4_vertices.py", "vertices"),
          ("stage5_cutflow.py", "cutflow")]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("input", help="raw grendelSim ROOT file")
    ap.add_argument("--outdir", default=".")
    ap.add_argument("--name", help="output file prefix (default: input file name without .root)")
    ap.add_argument("--entry-start", type=int)
    ap.add_argument("--entry-stop", type=int)
    ap.add_argument("--first-stage", type=int, default=1, choices=range(1, 6),
                    help="start here, reusing the earlier outputs already in --outdir")
    ap.add_argument("--last-stage", type=int, default=5, choices=range(1, 6))
    for i in range(1, 6):
        ap.add_argument(f"--stage{i}-args", default="", help=f"extra options for stage {i}")
    args = ap.parse_args()
    name = args.name or os.path.basename(args.input).removesuffix(".root")
    os.makedirs(args.outdir, exist_ok=True)
    out = {tag: os.path.join(args.outdir, f"{name}_{tag}.root") for _, tag in STAGES}

    t0 = time.time()
    prev = args.input
    for i, (script, tag) in enumerate(STAGES, start=1):
        if i < args.first_stage:
            prev = out[tag]
            continue
        if i > args.last_stage:
            break
        cmd = [sys.executable, os.path.join(HERE, script), prev]
        if i == 1:
            cmd.append(out[tag])
            if args.entry_start is not None:
                cmd += ["--entry-start", str(args.entry_start)]
            if args.entry_stop is not None:
                cmd += ["--entry-stop", str(args.entry_stop)]
        else:
            if not os.path.exists(prev):
                sys.exit(f"[chain] missing input for stage {i}: {prev}")
            cmd += ["--output", out[tag]]
        cmd += shlex.split(getattr(args, f"stage{i}_args"))
        print(f"[chain] stage {i}: {' '.join(cmd)}", flush=True)
        t = time.time()
        if subprocess.run(cmd).returncode != 0:
            sys.exit(f"[chain] stage {i} failed")
        print(f"[chain] stage {i} done in {time.time() - t:.0f}s -> {out[tag]}", flush=True)
        prev = out[tag]
    print(f"[chain] finished in {time.time() - t0:.0f}s", flush=True)


if __name__ == "__main__":
    main()

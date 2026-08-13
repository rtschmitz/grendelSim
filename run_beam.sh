#!/usr/bin/env bash
if [[ "${BASH_SOURCE[0]}" != "$0" ]]; then
  echo "Run this script as ./run_beam.sh; do not source it." >&2
  return 2
fi
set -euo pipefail
repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
cmake -S "$repo_dir" -B "$repo_dir/build"
cmake --build "$repo_dir/build" --parallel
cd "$repo_dir/build"
./grendelSim beam ../macros/beam_muon.mac

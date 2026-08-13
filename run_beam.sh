#!/usr/bin/env bash
set -euo pipefail
repo_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
cmake -S "$repo_dir" -B "$repo_dir/build"
cmake --build "$repo_dir/build" --parallel
cd "$repo_dir/build"
./grendelSim beam ../macros/beam_muon.mac

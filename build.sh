#!/usr/bin/env bash
set -euo pipefail

# Usage: ./build.sh [debug|release]
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
config="${1:-debug}"

cmake --preset "$config"
cmake --build --preset "$config" --parallel

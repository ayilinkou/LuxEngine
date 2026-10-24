#!/usr/bin/env bash
set -euo pipefail

# Usage: ./run.sh [debug|release]
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
config="${1:-debug}"

exec "./build/$config/LuxEngine"

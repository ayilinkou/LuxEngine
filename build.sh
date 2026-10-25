#!/usr/bin/env bash
set -euo pipefail

# Usage: ./build.sh [windows-on-linux-debug|windows-on-linux-release]
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
config="${1:-windows-on-linux-debug}"

if (( $# > 1 )); then
    echo "Usage: ./build.sh [windows-on-linux-debug|windows-on-linux-release]" >&2
    exit 2
fi
case "$config" in
    windows-on-linux-debug|windows-on-linux-release) ;;
    *) echo "Unknown build configuration: $config" >&2; exit 2 ;;
esac

cmake --preset "$config"
cmake --build --preset "$config" --parallel

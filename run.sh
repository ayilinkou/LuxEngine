#!/usr/bin/env bash
set -euo pipefail

# Usage: ./run.sh [windows-on-linux-debug|windows-on-linux-release]
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"
config="${1:-windows-on-linux-debug}"

if (( $# > 1 )); then
    echo "Usage: ./run.sh [windows-on-linux-debug|windows-on-linux-release]" >&2
    exit 2
fi
case "$config" in
    windows-on-linux-debug|windows-on-linux-release) ;;
    *) echo "Unknown build configuration: $config" >&2; exit 2 ;;
esac

executable="$PWD/build/$config/LuxEngine.exe"
if [[ ! -f "$executable" ]]; then
    echo "Build the executable first with: ./build.sh $config" >&2
    exit 1
fi

if ! command -v wine >/dev/null 2>&1; then
    echo "Wine is required to run a Windows build on Linux." >&2
    exit 1
fi
export WINEPREFIX="${WINEPREFIX:-$PWD/.cache/wine}"
if [[ ! -f "$WINEPREFIX/system.reg" ]]; then
    for tool in wineboot wineserver; do
        if ! command -v "$tool" >/dev/null 2>&1; then
            echo "$tool is required to initialize the Wine prefix." >&2
            exit 1
        fi
    done
    mkdir -p -- "$WINEPREFIX"
    echo "Initializing Wine prefix: $WINEPREFIX"
    wineboot --init
    # Finish initial setup and flush the registry before launching the app.
    wineserver --wait
fi
exec wine "$executable"

# LuxEngine

A D3D12, C++20 game engine for Windows and Linux through Wine and vkd3d-proton.

## Prerequisites

- CMake 3.21 or newer
- Ninja
- MSVC on Windows; a native C++ compiler for vcpkg host tools on Linux
- Git and a bootstrapped vcpkg checkout, with `VCPKG_ROOT` pointing to it

## Build

Use `-release` instead of `-debug` for release builds.

### Windows

```bat
build.bat windows-debug
```

### Windows-on-Linux

The Windows-on-Linux presets require LLVM-MinGW on an x86_64 Linux host.
The first configure downloads the pinned archive, verifies its SHA-256 checksum,
and extracts it into `.cache/tools/`. Later builds reuse that installation.

```bash
./build.sh windows-on-linux-debug
```

## Running

### Windows

```bat
run.bat windows-debug
```

### Windows-on-Linux

Wine is required to run Windows executables on Linux.

```bash
./run.sh windows-on-linux-debug
```

`run.sh` initializes `.cache/wine` on first use and reuses it afterward.
To select another prefix, provide an absolute path:

```bash
WINEPREFIX=/absolute/path/to/prefix ./run.sh windows-on-linux-debug
```

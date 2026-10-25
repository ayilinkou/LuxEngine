if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux" OR NOT CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64|AMD64)$")
    message(FATAL_ERROR "The Windows cross-build currently requires an x86_64 Linux host.")
endif()

# The version and digest come from the upstream GitHub release assets:
# https://github.com/mstorsjo/llvm-mingw/releases/tag/20261020
set(_lux_llvm_version "20261020")
set(_lux_llvm_package "llvm-mingw-${_lux_llvm_version}-ucrt-ubuntu-22.04-x86_64")
set(_lux_llvm_sha256 "55064b60b991a1e1e8090b8c2f84e82b22cfe5903d12c159d65064cf5e8ae335")
get_filename_component(_lux_tools_dir "${CMAKE_CURRENT_LIST_DIR}/../../.cache/tools" ABSOLUTE)
set(_lux_llvm_root "${_lux_tools_dir}/${_lux_llvm_package}")

function(_lux_ensure_llvm_mingw)
    file(MAKE_DIRECTORY "${_lux_tools_dir}")
    file(LOCK "${_lux_tools_dir}/llvm-mingw.lock" GUARD FUNCTION TIMEOUT 300)
    if(EXISTS "${_lux_llvm_root}/.luxengine-ready")
        return()
    endif()

    set(_archive "${_lux_tools_dir}/${_lux_llvm_package}.tar.xz")
    message(STATUS "Downloading LLVM-MinGW ${_lux_llvm_version} (Windows cross-build only)")
    file(DOWNLOAD
        "https://github.com/mstorsjo/llvm-mingw/releases/download/${_lux_llvm_version}/${_lux_llvm_package}.tar.xz"
        "${_archive}"
        EXPECTED_HASH "SHA256=${_lux_llvm_sha256}"
        TLS_VERIFY ON
        SHOW_PROGRESS
        STATUS _download_status
    )
    list(GET _download_status 0 _download_code)
    if(NOT _download_code EQUAL 0)
        message(FATAL_ERROR "Could not download LLVM-MinGW: ${_download_status}")
    endif()
    message(STATUS "Extracting LLVM-MinGW ${_lux_llvm_version}")
    file(ARCHIVE_EXTRACT INPUT "${_archive}" DESTINATION "${_lux_tools_dir}")
    if(NOT EXISTS "${_lux_llvm_root}/bin/x86_64-w64-mingw32-clang++")
        message(FATAL_ERROR "LLVM-MinGW archive did not contain the expected compiler.")
    endif()
    file(WRITE "${_lux_llvm_root}/.luxengine-ready" "${_lux_llvm_sha256}\n")
endfunction()

_lux_ensure_llvm_mingw()

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER "${_lux_llvm_root}/bin/x86_64-w64-mingw32-clang")
set(CMAKE_CXX_COMPILER "${_lux_llvm_root}/bin/x86_64-w64-mingw32-clang++")
set(CMAKE_RC_COMPILER "${_lux_llvm_root}/bin/x86_64-w64-mingw32-windres")
set(CMAKE_AR "${_lux_llvm_root}/bin/llvm-ar")
set(CMAKE_RANLIB "${_lux_llvm_root}/bin/llvm-ranlib")
set(CMAKE_STRIP "${_lux_llvm_root}/bin/llvm-strip")

# Preserve vcpkg's compiler/linker flag handling while overriding tool discovery.
if(DEFINED Z_VCPKG_ROOT_DIR)
    include("${Z_VCPKG_ROOT_DIR}/scripts/toolchains/mingw.cmake")
else()
    include("$ENV{VCPKG_ROOT}/scripts/toolchains/mingw.cmake")
endif()

# Programs used during the build run on Linux; headers and libraries target Windows.
# Toolchain files can be evaluated more than once. Preserve any dependency roots
# that vcpkg added during the previous evaluation.
list(APPEND CMAKE_FIND_ROOT_PATH "${_lux_llvm_root}/x86_64-w64-mingw32")
list(REMOVE_DUPLICATES CMAKE_FIND_ROOT_PATH)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

cmake_minimum_required(VERSION 3.21)

project(LuxEngine LANGUAGES CXX)

if(NOT WIN32)
  message(
    FATAL_ERROR
      "LuxEngine currently targets Windows. On Linux, use the windows-on-linux-debug or windows-on-linux-release preset."
  )
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json"
       "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC
       RESULT compile_commands_link_result)
  if(NOT compile_commands_link_result STREQUAL "0")
    message(
      WARNING
        "Could not link compile_commands.json: ${compile_commands_link_result}")
  endif()
endif()

if(MSVC)
  add_compile_options(/W4 /WX)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(LuxEngine apps/main.cpp)

find_package(SDL3 CONFIG REQUIRED)
target_link_libraries(LuxEngine PRIVATE SDL3::SDL3)

if(MINGW)
  # Include the compiler's C++ support libraries in the executable. Windows
  # system libraries (including the Universal CRT) are still supplied by the OS.
  target_link_options(LuxEngine PRIVATE -static)
endif()

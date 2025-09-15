cmake_minimum_required(VERSION 3.20)

# ---- Project ----
project(TeamProject_COMP345 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Executable ----
add_executable(TeamProject_COMP345
  main.cpp         # if your entry file is MainDriver.cpp, replace this line
  Map.cpp
  Map.h
  MapDriver.cpp
)

# Put the exe in build/bin
set_target_properties(TeamProject_COMP345 PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Copy the maps folder next to the exe after build (so relative paths work)
add_custom_command(TARGET TeamProject_COMP345 POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/maps
          ${CMAKE_BINARY_DIR}/bin/maps
)

cmake_minimum_required(VERSION 3.3)
project(ld34_growing_spaceships)

# Modify here to find external libraries
set(EXTRA_LIBS_DIR D:/msys64/mingw64)

# Setup search paths
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${EXTRA_LIBS_DIR})

# Setup compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wno-unused-private-field")

# Find required packages
find_package(SDL2 2.0.0 REQUIRED COMPONENTS image mixer ttf)
find_package(GLM 0.9.5.4 REQUIRED)


add_subdirectory(src)

cmake_minimum_required(VERSION 3.10)
project(LinChecksum)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Add source code subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../code code_build)

# Add test subdirectory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../gtests gtests_build)

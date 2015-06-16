cmake_minimum_required(VERSION 2.8)

project(testMappedArrays)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")
  set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -march=native -mtune=native -ftree-vectorize -ffast-math -fopt-info-vec-optimized")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -DDUMP_ARRAYS")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(srcs
    Io.cxx
    Timer.cxx
    )

add_executable(Magnitude ${srcs} Magnitude.cpp)
target_link_libraries(Magnitude ${VTK_LIBRARIES})

add_executable(Gradient ${srcs} Gradient.cpp)
target_link_libraries(Gradient ${VTK_LIBRARIES})

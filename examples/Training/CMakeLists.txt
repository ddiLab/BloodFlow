if(CI)
###############################################################################
# Jifu: you need to change the EXECUTABLE_NAME to your file, here is embolism
set(EXECUTABLE_NAME "TrainingFlow")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

###############################################################################

get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_executable(${PROJECT_NAME} "./${EXECUTABLE_NAME}.cpp")

# Link with the following libraries
target_link_libraries(${PROJECT_NAME} palabos)
if(ENABLE_MPI)
    target_link_libraries(${PROJECT_NAME} ${MPI_CXX_LIBRARIES})
endif()

###############################################################################

# This returns when you compile Palabos and all the examples (root CMakeLists.txt)
return()

###############################################################################
endif()

cmake_minimum_required(VERSION 2.8.12)

project(palabosExample)
enable_language(CXX)

# Jifu: you need to change the EXECUTABLE_NAME to your file, here is embolism
set(EXECUTABLE_NAME "TrainingFlow")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    #set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
endif()
message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")
message(${CMAKE_BUILD_TYPE})

# Compiler flags
# Append flags: set(CMAKE_XXX_FLAGS "${CMAKE_XXX_FLAGS} ...")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    message("GCC.")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DPLB_DEBUG -O0")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    message("Clang.")
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wnon-virtual-dtor")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DPLB_DEBUG -O0")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    message("MSVC.")
    set(CMAKE_CXX_FLAGS_RELEASE "/Ox /Ot /GS- /GL /DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "/DPLB_DEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/LTCG /INCREMENTAL:NO /OPT:REF")
else()
    message( FATAL_ERROR "CXX compiler not recognized. CMake will quit." )
endif()

option(ENABLE_MPI "Enable MPI" ON)
if(ENABLE_MPI)
    message("Enabling MPI")
    find_package(MPI REQUIRED)
    if(MPI_CXX_FOUND)
        #set(CMAKE_CXX_COMPILER "${MPI_CXX_COMPILER}")
        include_directories(${MPI_CXX_INCLUDE_PATH})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILE_FLAGS}")
        add_definitions(-DPLB_MPI_PARALLEL)
    endif()
endif()

if(WIN32)
    option(ENABLE_POSIX "Enable POSIX" OFF)
else()
    option(ENABLE_POSIX "Enable POSIX" ON)
endif()

if(ENABLE_POSIX)
    message("Enabling POSIX")
    add_definitions(-DPLB_USE_POSIX)
endif()

if(APPLE)
    add_definitions(-DPLB_MAC_OS_X)
endif()

if(WIN32 OR CYGWIN)
    add_definitions(-DPLB_WINDOWS)
endif()

###############################################################################
# Palabos Library
###############################################################################
# Jifu: you need to modify the path to palabos and other source code
include_directories("/home/mectro/palabos/src")
include_directories("/home/mectro/palabos/externalLibraries")
include_directories("/home/mectro/palabos/externalLibraries/Eigen3")
# Jifu: add the following two directories 
include_directories("/home/mectro/BloodFlow/ibm")
include_directories("/home/mectro/lammps/src")

# Jifu: you need to modify the path to palabos and other source code
file(GLOB_RECURSE PALABOS_SRC "/home/mectro/palabos/src/*.cpp")
file(GLOB_RECURSE EXT_SRC "/home/mectro/palabos/externalLibraries/tinyxml/*.cpp")
# Jifu: set the directory for lammps library
set(LAMMPS_SRC "/home/mectro/lammps/src")

add_library(palabos STATIC ${PALABOS_SRC} ${EXT_SRC})

###############################################################################

add_executable(${EXECUTABLE_NAME} "./${EXECUTABLE_NAME}.cpp")

# Link with the following libraries
# Jifu: modifiy the following line with linked lammps library
target_link_libraries(${EXECUTABLE_NAME} palabos ${LAMMPS_SRC}/liblammps_mpi.a)
if(ENABLE_MPI)
    target_link_libraries(${EXECUTABLE_NAME} ${MPI_CXX_LIBRARIES})
endif()

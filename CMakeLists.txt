cmake_minimum_required(VERSION 3.10)

# Project name
project(MPH0)

# Security checks when building Graph class
option(ENABLE_SECURITY_CHECKS "Enable security checks in Graph class" OFF)
if(ENABLE_SECURITY_CHECKS)
    # add to the compiler flags
    add_definitions(-DENABLE_SECURITY_CHECKS)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add profiling flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")

# Debug mode (eg. in vscode)
# set(CMAKE_BUILD_TYPE Debug)

# Include all hpp files
include_directories(${CMAKE_SOURCE_DIR}/include)
# Find Boost and Include Boost headers
set(BOOST_ROOT /usr)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

message(STATUS "Boost_ROOT: $ENV{BOOST_ROOT}")
message(STATUS "Boost_INCLUDEDIR: $ENV{BOOST_INCLUDEDIR}")
message(STATUS "Boost_LIBRARYDIR: $ENV{BOOST_LIBRARYDIR}")

# Print CMake's search paths
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_INCLUDE_PATH: ${CMAKE_INCLUDE_PATH}")
message(STATUS "CMAKE_LIBRARY_PATH: ${CMAKE_LIBRARY_PATH}")

find_package(Boost REQUIRED)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_FOUND: ${Boost_FOUND}")

include_directories(${Boost_INCLUDE_DIRS})

# Use Dynamic Tree
# Add the submodule's source directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src)

# Add the submodule's source files to your project
add_library(ST_Tree STATIC
    ${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src/ST_Tree.cpp
    ${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src/visualise.cpp
)
# Ensure -fPIC is used for the static library
set_target_properties(ST_Tree PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Add the executable
# Add the source files
set(SOURCES
    main.cpp
)
add_executable(main_out ${SOURCES})
target_link_libraries(main_out PRIVATE ${Boost_LIBRARIES} ST_Tree)


# # Add test
# set(TEST_SOURCES
#     test/test_grade.cpp
# )
# add_executable(test_grade ${TEST_SOURCES})
# target_link_libraries(test_grade PRIVATE ${Boost_LIBRARIES} ST_Tree)

# -------------------------------------------------
# build binding for python 
add_subdirectory(binding)

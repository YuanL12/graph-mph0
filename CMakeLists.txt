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

# if want to use perf to profile
option(PERF_PROFILE "Enable perf profiling" OFF)
if(PERF_PROFILE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()


# Include all hpp files
include_directories(${CMAKE_SOURCE_DIR}/include)
# Find Boost and Include Boost headers
set(BOOST_ROOT /usr)
set(Boost_NO_SYSTEM_PATHS ON)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)


find_package(Boost REQUIRED)
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_FOUND: ${Boost_FOUND}")

include_directories(${Boost_INCLUDE_DIRS})

option(MPH0_TIMERS "Enable MPH0 timing code" OFF)
find_package(Boost OPTIONAL_COMPONENTS timer)
if(Boost_TIMER_FOUND AND MPH0_TIMERS)
    add_compile_options(-DMPH0_TIMERS=1)
    link_libraries(Boost::timer)
endif()

# Use Dynamic Tree
# Add the submodule's source directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src)
add_library(ST_Tree STATIC
    ${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src/ST_Tree.cpp
    ${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src/visualise.cpp
)
# Ensure -fPIC is used for the static library
set_target_properties(ST_Tree PROPERTIES POSITION_INDEPENDENT_CODE ON)

# use the top tree library
# define its include path
set(TOP_TREE_PATH ${CMAKE_SOURCE_DIR}/toptreelib)

# -------------------------------------------------
# Add the cpp executables
add_executable(topTree_out test/topTree.cpp)
target_include_directories(topTree_out PRIVATE ${TOP_TREE_PATH})
target_link_libraries(topTree_out PRIVATE ${Boost_LIBRARIES} ST_Tree)

add_executable(write_to_firep test/write_to_firep.cpp)
target_include_directories(write_to_firep PRIVATE ${TOP_TREE_PATH})
target_link_libraries(write_to_firep PRIVATE ${Boost_LIBRARIES})


# add_executable(main main.cpp)
# target_include_directories(main PRIVATE ${TOP_TREE_PATH})
# target_link_libraries(main PRIVATE ${Boost_LIBRARIES} ST_Tree)

# -------------------------------------------------
# build binding for python 
# add_subdirectory(binding)

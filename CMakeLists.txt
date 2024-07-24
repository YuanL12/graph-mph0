cmake_minimum_required(VERSION 3.10)

# Project name
project(MPH0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Boost
find_package(Boost REQUIRED)

# Include Boost headers
include_directories(${Boost_INCLUDE_DIRS})

# Add the executable
# Add the source files
set(SOURCES
    graph.cpp
    main.cpp
)
add_executable(out ${SOURCES})

# Link Boost libraries
target_link_libraries(out ${Boost_LIBRARIES})
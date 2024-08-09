cmake_minimum_required(VERSION 3.10)

# Project name
project(MPH0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Boost
find_package(Boost REQUIRED)

# Include Boost headers
include_directories(${Boost_INCLUDE_DIRS})


# Use Dynamic Tree
# Add the submodule's source directory to the include path
include_directories(${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the submodule's source files to your project
add_library(ST_Tree STATIC
    ${CMAKE_SOURCE_DIR}/DynamicTrees_NF/ST_Tree/src/ST_Tree.cpp
)


# Add the executable
# Add the source files
set(SOURCES
    main.cpp
)
add_executable(out ${SOURCES})

# Link libraries
target_link_libraries(out PRIVATE ${Boost_LIBRARIES} ST_Tree)

# add_executable(test test.cpp)
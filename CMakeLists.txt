cmake_minimum_required(VERSION 3.10)

# Project name
project(MPH0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# To run debug mode (eg. in vscode)
# set(CMAKE_BUILD_TYPE Debug)

# Include all hpp files
include_directories(${CMAKE_SOURCE_DIR}/include)
# Find Boost and Include Boost headers
find_package(Boost REQUIRED)
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
add_executable(out ${SOURCES})

# Link libraries
target_link_libraries(out PRIVATE ${Boost_LIBRARIES} ST_Tree)

# -------------------------------------------------
# build binding for python 
add_subdirectory(binding)

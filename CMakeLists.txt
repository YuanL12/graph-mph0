cmake_minimum_required(VERSION 3.20)

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

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})

option(MPH0_TIMERS "Enable MPH0 timing code" OFF)
find_package(Boost REQUIRED OPTIONAL_COMPONENTS timer)
if(Boost_TIMER_FOUND AND MPH0_TIMERS)
    add_compile_options(-DMPH0_TIMERS=1)
    link_libraries(Boost::timer)
endif()

# Add Graph_MPH cpp library
file(GLOB_RECURSE Graph_MPH_SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_library(Graph_MPH STATIC ${Graph_MPH_SRC_FILES})
# Ensure -fPIC is used for the static library
set_target_properties(Graph_MPH PROPERTIES POSITION_INDEPENDENT_CODE ON)

# use the top tree library
# define its include path
set(TOP_TREE_PATH ${CMAKE_SOURCE_DIR}/toptreelib)

# -------------------------------------------------
# Add the cpp executables
add_executable(time_point_cloud test/cpp/time_point_cloud.cpp)
target_include_directories(time_point_cloud PRIVATE ${TOP_TREE_PATH})
target_link_libraries(time_point_cloud PRIVATE ${Boost_LIBRARIES} Graph_MPH)

add_executable(time_firep test/cpp/time_firep.cpp)
target_include_directories(time_firep PRIVATE ${TOP_TREE_PATH})
target_link_libraries(time_firep PRIVATE ${Boost_LIBRARIES} Graph_MPH)

add_executable(write_to_firep test/cpp/write_to_firep.cpp)
target_include_directories(write_to_firep PRIVATE ${TOP_TREE_PATH})
target_link_libraries(write_to_firep PRIVATE ${Boost_LIBRARIES} Graph_MPH)

add_executable(read_firep test/cpp/read_firep.cpp)
target_include_directories(read_firep PRIVATE ${TOP_TREE_PATH})
target_link_libraries(read_firep PRIVATE ${Boost_LIBRARIES} Graph_MPH)

add_executable(rational test/cpp/rational.cpp)
target_include_directories(rational PRIVATE ${TOP_TREE_PATH})
target_link_libraries(rational PRIVATE ${Boost_LIBRARIES} Graph_MPH)

add_executable(main test/cpp/main.cpp)
target_include_directories(main PRIVATE ${TOP_TREE_PATH})
target_link_libraries(main PRIVATE ${Boost_LIBRARIES} Graph_MPH)

add_executable(test_ball_density test/cpp/test_ball_density.cpp)
target_include_directories(test_ball_density PRIVATE ${TOP_TREE_PATH})
target_link_libraries(test_ball_density PRIVATE ${Boost_LIBRARIES} Graph_MPH)


# -------------------------------------------------
# Add test target that runs the 4 commands and Python comparison
add_custom_target(run_tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running test commands..."
    COMMAND ${CMAKE_BINARY_DIR}/main /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt ball_density_rational --output annulus_200_ball_density_exact_our_out.txt
    COMMAND ${CMAKE_BINARY_DIR}/main /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_400.txt ball_density_rational --output annulus_400_ball_density_exact_our_out.txt 
    COMMAND ${CMAKE_BINARY_DIR}/main /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_200.txt degree_rational --output annulus_200_degree_exact_our_out.txt
    COMMAND ${CMAKE_BINARY_DIR}/main /home/yluo/Documents/graph-mph0/experiment/data/PointCloud/annulus_400.txt degree_rational --output annulus_400_degree_exact_our_out.txt
    COMMAND ${CMAKE_COMMAND} -E echo "Running Python comparison script..."
    COMMAND python3 ${CMAKE_SOURCE_DIR}/test/python/read_and_compare_rivet.py
    DEPENDS main
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all test commands and Python comparison"
)

# ----------------- Python binding -----------------
option(BUILD_PYTHON "Build Python bindings with pybind11" OFF)
if(BUILD_PYTHON)
    add_subdirectory(binding)
endif()

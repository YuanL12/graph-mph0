# # Manually set the Python executable and library paths
# set(PYTHON_EXECUTABLE /root/miniconda3/envs/dl/bin/python)

# find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# # Ensure CMake uses the specified Python executable
# set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})

# # Optionally, you can print the paths to confirm
# message(STATUS "Python executable: ${Python3_EXECUTABLE}")
# message(STATUS "Python library: ${Python3_LIBRARIES}")

# Copy the abmph folder into the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/abmph DESTINATION ${CMAKE_BINARY_DIR})

# load pybind11
add_subdirectory(pybind11)

# create binary file 
pybind11_add_module(_abmph abmph.cpp)
set_target_properties(_abmph PROPERTIES OUTPUT_NAME _abmph) # set the output name
set_target_properties(_abmph PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/abmph)
target_link_libraries(_abmph PRIVATE ST_Tree)
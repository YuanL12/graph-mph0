# If you use Vscode and want to use the debug feature 
# you need to 
# 1. manually set the Python executable and library paths here
# 2. set the debug mode of CMake 
# 3. set the launch.jason 
# set(PYTHON_EXECUTABLE /root/miniconda3/envs/pyg/bin/python)
# set(PYTHON_LIBRARY /root/miniconda3/envs/pyg/bin/python)

# Optionally, you can print the paths to confirm
message(STATUS "Caution: Python executable: ${PYTHON_EXECUTABLE}")
message(STATUS "Caution: Python library: ${PYTHON_EXECUTABLE}")

# Copy the abmph folder into the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/abmph DESTINATION ${CMAKE_BINARY_DIR})

# load pybind11
add_subdirectory(pybind11)

# create binary file 
pybind11_add_module(_abmph abmph.cpp)
set_target_properties(_abmph PROPERTIES OUTPUT_NAME _abmph) # set the output name
set_target_properties(_abmph PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/abmph)
target_link_libraries(_abmph PRIVATE ST_Tree)
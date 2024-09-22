# If you use Vscode and want to use the its botton to build/debug automatically 
# you need to manually set the Python executable and library paths here
# set(PYTHON_EXECUTABLE /root/miniconda3/envs/pyg/bin/python)
# set(PYTHON_LIBRARY /root/miniconda3/envs/pyg/bin/python)

# If you want to debug using VSCode, you will need to 
# 2. set the debug mode
# 3. write your launch.jason 

# You can print the paths to confirm
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
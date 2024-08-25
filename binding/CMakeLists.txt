# Copy the abmph folder into the build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/abmph DESTINATION ${CMAKE_BINARY_DIR})

# load pybind11
add_subdirectory(pybind11)

# create binary file 
pybind11_add_module(_abmph abmph.cpp)
set_target_properties(_abmph PROPERTIES OUTPUT_NAME _abmph) # set the output name
set_target_properties(_abmph PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/abmph)
target_link_libraries(_abmph PRIVATE ST_Tree)
# # pybind11 subproject
# find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
# find_package(pybind11 CONFIG REQUIRED)

# # Copy the abmph folder into the build directory as the package API
# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/abmph DESTINATION ${CMAKE_BINARY_DIR})

# # create binary file for python
# python_add_library(_abmph MODULE abmph.cpp WITH_SOABI)
# target_include_directories(_abmph PRIVATE ${TOP_TREE_PATH})
# target_link_libraries(_abmph PRIVATE ${Boost_LIBRARIES} AbMPH pybind11::headers)

# # Set the output directory for the .so file
# set_target_properties(_abmph PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/abmph)

# # The install directory is the output (wheel) directory (e.g. conda env package path)
# install(TARGETS _abmph DESTINATION abmph)
# install(FILES ${CMAKE_BINARY_DIR}/abmph/__init__.py DESTINATION abmph) # Destination is the final package name


# -------------------------------------------------------------
# pybind11 subproject
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# create binary file for python
python_add_library(_abmph MODULE abmph.cpp WITH_SOABI)
target_include_directories(_abmph PRIVATE ${TOP_TREE_PATH})
target_link_libraries(_abmph PRIVATE ${Boost_LIBRARIES} AbMPH pybind11::headers)

# The install directory is the output (wheel) directory (e.g. conda env package path)
install(TARGETS _abmph DESTINATION abmph)

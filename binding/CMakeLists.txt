# -------------------------------------------------------------
# pybind11 subproject
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# create binary file for python
python_add_library(_graph_mph MODULE graph_mph.cpp WITH_SOABI)
target_include_directories(_graph_mph PRIVATE ${TOP_TREE_PATH})
target_link_libraries(_graph_mph PRIVATE ${Boost_LIBRARIES} Graph_MPH pybind11::headers)

# The install directory is the output (wheel) directory (e.g. conda env package path)
install(TARGETS _graph_mph DESTINATION graph_mph)

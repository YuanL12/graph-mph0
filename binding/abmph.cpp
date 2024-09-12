#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <vector>
#include <tuple>
#include "MPH.hpp"

namespace py = pybind11;

// // Define the lambda function as a global variable
// auto to_pylist = [](const std::vector<R2>& vec) {
//     py::list py_list;
//     for (const auto& item : vec) {
//         py_list.append(py::make_tuple(item.x, item.y));
//     }
//     return py_list;
// };

// Example of adding print statements
auto to_pylist = [](const std::vector<R2>& vec) {
    py::list py_list;
    // std::cout << "Converting vector of size: " << vec.size() << std::endl;
    for (const auto& item : vec) {
        // std::cout << "Item: (" << item.x << ", " << item.y << ")" << std::endl;
        py_list.append(py::make_tuple(item.x, item.y));
    }
    return py_list;
};

void init_R2(py::module& m){
	py::class_<R2>(m, "R2")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def_readwrite("x", &R2::x)
        .def_readwrite("y", &R2::y)
        .def("__getitem__", [](const R2 &r2, int index) {
            return r2[index];
        });
		;
}

void init_graph(py::module& m){
	py::class_<Graph<R2>>(m, "Graph")
        .def(py::init<int>())  // Existing constructor
        .def(py::init([](const std::vector<int>& node_labels, 
                         const std::vector<std::vector<double>>& node_features_, 
                         const std::vector<std::vector<int>>& edges_input, 
                         const std::vector<std::vector<double>>& edge_features_) {
			// std::cout<< "call from the first constructor of graph" <<std::endl;
            // Convert node_features (2D array) to std::vector<R2>
            std::vector<R2> node_features;
            node_features.reserve(node_features_.size());
            for (const auto& inner_vec : node_features_) {
                if (inner_vec.size() == 2) {
                    node_features.emplace_back(inner_vec[0], inner_vec[1]);
                } else {
                    // Handle the case where inner_vec does not have exactly 2 elements
                    throw std::runtime_error("Each inner vector of node_features_ must have exactly 2 elements.");
                }
            }

            // Convert edges_input to std::vector<std::pair<int, int>>
            std::vector<std::pair<int, int>> edges;
            edges.reserve(edges_input.size());
            for (const auto& inner_vec : edges_input) {
                if (inner_vec.size() == 2) {
                    edges.emplace_back(inner_vec[0], inner_vec[1]);
                } else {
                    // Handle the case where inner_vec does not have exactly 2 elements
                    throw std::runtime_error("Each inner vector of edges must have exactly 2 elements.");
                }
            }
            
            // Convert edge_features (2D array) to std::vector<R2>
            std::vector<R2> edge_features;
            edge_features.reserve(edge_features_.size());
            for (const auto& inner_vec : edge_features_) {
                if (inner_vec.size() == 2) {
                    edge_features.emplace_back(inner_vec[0], inner_vec[1]);
                } else {
                    // Handle the case where inner_vec does not have exactly 2 elements
                    throw std::runtime_error("Each inner vector of edge_features must have exactly 2 elements.");
                }
            }

            // Call the Graph constructor
            return Graph<R2>(node_labels, node_features, edges, edge_features);
		}))
        .def("add_vertex", &Graph<R2>::add_vertex)
		.def("add_edge", py::overload_cast<int, int, R2>(&Graph<R2>::add_edge), "Add edge with filtration value specified")
		.def("add_edge", py::overload_cast<int, int>(&Graph<R2>::add_edge), "Add edge without filtration value")
		.def("print_adjacency", &Graph<R2>::print_adjacency)
        .def("print_filtration_value", &Graph<R2>::print_filtration_value)
		;
}

PYBIND11_MODULE(_abmph, m) {  
	m.doc() = "abmph Python package(version 0.1)\n"
			"This package provides utilities for computing absolute 2-parameter homology on graphs ";

	m.def("subtract", [](int i, int j) { return i - j; });
	init_graph(m);
	init_R2(m);

	m.def("test_pylist", []() {
		std::vector<R2> test_vec = {{1, 2}, {3, 4}};
		return to_pylist(test_vec);
	});
	


	m.def("compute_MPH0_Dengrogram_CXX", [](const Graph<R2>& g) {
            // Call the C++ function and unpack the tuple
            auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_Dengrogram<R2>(g);
            // Return the tuple to Python
			auto py_betti_0 = to_pylist(betti_0);
			auto py_betti_1 = to_pylist(betti_1);
			auto py_betti_2 = to_pylist(betti_2);
			auto py_betti_0_1 = to_pylist(betti_0_1);
			return std::make_tuple(py_betti_0, py_betti_1, py_betti_2, py_betti_0_1, M);
        }, "Compute the absolute MPH0 by Dendrogram and return the results as a tuple of vectors.");

	m.def("compute_MPH0_DTree_CXX", [](const Graph<R2>& g) {
            // Call the C++ function and unpack the tuple
            auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
			auto py_betti_0 = to_pylist(betti_0);
			auto py_betti_1 = to_pylist(betti_1);
			auto py_betti_2 = to_pylist(betti_2);
			auto py_betti_0_1 = to_pylist(betti_0_1);
			return std::make_tuple(py_betti_0, py_betti_1, py_betti_2, py_betti_0_1, M);
        }, "Compute the absolute MPH0 by Dynamic Tree and return the results as a tuple of vectors.");

    m.def("compute_MPH0_DTree_CXX_debug", [](const Graph<R2>& g) {
            // Call the C++ function and unpack the tuple
            auto [betti_0, betti_1, betti_2, betti_0_1, M, em] = compute_MPH0_DTree_debug<R2>(g);
			auto py_betti_0 = to_pylist(betti_0);
			auto py_betti_1 = to_pylist(betti_1);
			auto py_betti_2 = to_pylist(betti_2);
			auto py_betti_0_1 = to_pylist(betti_0_1);
			return std::make_tuple(py_betti_0, py_betti_1, py_betti_2, py_betti_0_1, M, em);
        }, "Compute the absolute MPH0 by Dynamic Tree and return the results as a tuple of vectors and simplices matching.");
}
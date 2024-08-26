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
    std::cout << "Converting vector of size: " << vec.size() << std::endl;
    for (const auto& item : vec) {
        std::cout << "Item: (" << item.x << ", " << item.y << ")" << std::endl;
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
		.def(py::init([](const py::array_t<int>& node_labels, 
                 const py::array_t<double>& node_features, 
                 const py::array_t<int>& edges_input, 
                 const py::array_t<double>& edge_features) {
			// Convert numpy arrays to std::vector
            auto node_labels_unchecked = node_labels.unchecked<1>();
            auto node_features_unchecked = node_features.unchecked<2>();
            auto edges_input_unchecked = edges_input.unchecked<2>();
            auto edge_features_unchecked = edge_features.unchecked<2>();

			std::vector<int> labels(node_labels_unchecked.size());
            for (ssize_t i = 0; i < node_labels_unchecked.size(); ++i) {
                labels[i] = node_labels_unchecked(i);
            }

			// std::cout << "labels:" << std::endl;
			// for (auto k: labels){
			// 	std::cout << " " << k;
			// }
			// std::cout<< std::endl;

			// Convert node_features (2D array) to std::vector<R2>
            std::vector<R2> features;
            features.reserve(node_features_unchecked.shape(0));
            for (ssize_t i = 0; i < node_features_unchecked.shape(0); ++i) {
                features.emplace_back(node_features_unchecked(i, 0), 
                                      node_features_unchecked(i, 1));
            }

			// std::cout << "features:" << std::endl;
			// for (auto k: features){
			// 	std::cout << " (" << k[0] << "," << k[1] << ")";
			// }
			// std::cout<< std::endl;

            // Convert edges_input to std::vector<std::pair<int, int>>
            std::vector<std::pair<int, int>> edges;
            for (ssize_t i = 0; i < edges_input_unchecked.shape(0); ++i) {
                edges.emplace_back(edges_input_unchecked(i, 0), 
                                   edges_input_unchecked(i, 1));
            }

			// std::cout << "edges:" << std::endl;
			// for (auto k: edges){
			// 	std::cout << " (" << k.first << "," << k.second << ")";
			// }
			// std::cout<< std::endl;


            // Convert edge_features (2D array) to std::vector<R2>
            std::vector<R2> edge_feats;
            edge_feats.reserve(edge_features_unchecked.shape(0));
            for (ssize_t i = 0; i < edge_features_unchecked.shape(0); ++i) {
                edge_feats.emplace_back(edge_features_unchecked(i, 0), 
                                        edge_features_unchecked(i, 1));
            }

            // Call the Graph constructor
            return Graph<R2>(labels, features, edges, edge_feats);
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
	


	m.def("compute_MPH0_Dengrogram", [](const Graph<R2>& g) {
            // Call the C++ function and unpack the tuple
            auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_Dengrogram<R2>(g);
            // Return the tuple to Python
			auto py_betti_0 = to_pylist(betti_0);
			auto py_betti_1 = to_pylist(betti_1);
			auto py_betti_2 = to_pylist(betti_2);
			auto py_betti_0_1 = to_pylist(betti_0_1);
			return std::make_tuple(py_betti_0, py_betti_1, py_betti_2, py_betti_0_1, M);
            // return std::make_tuple( to_pylist(betti_0), 
			// 						to_pylist(betti_1), 
			// 						to_pylist(betti_2), 
			// 						to_pylist(betti_0_1), 
			// 						M);
        }, "Compute the absolute MPH0 by Dendrogram and return the results as a tuple of vectors.");

	m.def("compute_MPH0_DTree", [](const Graph<R2>& g) {
            // Call the C++ function and unpack the tuple
			std::cout << "Into the fucntion compute_MPH0_DTree in Pybind11" << std::endl;
            auto [betti_0, betti_1, betti_2, betti_0_1, M] = compute_MPH0_DTree<R2>(g);
            std::cout << "Finish compute_MPH0_DTree, and return to Pybind11 " << std::endl;
			auto py_betti_0 = to_pylist(betti_0);
			auto py_betti_1 = to_pylist(betti_1);
			auto py_betti_2 = to_pylist(betti_2);
			auto py_betti_0_1 = to_pylist(betti_0_1);
			return std::make_tuple(py_betti_0, py_betti_1, py_betti_2, py_betti_0_1, M);
        }, "Compute the absolute MPH0 by Dynamic Tree and return the results as a tuple of vectors.");
}
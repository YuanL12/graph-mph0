#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Filtration.hpp"
#include "MPH.hpp"
#include "RIVET.hpp"

namespace py = pybind11;

// Helper function to convert rivet::ExactValue to string
std::string exact_value_to_string(const rivet::ExactValue& ev) {
    std::ostringstream oss;
    oss << ev.exact_value;
    return oss.str();
}

// Helper function to convert 2D Python array to std::vector<std::vector<double>>
std::vector<std::vector<double>> convert_2D_py_array(py::array_t<double> points_array) {
    auto buf = points_array.request();
    if (buf.ndim != 2) {
        throw std::runtime_error("Points array must be 2-dimensional (n_points, n_dimensions)");
    }

    size_t n_points = buf.shape[0];
    size_t n_dims = buf.shape[1];
    double* ptr = static_cast<double*>(buf.ptr);

    std::vector<std::vector<double>> points;
    points.reserve(n_points);

    // Check if array is C-contiguous or F-contiguous
    bool is_c_contiguous = points_array.flags() & py::array::c_style;

    for (size_t i = 0; i < n_points; ++i) {
        std::vector<double> point;
        point.reserve(n_dims);
        for (size_t j = 0; j < n_dims; ++j) {
            if (is_c_contiguous) {
                // C-contiguous: row-major order
                std::cout << "C-contiguous" << std::endl;
                point.push_back(ptr[i * n_dims + j]);
            } else {
                // F-contiguous: column-major order
                std::cout << "F-contiguous" << std::endl;
                point.push_back(ptr[j * n_points + i]);
            }
        }
        points.push_back(std::move(point));
    }

    return points;
}

// Helper function to convert 1D Python array to std::vector<double>
std::vector<double> convert_1D_py_array(py::array_t<double> one_dim_array) {
    auto buf = one_dim_array.request();
    if (buf.ndim != 1) {
        throw std::runtime_error("one_dim_array must be 1-dimensional");
    }

    size_t n_values = buf.shape[0];
    double* ptr = static_cast<double*>(buf.ptr);
    std::vector<double> values(ptr, ptr + n_values);
    return values;  // no need to check if it is C-contiguous or F-contiguous
}

// Helper function to convert CoordsVariant to py::object
py::object coords_variant_wrapper(const CoordsVariant& coords) {
    return std::visit(
        [](auto&& coord_vec) -> py::object {
            using T = std::decay_t<decltype(coord_vec)>;
            if constexpr (std::is_same_v<T, std::vector<double>>) {
                return py::cast(coord_vec);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                return py::cast(coord_vec);
            } else if constexpr (std::is_same_v<T, std::vector<rivet::ExactValue>>) {
                std::vector<std::string> str_coords;
                str_coords.reserve(coord_vec.size());
                for (const auto& ev : coord_vec) {
                    str_coords.push_back(exact_value_to_string(ev));
                }
                return py::cast(str_coords);
            } else {
                throw std::runtime_error(
                    "Trying to convert an unknown CoordsVariant type, expected "
                    "std::vector<double>, std::vector<int>, "
                    "or std::vector<rivet::ExactValue>, got " +
                    std::string(typeid(T).name()));
            }
        },
        coords);
}

void init_GGraph(py::module& m) {
    py::class_<GGraph>(m, "GGraph")
        .def(py::init<int>(), py::arg("n"))
        .def("get_nvertices", &GGraph::get_nvertices)
        .def("get_nedges", &GGraph::get_nedges);
}

void init_bifiltration(py::module& m) {
    py::class_<BiFiltration>(m, "BiFiltration")
        .def(
            py::init<const std::string&, const std::string&>(), py::arg("filtration_type"),
            py::arg("file_path"),
            "Initialize a BiFiltration reading from a file by providing a specific filtration type")
        .def(py::init<const std::vector<std::vector<double>>&, const std::string&>(),
             py::arg("points"), py::arg("filtration_type"),
             "Initialize a BiFiltration from a 2D numpy array of points (shape: n_points, "
             "n_dimensions)")
        .def(py::init<const std::vector<std::vector<double>>&, std::vector<double>&,
                      std::optional<double>, std::optional<double>>(),
             py::arg("points"), py::arg("function_values"),
             py::arg("max_function_value") = std::nullopt, py::arg("max_distance") = std::nullopt,
             "Initialize a Function-Rips BiFiltration from a 2D numpy array of points and function "
             "values")
        .def_readwrite("ggraph", &BiFiltration::ggraph, "Grade Graph")
        .def(
            "get_x_coords",
            [](const BiFiltration& bf) { return coords_variant_wrapper(bf.get_x_coords()); },
            py::return_value_policy::reference,
            "Get the x coordinates of the grade table (converts rivet::ExactValue to strings)")
        .def(
            "get_y_coords",
            [](const BiFiltration& bf) { return coords_variant_wrapper(bf.get_y_coords()); },
            py::return_value_policy::reference,
            "Get the y coordinates of the grade table (converts rivet::ExactValue to strings)");
}

PYBIND11_MODULE(_graph_mph, m) {
    m.doc() =
        "graph_mph Python package(version 0.0.1)\n"
        "This package provides utilities for computing absolute 2-parameter absolute Betti numbers "
        "on graphs ";

    // classes
    init_GGraph(m);
    init_bifiltration(m);

    // functions
    m.def("subtract", [](int i, int j) { return i - j; });
    m.def("compute_MPH0_CXX", &compute_MPH0);
}
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
            py::arg("path"),
            "Initialize a BiFiltration reading from a file by providing a specific filtration type")
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

PYBIND11_MODULE(_abmph, m) {
    m.doc() =
        "abmph Python package(version 0.0.1)\n"
        "This package provides utilities for computing absolute 2-parameter absolute Betti numbers "
        "on graphs ";

    // classes
    init_GGraph(m);
    init_bifiltration(m);

    // functions
    m.def("subtract", [](int i, int j) { return i - j; });
    m.def("compute_MPH0_CXX", &compute_MPH0);
}
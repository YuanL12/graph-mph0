
#include <pybind11/pybind11.h>
#include "basic.hpp"
#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
#define VERSION 0.0
namespace py = pybind11;
PYBIND11_MODULE(_abmph, m) {  
    m.attr("__version__") = MACRO_STRINGIFY(VERSION);
	m.def("add", &add);
	m.def("subtract", [](int i, int j) { return i - j; });
	py::class_<Pet>(m, "Pet")
		.def(py::init<const std::string &>())
		.def_property("name", &Pet::getName, &Pet::setName)
		.def_property("age", &Pet::getAge, &Pet::setAge)
		.def("setName", &Pet::setName)
		.def("getName", &Pet::getName)
		.def("__repr__", [](const Pet &a) {
			return "<example.Pet named '" + a.name + "'>";
			})
		.def("set", py::overload_cast<int>(&Pet::set), "Set the pet's age")
		.def("set", py::overload_cast<const std::string &>(&Pet::set), "Set the pet's name");

	py::class_<Dog, Pet>(m, "Dog")
		.def(py::init<const std::string &>())
		.def("bark", &Dog::bark);
}
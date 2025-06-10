#pragma once
#include <functional>
#include <boost/multiprecision/cpp_int.hpp>
#include "Types.hpp"

// General template for filtration type(Poset) hash functor
template <typename T>
struct FTHash {
    std::size_t operator()(const T& t) const {
        return std::hash<T>{}(t);
    }
};

// Hash function for boost::multiprecision::cpp_int
namespace std {
    template<>
    struct hash<boost::multiprecision::cpp_int> {
        size_t operator()(const boost::multiprecision::cpp_int& n) const {
            // Convert to string and hash the string representation
            std::string s = n.str();
            return std::hash<std::string>{}(s);
        }
    };
}

// Hash function for Vertex
struct VertexHash {
    size_t operator()(const Vertex& v) const {
        return std::hash<VertexId>{}(v.id);
    }
};
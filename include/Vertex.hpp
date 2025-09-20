#pragma once
#include <iostream>

#include "Types.hpp"

struct Vertex {
    VertexId id;  // -1 means null vertex due to removal
    int label;    // label of the vertex
    Vertex() : id(-1), label(-1) {}
    Vertex(VertexId id, int label) : id(id), label(label) {}

    // equality operator
    bool operator==(const Vertex &other) const {
        return id == other.id && label == other.label;
    }

    // << operator
    friend std::ostream &operator<<(std::ostream &os, const Vertex &v) {
        os << v.label;
        return os;
    }
};
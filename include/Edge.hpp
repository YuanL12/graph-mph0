#pragma once
#include <iostream>
#include <limits>
#include <tuple>

// Define type
using EdgeId = int;
using VertexID = size_t;
using Vertex = int;

class Edge {
public:
    Edge(){}
    // Constructor
    Edge(Vertex v0, Vertex v1, EdgeId id): v0(v0), v1(v1), id(id) {}

    // Accessor methods
    Vertex get_v0() const { return v0; }
    Vertex get_v1() const { return v1; }
    EdgeId get_id() const { return id; }

    Vertex& get_v0() { return v0; }
    Vertex& get_v1() { return v1; }
    EdgeId& get_id() { return id; }


    // Overload the output operator
    friend std::ostream& operator<<(std::ostream& os, const Edge& edge) {
        os << "Edge(" << edge.v0 << ", " << edge.v1 << ", id = " << edge.id << ")";
        return os;
    }

    // Null edge constant
    static const Edge NULL_EDGE;

    // Overload the index operator
    int operator[](int index) const {
        switch (index) {
            case 0: return v0;
            case 1: return v1;
            default: throw std::out_of_range("Index out of range for Edge");
        }
    }

    // Overload the index operator (non-const version for write access)
    int& operator[](int index) {
        switch (index) {
            case 0: return v0;
            case 1: return v1;
            default: throw std::out_of_range("Index out of range for Edge");
        }
    }

    bool operator==(const Edge& other) const {
        return v0 == other.v0 && v1 == other.v1 && id == other.id;
    }

    bool operator!=(const Edge& other) const {
        return !(*this == other);
    }

private:
    Vertex v0;
    Vertex v1;
    EdgeId id;
};

// Define NULL_EDGE
const Edge Edge::NULL_EDGE = Edge(
    std::numeric_limits<Vertex>::max(), 
    std::numeric_limits<Vertex>::max(), 
    std::numeric_limits<EdgeId>::max()
);

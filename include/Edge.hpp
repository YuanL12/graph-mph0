#pragma once
#include <iostream>
#include <limits>
#include <tuple>

#include "Types.hpp"

// Edge: an edge in the graph
class Edge {
   public:
    Edge() {}
    // Constructor
    Edge(VertexId v0, VertexId v1, EdgeId id) : v0(v0), v1(v1), id(id) {}

    // Accessor methods
    VertexId get_v0() const { return v0; }
    VertexId get_v1() const { return v1; }
    EdgeId get_id() const { return id; }

    VertexId &get_v0() { return v0; }
    VertexId &get_v1() { return v1; }
    EdgeId &get_id() { return id; }

    // Setter methods
    void set_v0(VertexId v0) { this->v0 = v0; }
    void set_v1(VertexId v1) { this->v1 = v1; }
    void set_id(EdgeId id) { this->id = id; }

    // Overload the output operator
    friend std::ostream &operator<<(std::ostream &os, const Edge &edge) {
        os << "Edge(" << edge.v0 << ", " << edge.v1 << ", id = " << edge.id << ")";
        return os;
    }

    // Null edge constant
    static const Edge NULL_EDGE;

    // Overload the index operator
    VertexId operator[](int index) const {
        switch (index) {
            case 0:
                return v0;
            case 1:
                return v1;
            default:
                throw std::out_of_range("Index out of range for Edge");
        }
    }

    // Overload the index operator (non-const version for write access)
    VertexId &operator[](int index) {
        switch (index) {
            case 0:
                return v0;
            case 1:
                return v1;
            default:
                throw std::out_of_range("Index out of range for Edge");
        }
    }

    bool operator==(const Edge &other) const {
        return v0 == other.v0 && v1 == other.v1 && id == other.id;
    }

    bool operator!=(const Edge &other) const { return !(*this == other); }

   private:
    VertexId v0;
    VertexId v1;
    EdgeId id;  // unique id for the edge, -1 for removed edge, -2 for null edge (used for
                // collapsing)
};

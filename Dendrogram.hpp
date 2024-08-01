#pragma once

#include "Edge.hpp"

template<typename T>
class Dendrogram
{
private:
    /* data */
    std::vector<Vertex> vertices;
    std::unordered_map<Vertex, T> vert_values; 
public:
    Dendrogram(const std::vector<Vertex>& vertices_){
        vertices = vertices_;
        size_t n = vertices.size();
        vert_values.assign(n, 0);
    };

    // returns the smallest t ∈ [0, ∞) such that [v] = [w] ∈ π0(G,f)(r), 
    // or ∞ if [v] != [w] ∈ π0(G,f)(r) for all r ∈ [0,∞).
    T time_of_merge(Vertex v, Vertex w); // nearest common ancestor (nca)
    void merge_at_time(EdgeId eid, T time); // merge and update nca
};


template<typename T>
T Dendrogram<T>::time_of_merge(Vertex v, Vertex w){
    T f_value;
    return f_value;
}


template<typename T>
void Dendrogram<T>::merge_at_time(EdgeId eid, T t){
    
}
#pragma once
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <limits>
#include <icecream.hpp>
#include <utility> // for std::pair
#include <cstddef> // for std::size_t
#include <functional> // for std::hash
#include "Types.hpp"
#include "ContractionTopTree.hpp"
#include "Hash.hpp"

// Custom hash function for std::pair<int, int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        // Combine the hash of the two elements in the pair
        return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
    }
};


/*
    Dynamic tree using top tree that supports finding the minimal edge weight. 
    We use negative time to represent the edge weight,
    because we need to locate edge with the latest time in a c.c. (i.e., a cluster). 
*/
template<typename T, typename VertexType = Vertex, typename HashFunction = VertexHash>
class DTree_TopTree
{
public:
    T max_edge_weight = std::numeric_limits<T>::max(); // maximum edge weight

    DTree_TopTree(const std::vector<VertexType>& vertices_){
        size_t i = 0; // TopTree uses 0-based indexing
        TTvertices.reserve(vertices_.size());
        for(const auto& v: vertices_){
            TTvertices.emplace_back(forest.newVertex());
            vert2node[v] = i;
            i++;
        }
    };

    void link(VertexType v, VertexType w, T time){
        if (forest.sameComponent(TTvertices[vert2node[v]], TTvertices[vert2node[w]])){
            throw std::runtime_error("try to conenct v and w in the same component, it is not allowed in a tree");
        }
        forest.link(TTvertices[vert2node[v]], TTvertices[vert2node[w]], -time);
    }

    T time_of_merge(VertexType v, VertexType w){
        // check if v and w are connected first
        if (!forest.sameComponent(TTvertices[vert2node[v]], TTvertices[vert2node[w]])){
            return max_edge_weight;
        }

        // if connected, find the min weight edge between v and w
        forest.expose(TTvertices[vert2node[v]], TTvertices[vert2node[w]]);
        return -forest.getExposedData().minWeight;
    }


    void merge_at_time(VertexType v, VertexType w, T time){
        // if v and w are connected first, we need to cut the min weight edge in the path between v and w
        if (forest.sameComponent(TTvertices[vert2node[v]], TTvertices[vert2node[w]])){
            cutMinEdge(v, w);
        }
        forest.link(TTvertices[vert2node[v]], TTvertices[vert2node[w]], -time);
    }

private:

    // search for minimal edge in the path from v to w and cut it
    void cutMinEdge(VertexType v, VertexType w){
        // expose the path from v to w
        forest.expose(TTvertices[vert2node[v]], TTvertices[vert2node[w]]);
        // search for minimal edge in the path from v to w
        forest.pathSearch([](auto eventData) {
            return eventData.children[0].minWeight > eventData.children[1].minWeight;
        });
        // locate the two end points of the minimal edge
        auto [e0, e1] = forest.getBoundary();
        // cut it 
        forest.cut(e0, e1);
    }

    struct ClusterMinEdgeData {
        int minWeight; // minimal weight on path (or weight of an edge)

        ClusterMinEdgeData(int weight) : minWeight(weight) {}
        ClusterMinEdgeData() = default;

        // Increase weights of all edges on the path (to be propagated later)
        void incWeightsOnPath(int delta) {
            minWeight += delta;
        }

        // Aggregate minimal weight of an edge in the cluster
        static void join(TopTreeEventData<ClusterMinEdgeData> eventData) {
            if (eventData.type == TopTreeClusterType::COMPRESS) {

                eventData.parent.minWeight =
                    std::min(eventData.children[0].minWeight,
                        eventData.children[1].minWeight);

            } else { // TopTreeClusterType::RAKE

                eventData.parent.minWeight = eventData.children[0].minWeight;

            }
        }

        // Compute how minWeight was changed and propagate the change to subclusters
        static void split(TopTreeEventData<ClusterMinEdgeData> eventData) {
            if (eventData.type == TopTreeClusterType::COMPRESS) {

                int delta =
                    eventData.parent.minWeight -
                    std::min(eventData.children[0].minWeight,
                        eventData.children[1].minWeight);
                eventData.children[0].minWeight += delta;
                eventData.children[1].minWeight += delta;

            } else { // TopTreeClusterType::RAKE

                int delta = eventData.parent.minWeight - eventData.children[0].minWeight;
                eventData.children[0].minWeight += delta;

            }
        }

    };

    ContractionTopTree<ClusterMinEdgeData> forest;
    std::unordered_map<VertexType, size_t, HashFunction> vert2node; // map vertex to tree node label
    std::vector<TopTreeVertex> TTvertices; // vertices of the top tree
};
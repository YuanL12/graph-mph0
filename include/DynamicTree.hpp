#pragma once
#include "ST_Tree.hpp"
#include "visualise.hpp"
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <memory>
#include <vector>
#include <algorithm>
#include <limits>
#include <icecream.hpp>
using Vertex = int;

/* 
Terminology: vertex for input complex, and node for tree
Notes for using Dynamic Tree for MPH_0 consideration
1. Use negative time because ST supports mincost
2. time_of_merge and merge_at_time both looks for the highest edge,
3. merge_at_time will remove it only if currect edge time is less than the highest
*/
template<typename T>
class DynamicTree
{
private:
    std::unordered_map<Vertex, int> vert2node; // map vertex to tree node label
    ST_Tree ST;
    GraphManager graph_manager;
public:
    // Construct with vertices
    DynamicTree(const std::vector<Vertex>& vertices_)
        : ST(true, vertices_.size(), 0),   // Initialize ST_Tree directly in the initializer list
          graph_manager(vertices_.size()){
        int i = 1;
        for(const auto& v: vertices_){
            vert2node[v] = i;
            i++;
        }
        int op_num = 0;
    };


    // T time_of_merge(Vertex v, Vertex w);
    T time_of_merge_double(Vertex v, Vertex w){
        ST.evert(vert2node[v]); // make v the root of the tree containing v
        int rt_w = ST.root(vert2node[w]);// root of w
        // if not in the same tree, i.e., root of w is not v
        if (vert2node[v] != rt_w) return std::numeric_limits<T>::max();
        // if in the same tree
        int mincost_node = ST.mincost(vert2node[w]);
        return -ST.cost(mincost_node);
    };
    
    void merge_at_time(Vertex v, Vertex w, T time){
        // key assumption: v has been the root of the tree containing v
        // if they are in the separte tree
        if (ST.root(vert2node[w]) !=  vert2node[v]){
            // make sure w is the root for safety link, o.w. w can have multiple parents which is terrible
            ST.evert(vert2node[w]); 
             // make v the parent of w
            ST.link(vert2node[w], vert2node[v], -time);
        }else{ // same tree
            int mincost_node = ST.mincost(vert2node[w]);
            T time_of_merge = -ST.cost(mincost_node);
            if (time_of_merge <= time){ // no need to update the tree 
                return;
            }else{ // current time (edge weight) < time of merge (max one in the tree), need to update/remove the max one 
                // cut the max edge first 
                ST.cut(mincost_node);
                // make sure w is the root for safety link, o.w. w can have multiple parents
                ST.evert(vert2node[w]); 
                // link the edge v and w, such that v is the parent of w 
                ST.link(vert2node[w], vert2node[v], -time);
            }
        }
    }; 
};
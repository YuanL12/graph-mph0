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
#include <utility> // for std::pair
#include <cstddef> // for std::size_t
#include <functional> // for std::hash
#include "Types.hpp"

// Custom hash function for std::pair<int, int>
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& p) const {
        // Combine the hash of the two elements in the pair
        return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second);
    }
};



/* 
Assumption:
Input time (or edge weight) has to be non-negative and then this class will negate to negatives, 
because we will need mincost() in ST_Tree. 

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
    std::unordered_map<std::pair<int, int>, int, pair_hash> vertex2edgeID;
    ST_Tree ST;
    GraphManager graph_manager;
    int op_num = 0;
    bool visual = false; // wheather show the graph after each operation
    std::string graph_save_folder_name = "DT_graph_results";
    const int null_edge_id = -1;

public:
    double max_edge_weight = 1e10; // maximum edge weight
    
    // Construct with vertices
    DynamicTree(const std::vector<Vertex>& vertices_, bool visual_ = false)
        : ST(true, vertices_.size(), 0),   // Initialize ST_Tree directly in the initializer list
          graph_manager(vertices_.size()),
          visual(visual_){
        int i = 1;
        for(const auto& v: vertices_){
            vert2node[v] = i;
            i++;
        }
        if (visual){
            std::cout << "call the remove and mkdir function" << std::endl;
            std::string command = "rm -rf " + graph_save_folder_name + " && mkdir " + graph_save_folder_name;
            system(command.c_str());
        }
    };

    void update_max_edge_weight(T y){
        if (y > max_edge_weight){
            max_edge_weight = y + 10;
        }
    }

    // T time_of_merge(Vertex v, Vertex w);
    T time_of_merge_double(Vertex v, Vertex w, bool visual = false){
        ST.evert(vert2node[v]); // make v the root of the tree containing v
        if(visual) {
            std::string op_name = "evert_"+std::to_string(vert2node[v]);
            displayGraph(op_name);
        }
        
        int rt_w = ST.root(vert2node[w]);// root of w
        // if not in the same tree, i.e., root of w is not v
        if (vert2node[v] != rt_w) return max_edge_weight;
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
            if(visual) {
                std::string op_name = "evert_"+std::to_string(vert2node[w]);
                displayGraph(op_name);
            }

            // make v the parent of w
            ST.link(vert2node[w], vert2node[v], -time);
            if(visual){
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(3) << -time;
                std::string op_name = "link_"+std::to_string(vert2node[w])+
                                        "_" + std::to_string(vert2node[v]) +
                                        "_" + oss.str();
                displayGraph(op_name); 
            }
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
                if(visual) {
                    std::string op_name = "evert_"+std::to_string(vert2node[w]);
                    displayGraph(op_name); 
                }
                // link the edge v and w, such that v is the parent of w 
                ST.link(vert2node[w], vert2node[v], -time);
                if(visual){
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(3) << -time;
                    std::string op_name = "link_"+std::to_string(vert2node[w])+
                                            "_" + std::to_string(vert2node[v]) +
                                            "_" + oss.str();
                    displayGraph(op_name); 
                }
            }
        }
    }; 

    // the folloiwng two debug versions can give edge matchings used in betti_1 and betti_2
    std::pair<T, int> time_of_merge_double_debug(Vertex v, Vertex w, bool visual = false){
        ST.evert(vert2node[v]); // make v the root of the tree containing v
        if(visual) {
            std::string op_name = "evert_"+std::to_string(vert2node[v]);
            displayGraph(op_name);
        }
        
        int rt_w = ST.root(vert2node[w]);// root of w
        // if not in the same tree, i.e., root of w is not v
        if (vert2node[v] != rt_w) {
            // a tree can have n-1 edges, return a larger one is enough for null edge 
            return std::make_pair(max_edge_weight, null_edge_id);
        }
        // if in the same tree
        int mincost_node = ST.mincost(vert2node[w]);
        int e0, e1;
        std::tie(e0, e1) = std::minmax(mincost_node, ST.parent(mincost_node));
        int eid = vertex2edgeID[std::make_pair(e0, e1)];
        return std::make_pair(-ST.cost(mincost_node), eid);
    };


    // return the id of edge id used to  too
    void merge_at_time_debug(Vertex v, Vertex w, T time, int eid){
        // key assumption: v has been the root of the tree containing v
        // if they are in the separte tree
        int e0, e1;
        if (ST.root(vert2node[w]) !=  vert2node[v]){
            // make sure w is the root for safety link, o.w. w can have multiple parents which is terrible
            ST.evert(vert2node[w]);
            if(visual) {
                std::string op_name = "evert_"+std::to_string(vert2node[w]);
                displayGraph(op_name);
            }

            // make v the parent of w
            ST.link(vert2node[w], vert2node[v], -time);
            std::tie(e0, e1) = std::minmax(vert2node[w], vert2node[v]);
            vertex2edgeID[std::make_pair(e0, e1)] = eid;
            if(visual){
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(3) << -time;
                std::string op_name = "link_"+std::to_string(vert2node[w])+
                                        "_" + std::to_string(vert2node[v]) +
                                        "_" + oss.str();
                displayGraph(op_name); 
            }
        }else{ // same tree
            int mincost_node = ST.mincost(vert2node[w]);
            T time_of_merge = -ST.cost(mincost_node);
            if (time_of_merge <= time){ // no need to update the tree 
                return;
            }else{ // current time (edge weight) < time of merge (max one in the tree), need to update/remove the max one 
                // cut the max edge first 
                ST.cut(mincost_node);
                // update the vertex2edgeID map
                std::tie(e0, e1) = std::minmax(mincost_node, ST.parent(mincost_node));
                vertex2edgeID.erase(std::make_pair(e0, e1));

                // make sure w is the root for safety link, o.w. w can have multiple parents
                ST.evert(vert2node[w]); 
                if(visual) {
                    std::string op_name = "evert_"+std::to_string(vert2node[w]);
                    displayGraph(op_name); 
                }
                // link the edge v and w, such that v is the parent of w 
                ST.link(vert2node[w], vert2node[v], -time);
                std::tie(e0, e1) = std::minmax(vert2node[w], vert2node[v]);
                vertex2edgeID[std::make_pair(e0, e1)] = eid;

                if(visual){
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(3) << -time;
                    std::string op_name = "link_"+std::to_string(vert2node[w])+
                                            "_" + std::to_string(vert2node[v]) +
                                            "_" + oss.str();
                    displayGraph(op_name); 
                }
            }
        }
    }; 

    // only show the integer part of edge weight
    void displayGraph(std::string operation_name){
        std::string filename = graph_save_folder_name+ "/"+std::to_string(++op_num)+ "_" + operation_name;
        std::vector<std::vector<int> > boldEdges = ST.getAllEdges(); // get all bold edges
        std::vector<std::vector<int> > dashedEdges = ST.getAllDashEdges(); // get all dashed edges
        int mode = 1;
        graph_manager.displayCombinedGraph(boldEdges, dashedEdges, filename, mode); // display the graph
    };
};
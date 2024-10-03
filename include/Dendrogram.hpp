#pragma once
#include "Tree.hpp"
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <icecream.hpp>
using Vertex = int;
// IDEA:
/*
Start with root nodes with value 0, each of them corrsponds to a vertex

In order to merge, 
1. add an edge root node disjoint from any tree
2. pick one of its end point node, find the path to its root node
3. merge the two path (edge root node is also considered as path)
4. merge the path starting from the other node with the path from last step

Assumption: 1. leaf nodes are all vertices
*/

template<typename T>
class Dendrogram
{
private:
    std::unordered_map<Vertex, std::shared_ptr<Node>> leaf_nodes_map;
    std::unordered_map<size_t, std::shared_ptr<Node>> edge_nodes_map;
public:
    double max_edge_weight = 1e10; // maximum edge weight

    // Construct with vertices
    Dendrogram(const std::vector<Vertex>& vertices_){
        for(const auto& v: vertices_){
            // Create vertex node label (v)
            std::string vertex_label =  "(" + std::to_string(v)+ ")"; 
            leaf_nodes_map[v] = std::make_shared<Node>(T(0.0), vertex_label);
        }
    };

    // Construct with vertices and priscribed vertex values
    Dendrogram(const std::vector<Vertex>& vertices_, const std::vector<T>& vertex_values){
        assert(vertices_.size() == vertex_values.size() && "The number of vertices and vertex values should be the same.");
        for (size_t i = 0; i < vertices_.size(); ++i){
            // Create vertex node label (v)
            std::string vertex_label =  "(" + std::to_string(vertices_[i])+ ")"; 
            leaf_nodes_map[vertices_[i]] = std::make_shared<Node>(vertex_values[i], vertex_label);
        }
    };

    std::shared_ptr<Node> get_vertex_node(Vertex v){return leaf_nodes_map[v];};

    void update_max_edge_weight(T y){
        if (y > max_edge_weight){
            max_edge_weight = y + 10;
        }
    }

    // returns the smallest t ∈ [0, ∞) such that [v] = [w] ∈ π0(G,f)(r), 
    // or ∞ if [v] != [w] ∈ π0(G,f)(r) for all r ∈ [0,∞).
    // i.e. nearest common ancestor (nca)
    T time_of_merge(Vertex v, Vertex w);
    double time_of_merge_double(Vertex v, Vertex w){
        auto lca_node = findLCA(leaf_nodes_map[v], leaf_nodes_map[w]);
        if (lca_node) return lca_node->value;
        return max_edge_weight;
    };
    
    /*
    Let P and Q, respectively, be the paths from v and w to the roots of their
    respective trees. Restructure the tree or trees containing v and w by merging the paths
    P and Q while preserving heap order.
    */
    void merge_at_time(Vertex v, Vertex w, size_t eid, T time); 
};


template<typename T>
T Dendrogram<T>::time_of_merge(Vertex v, Vertex w){
    T f_value;
    return f_value;
}


// Insert an edge node into a path
void insert_edge_node_into_path(std::vector<std::shared_ptr<Node>>& path_v, 
                                const std::shared_ptr<Node>& edge_node) {
    // Find insertion point, e.g., 3-7-15 has possible insertion points 0, 1, 2, 3
    size_t i = 0;
    assert(path_v[0]->value <= edge_node->value && "The vertex value has to be <= edge value, but not now.");
    // Find the appropriate insertion position
    while (i < path_v.size() && path_v[i]->value <= edge_node->value) {
        ++i;
    }

    assert(i > 0 && "The new edge node has to come after vertex node");
    if (i == path_v.size()){
        // New node becomes the new root as its value is higher than all existing nodes
        edge_node->left = path_v.back();
        path_v.back()->parent = edge_node;
        path_v.push_back(edge_node);
    }else{
        // Insert the new node between path[i-1] and path[i]
        edge_node->parent = path_v[i];
        edge_node->left = path_v[i-1];
        if (path_v[i]->left == path_v[i-1]) {
            path_v[i]->left = edge_node;
        } else if(path_v[i]->right == path_v[i-1]) {
            path_v[i]->right = edge_node;
        }else{
            std::ostringstream errorMessage;
            errorMessage << "Runtime error: path_v[i] = " << path_v[i] 
                        << "is not a parent of path_v[i-1] = " << path_v[i-1];
            throw std::runtime_error(errorMessage.str());
        }
        path_v[i-1]->parent = edge_node;
        
        path_v.insert(path_v.begin() + i, edge_node);
    }
}


// Insert an disjoint edge node into a path (assume bottom-to-top order, i.e. ascending)
void insert_edge_node_into_path(std::vector<std::pair<std::shared_ptr<Node>, bool>>& path_v, 
                                const std::shared_ptr<Node>& edge_node) {
    assert(edge_node->left == nullptr);
    assert(edge_node->right == nullptr);
    assert(edge_node->parent.lock() == nullptr);
        
    // Find insertion point, e.g., 3-7-15 has possible insertion points 0, 1, 2, 3
    size_t i = 0;

    // Debug output with if-guard
    if (!(path_v[0].first->value <= edge_node->value)) {
        std::ostringstream oss;
        oss << "Assertion failed: The vertex value has to be <= edge value, but not now.\n"
            << "\tVertex value: " << path_v[0].first->value << "\n"
            << "\tEdge value: " << edge_node->value << "\n";
        std::string errorMessage = oss.str();
        throw std::runtime_error(errorMessage);
    }
    // Find the appropriate insertion position
    while (i < path_v.size() && path_v[i].first->value <= edge_node->value) {
        ++i;
    }

    assert(i > 0 && "The new edge node has to come after vertex node");
    if (i == path_v.size()){
        // New node becomes the new root as its value is higher than all existing nodes
        auto last_node = path_v.back().first;
        edge_node->left = last_node;
        last_node->parent = edge_node;
        path_v.push_back(std::make_pair(edge_node, true));
    }else{
        // Insert the new node between path[i-1] and path[i]
        edge_node->parent = path_v[i].first;
        edge_node->left = path_v[i-1].first; // set as left child 
        if (path_v[i].second) { // its left child is in the path
            path_v[i].first->left = edge_node;
        } else{
            path_v[i].first->right = edge_node;
        }
        path_v[i-1].first->parent = edge_node;
        
        path_v.insert(path_v.begin() + i, std::pair(edge_node, true)); 
    }
}

/*
Path 1 and 2 are stored in an increasing/ascending order (node to root)
Assumption: 1. Path 1 has an empty child 
            2. path 2 
*/             
void merge_paths(std::vector<std::pair<std::shared_ptr<Node>, bool>>& path1, 
                 std::vector<std::pair<std::shared_ptr<Node>, bool>>& path2){
    // reverse them to gaurentee decreasing order 
    std::reverse(path1.begin(), path1.end());
    std::reverse(path2.begin(), path2.end());

    // for (const auto& [node, isLeft] : path) {
    //     std::cout << "Node Value: " << node->value << ", Is Left: " << isLeft << std::endl;
    // }

    // New path vector to hold the merged path
    std::vector<std::pair<std::shared_ptr<Node>, bool>> mergedPath;
    // Merging two paths
    size_t i = 0, j = 0;
    std::shared_ptr<Node> dummy_node = std::make_shared<Node>(0.0, "dummy");
    mergedPath.push_back(std::make_pair(dummy_node, true));
    while (i < path1.size() && j < path2.size()) {
        if (path1[i] == path2[j]){ // same node and child direction 
            mergedPath.push_back(path1[i]);
            i++; j++;
        }else{ 
            // same node different child direction
            if (path1[i].first == path2[j].first){ // choose either one direction
                mergedPath.push_back(std::make_pair(path1[i].first, true));
                path1[i].first->right = nullptr;
                i++; j++;
            } else if (path1[i].first->value >= path2[j].first->value){
                // insert the larger one
                auto& waiting_node = path1[i].first; // node to be inserted
                auto& last_pair = mergedPath.back();
                // if second is true means left child was found in the path, 
                // we need to insert to the left child
                if (last_pair.second){ 
                    last_pair.first->left = waiting_node;
                }else{
                    last_pair.first->right = waiting_node;
                }
                waiting_node->parent = last_pair.first; 
                mergedPath.push_back(path1[i]);
                i++;
            }else{
                // insert the larger one
                auto& waiting_node = path2[j].first; // node to be inserted
                auto& last_pair = mergedPath.back();
                if (last_pair.second){//insert to the left child
                    last_pair.first->left = waiting_node;
                }else{
                    last_pair.first->right = waiting_node;
                }
                waiting_node->parent = last_pair.first;
                mergedPath.push_back(path2[j]);
                j++;
            }
        }
    }
    // std::cout << "Either one of i or j is ended, mergedPath:" << std::endl;
    // for (const auto& [node, isLeft] : mergedPath) {
    //     std::cout << "Node label: " << node->label << ", Is Left: " << isLeft << std::endl;
    // }

    // Append the remaining nodes from path1 or path2
    // there are two key steps: 
    // 1. delete the child from the unfinished path
    // 2. make the parent-child relation between the ending node of merged path and first of unfinished path
    auto& ending_node_pair = mergedPath.back();
    if (i < path1.size()) { 
        // // first delete the child relation in the unfinished path
        // if (i != 0){
        //     if(path1[i-1].second) {path1[i-1].first->left = nullptr;}
        //     else {path1[i-1].first->right = nullptr;}
        // }
        // next connect merged_path[-1] with the first in the unfinished path
        path1[i].first->parent = ending_node_pair.first;
        if (ending_node_pair.first->left){ // left is available
            ending_node_pair.first->right = path1[i].first;
            ending_node_pair.second = false;
        }else if(ending_node_pair.first->right){
            ending_node_pair.first->left = path1[i].first;
            ending_node_pair.second = true;
        }else{
            std::ostringstream errorMessage;
            errorMessage << "The last node of merged path " << *(ending_node_pair.first)
                        << "does not have availability for inserting a new node as its child ";
            throw std::runtime_error(errorMessage.str());
        }
    }
    while (i < path1.size()) mergedPath.push_back(path1[i++]);

    // std::cout << "Finish the rest i, mergedPath:" << std::endl;
    // for (const auto& [node, isLeft] : mergedPath) {
    //     std::cout << "Node label: " << node->label << ", Is Left: " << isLeft << std::endl;
    // }


    if (j < path2.size()) {
        // first delete the child relation in the unfinished path
        // if (j != 0){
        //     if(path2[j-1].second) {path2[j-1].first->left = nullptr;}
        //     else {path2[j-1].first->right = nullptr;}
        // }// TODO: check here to see if it's needed

        // next connect merged_path[-1] with the first in the unfinished path
        path2[j].first->parent = ending_node_pair.first;
        if (ending_node_pair.first->left){ // left is occupied, use the right child 
            ending_node_pair.first->right = path2[j].first;
            ending_node_pair.second = false;
        }else if(ending_node_pair.first->right){
            ending_node_pair.first->left = path2[j].first;
            ending_node_pair.second = true;
        }else{
            // std::ostringstream errorMessage;
            // errorMessage << "The last node of merged path " << *(ending_node_pair.first)
            //             << "does not have availability for inserting a new node as its child ";
            // throw std::runtime_error(errorMessage.str());
            ending_node_pair.first->left = path2[j].first;
            ending_node_pair.second = true;
        }
    }
    while (j < path2.size()) mergedPath.push_back(path2[j++]);
    // std::cout << "Finish the rest j, mergedPath:" << std::endl;
    // for (const auto& [node, isLeft] : mergedPath) {
    //     std::cout << "Node label: " << node->label << ", Is Left: " << isLeft << std::endl;
    // }
    // printBT(mergedPath.front().first);
    // erase the dummy node and reset the parent 
    mergedPath.erase(mergedPath.begin());
    mergedPath.front().first->parent.reset();
    // std::cout << "Final mergedPath:" << std::endl;
    // for (const auto& [node, isLeft] : mergedPath) {
    //     std::cout << "Node label: " << node->label << ", Is Left: " << isLeft << std::endl;
    // }
    // printBT(mergedPath.front().first);
}


template<typename T>
void Dendrogram<T>::merge_at_time(Vertex v, Vertex w, size_t eid, T t){
    // IC(edge_nodes_map);
    // std::cout << "call merge_at_time()" << std::endl;
    // IC(v, w, eid, t);
    // std::cout << "before merge, Tree with v" << std::endl;
    // printBT(getRoot(leaf_nodes_map[v]));
    // std::cout << "before merge, Tree with w" << std::endl;
    // printBT(getRoot(leaf_nodes_map[w]));

    // Create edge node label (v,w)
    std::string edge_label =  "(e_"+ std::to_string(eid) + ")"; 
    // Create an new disjoint edge node
    std::shared_ptr<Node> edge_node = std::make_shared<Node>(t, edge_label);
    edge_nodes_map[eid] = edge_node;

    // Merge the egde node to the path from v to its root
    std::vector<std::pair<std::shared_ptr<Node>, bool>> path_v = getPathToRoot(leaf_nodes_map[v]);
    insert_edge_node_into_path(path_v, edge_node);
    // std::cout << "insert edge node into path of v to its root, Tree with v" << std::endl;
    // printBT(getRoot(leaf_nodes_map[v]));

    // Merge two paths
    std::vector<std::pair<std::shared_ptr<Node>, bool>> path_h = getPathToRoot(edge_node);
    std::vector<std::pair<std::shared_ptr<Node>, bool>> path_w = getPathToRoot(leaf_nodes_map[w]);
    // IC(path_h);
    // IC(path_w);
    merge_paths(path_h, path_w);
    // std::cout << "after merge, Tree is" << std::endl;
    // printBT(getRoot(leaf_nodes_map[v]));
}
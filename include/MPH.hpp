#pragma once
#include "Timer.hpp"
#include <queue>
#include "DynamicTree.hpp"
#include "Graph.hpp"
#include "Poset.hpp"
#include "Dendrogram.hpp"
#include <set>
#include <icecream.hpp>
#include "Grade.hpp"
#include "GGraph.hpp"

/*
    Algorithm 1: Collapse local collapsible edges (inplace)
    Assumption: g1 and g2 are the same when passed. We will only modify g2. 
*/
template<typename FT>
void local_collapse_edges_inplace(Graph<FT>& g) {
    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::unordered_map<int, bool> visited;
    for (auto v: g.get_vertices() ) {
        vertex_dict[v] = v;
        visited[v] = false;
    }

    // type of (vertex, edge, vertex)
    using VEV = std::tuple<int, Edge, int>; 
    // Run depth-first search from v on local edges 
    for (auto v: g.get_vertices() ) {
        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, Edge::NULL_EDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            int v_; Edge e; int u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_);
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (!visited[u]) {
                visited[u] = true;
                if (e != Edge::NULL_EDGE){
                    vertex_dict[u] = v;
                    g.remove_edge(e);
                }
                // local collapsible
                for (const auto& eid_local: g.get_adj(u)){
                    e = g.get_edge(eid_local);
                    Vertex x = (e[0] == u) ? e[1]: e[0];
                    
                    FT fx, fu, fe;
                    fx = g.get_vertex_value(x);
                    fu = g.get_vertex_value(u);
                    fe = g.get_edge_value(eid_local);
                    if (fe == fx && fx == fu){
                        stack.push(VEV(v, g.get_edge(eid_local), x));
                    }
                }
            }
        }
    }

    // update vertices and edges
    g.update_graph(vertex_dict);
}



/*
    Algorithm 2: Collapse to vertex-minimal graph (inplace)
*/
template<typename FT>
void collapse_to_vertex_minimal_inplace(Graph<FT>& g) {
    // local collapse edges
    local_collapse_edges_inplace(g);

    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::set<int> visited;
    for (auto v: g.get_vertices() ) {
        vertex_dict[v] = v;
    }

    // Define (vertex, edge, vertex) type 
    using VEV = std::tuple<int, Edge, int>;
    // Run depth-first search from minimal vertices
    for (auto v: g.get_vertices() ) {
        FT fv = g.get_vertex_value(v);
        bool check_minimal = true;
        for (const auto& eid : g.get_adj(v)) {
            Edge e = g.get_edge(eid);
            Vertex u = (e[0] == v) ? e[1]: e[0];
            FT fe = g.get_edge_value(eid);
            FT fu = g.get_vertex_value(u);
            if (fe == fv && fe > fu){ // v is not minimal
                check_minimal = false;
                break;
            }
        }
        if(!check_minimal){continue;} // if v is not minimal, skip to the next vertex

        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, Edge::NULL_EDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            int v_; Edge e; int u;
            std::tie(v_, e, u) = stack.top();
            if (v != v_) {
                std::stringstream ss;
                ss << "Runtime error: Expected vertex " << v << " but got " << v_;
                throw std::runtime_error(ss.str());
            }
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (visited.find(u) == visited.end()) {
                visited.insert(u);
                if (e != Edge::NULL_EDGE){
                    vertex_dict[u] = v;
                    g.remove_edge(e);
                }

                // local collapsible
                for (const auto& eid_local: g.get_adj(u)){
                    e = g.get_edge(eid_local);
                    Vertex x = (e[0] == u) ? e[1]: e[0];
                    FT fx, fu, fe;
                    fx = g.get_vertex_value(x);
                    fu = g.get_vertex_value(u);
                    fe = g.get_edge_value(eid_local);
                    if (fe == fx && fx > fu){
                        stack.push(VEV(v, g.get_edge(eid_local), x));
                    }
                }
            }
        }
    }

    // update vertices 
    g.update_graph(vertex_dict);    
}


/*
    Algorithm 1: Collapse local collapsible edges
    Assumption: g1 and g2 are the same when passed. We will only modify g2. 
*/
template<typename FT>
void localCollapse(const Graph<FT>& g1, Graph<FT>& g2) {
    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::unordered_map<int, bool> visited;
    for (auto v: g1.get_vertices() ) {
        vertex_dict[v] = v;
        visited[v] = false;
    }

    // type of (vertex, edge, vertex)
    using VEV = std::tuple<int, Edge, int>; 
    std::cout << "Running DFS in localCollapse..." << std::endl;
    // Run depth-first search from v on local edges 
    for (auto v: g1.get_vertices() ) {
        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, Edge::NULL_EDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            int v_; Edge e; int u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_);
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (!visited[u]) {
                // std::cout << "Visit " << u << std::endl;
                visited[u] = true;
                if (e != Edge::NULL_EDGE){
                    vertex_dict[u] = v;
                    g2.remove_edge(e);
                    // std::cout << "Remove "<< e << std::endl;
                }
                // local collapsible
                for (const auto& eid_local: g1.get_adj(u)){
                    e = g1.get_edge(eid_local);
                    Vertex x = (e[0] == u) ? e[1]: e[0];
                    
                    FT fx, fu, fe;
                    fx = g1.get_vertex_value(x);
                    fu = g1.get_vertex_value(u);
                    fe = g1.get_edge_value(eid_local);
                    if (fe == fx && fx == fu){
                        stack.push(VEV(v, g1.get_edge(eid_local), x));
                    }
                }
            }
        }
    }
    std::cout << "Done running DFS in localCollapse" << std::endl;

    // update vertices and edges
    std::cout << "Updating graph..." << std::endl;
    g2.update_graph(vertex_dict);
    std::cout << "Done updating graph" << std::endl;
}


/*
    Algorithm 2: Collapse to vertex-minimal graph
*/
template<typename FT>
Graph<FT> collapse_to_vertex_minimal(const Graph<FT>& g) {
    std::cout << "Local Collapse..." << std::endl;
    std::cout << "Copying graph..." << std::endl;
    Graph<R2> g1 = Graph<R2>(g);
    std::cout << "Done copying graph" << std::endl;
    localCollapse(g, g1);
    std::cout << "Done local collapse" << std::endl;
    std::cout << "Copying graph..." << std::endl;
    Graph<R2> g2 = Graph<R2>(g1); 
    std::cout << "Done copying graph" << std::endl;


    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::set<int> visited;
    for (auto v: g1.get_vertices() ) {
        vertex_dict[v] = v;
    }

    std::cout << "Running DFS..." << std::endl;
    // Define (vertex, edge, vertex) type 
    using VEV = std::tuple<int, Edge, int>;
    // Run depth-first search from minimal vertices
    for (auto v: g1.get_vertices() ) {
        FT fv = g1.get_vertex_value(v);
        bool check_minimal = true;
        for (const auto& eid : g1.get_adj(v)) {
            Edge e = g1.get_edge(eid);
            int u = (e[0] == v) ? e[1]: e[0];
            FT fe = g1.get_edge_value(eid);
            FT fu = g1.get_vertex_value(u);
            if (fe == fv && fe > fu){ // v is not minimal
                check_minimal = false;
                break;
            }
        }
        if(!check_minimal){continue;}

        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, Edge::NULL_EDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            int v_; Edge e; int u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_);
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (visited.find(u) == visited.end()) {
                visited.insert(u);
                if (e != Edge::NULL_EDGE){
                    vertex_dict[u] = v;
                    g2.remove_edge(e);
                }

                // local collapsible
                for (const auto& eid_local: g1.get_adj(u)){
                    e = g1.get_edge(eid_local);
                    int x = (e[0] == u) ? e[1]: e[0];
                    FT fx, fu, fe;
                    fx = g1.get_vertex_value(x);
                    fu = g1.get_vertex_value(u);
                    fe = g1.get_edge_value(eid_local);
                    if (fe == fx && fx > fu){
                        stack.push(VEV(v, g1.get_edge(eid_local), x));
                    }
                }
            }
        }
    }
    std::cout << "Done running DFS" << std::endl;

    std::cout << "Updating graph..." << std::endl;
    // update vertices 
    g2.update_graph(vertex_dict);
    std::cout << "Done updating graph" << std::endl;
    return g2;
}

template<typename FT>
std::set<FT> collect_grade_pts_from_graph(const Graph<FT> g){
    std::set<FT> grade_pts;
    std::unordered_map<EdgeId, FT> g_edges_values = g.get_edges_values();
    std::unordered_map<Vertex, FT> g_vert_values = g.get_vert_values();
    size_t n = g_edges_values.size() + g_vert_values.size();
    
    grade_pts.resize(n);
    // loop over vertex values
    grade_pts.emplace_back();
    // loop over edge values


    return grade_pts;
}

// Algorithm 4: Betti tables and minimal presentation of R2-filtered graph
// Return: 4 vectors as Betti tables, 1 sparse Matrix as presentation
template<typename FT>
std::tuple<
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<std::tuple<size_t, size_t, int>>
> compute_MPH0_Dengrogram(const Graph<FT>& g) {
    std::vector<FT> betti_0, betti_1, betti_2, betti_0_1;
    
    Graph<FT> g1 = collapse_to_vertex_minimal(g);
    
    // initialize Dendrogram with zero vertices values 
    // Dendrogram<typename FT::CoordinateTP> D(g1.get_vertices());

    std::vector<std::tuple<size_t, size_t, int>> M;
    std::unordered_map<Vertex, size_t> row_idx;

    // get f of graph  
    std::unordered_map<EdgeId, FT> g1_edges_values = g1.get_edges_values();
    std::unordered_map<Vertex, FT> g1_vert_values = g1.get_vert_values();

    std::vector<typename FT::CoordinateTP> y_values_vector;
    y_values_vector.reserve(g1.get_vertices().size());
    for (const auto& v_values: g1.get_vert_values_vector()){
        y_values_vector.emplace_back(v_values.getY());
    }
    Dendrogram<typename FT::CoordinateTP> D(g1.get_vertices(), y_values_vector);

    // Construct the dictionary mapping R2 to a tuple of vectors(vertices and edges' ids)
    std::unordered_map<FT, std::tuple<std::vector<Vertex>, std::vector<EdgeId>>, FTHash<FT>> FT_2_vertex_edges_id;
    // Define a set of R2 objects using the custom comparator for lexicographical ordering
    std::set<R2, LexicographicalCompareR2> gd_points;

    for (const auto& pair : g1_vert_values) {
        Vertex v = pair.first;
        const FT& fv = pair.second;
        // push it to the first element of the tuple
        std::get<0>(FT_2_vertex_edges_id[fv]).push_back(v);
        gd_points.emplace(fv);
    }

    for (const auto& pair : g1_edges_values) {
        EdgeId eid = pair.first;
        const FT& fe = pair.second;
        // push it to the first element of the tuple
        std::get<1>(FT_2_vertex_edges_id[fe]).push_back(eid);
        gd_points.emplace(fe);
    }


    for (const auto& gd_point : gd_points) {
        // All vertices belong to the projective cover
        std::vector<Vertex> verts_gd = std::get<0>(FT_2_vertex_edges_id[gd_point]);
        if (verts_gd.size()!= 0){
            for (const auto& v: verts_gd){
                betti_0.emplace_back(gd_point);
                // row_idx[v] ← |β0|
                row_idx[v] = betti_0.size();
            }
        }
        // Check edges
        auto y = gd_point.getY();
        auto x = gd_point.getX();
        D.update_max_edge_weight(y);
        std::vector<EdgeId> edges_ids_gd = std::get<1>(FT_2_vertex_edges_id[gd_point]);
        for (const auto& eid: edges_ids_gd){
            Vertex e_0, e_1;           
            auto e = g1.get_edge(eid);
            e_0 = e[0]; e_1 = e[1];
            auto s = D.time_of_merge_double(e_0, e_1); // y-coordinate
            if (e_0 == e_1){
                betti_0_1.emplace_back(x,y);
                continue; // self loop only affects betti_0_1
            }
            else{
                D.merge_at_time(e_0, e_1, eid, y);
            }

            if (s <= y){
                betti_0_1.emplace_back(gd_point); // The edge is deletable, so it only affects H1
            }else{ // Edge is not deletable, so belongs to relations in resolution
                betti_1.emplace_back(gd_point);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                if (s < D.max_edge_weight){ // The edge is cycle-creating
                    betti_2.emplace_back(x,s);
                    betti_0_1.emplace_back(x,s);
                }
            }
        }
    }
    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M); 
}



// Algorithm 4: Betti tables and minimal presentation of R2-filtered graph
// Return: 4 vectors as Betti tables, 1 sparse Matrix as presentation
template<typename FT>
std::tuple<
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<std::tuple<size_t, size_t, int>>
> compute_MPH0_DTree_Inplace(Graph<FT>& g, bool visual_DT = false) {
    std::vector<FT> betti_0, betti_1, betti_2, betti_0_1;
    std::vector<std::tuple<size_t, size_t, int>> M;
    // collapse to vertex minimal
    collapse_to_vertex_minimal_inplace(g);

    // build the dynamic tree
    DynamicTree<typename FT::CoordinateTP> DT(g.get_vertices(), visual_DT);

    std::unordered_map<Vertex, size_t> row_idx; // row index for M matrix 
    
    // get f of graph  
    const auto& g_edges_values = g.get_edges_values();
    const auto& g_vert_values = g.get_vert_values();

    // Construct the dictionary mapping R2 to a tuple of vectors(vertices and edges' ids)
    std::unordered_map<FT, std::tuple<std::vector<Vertex>, std::vector<EdgeId>>, FTHash<FT>> FT_2_vertex_edges_id;
    // Define a set of R2 objects using the custom comparator for lexicographical ordering
    std::set<R2, LexicographicalCompareR2> gd_points;
    std::set<R2, LexicographicalCompareR2> gd_points_shifted;

    // get grid points of all vertices and edges
    // and find the minimum of x and y coordinates
    R2::CoordinateTP min_x = R2::CoordinateMax;
    R2::CoordinateTP min_y = R2::CoordinateMax;
    for (const auto& pair : g_vert_values) {
        Vertex v = pair.first;
        const FT& fv = pair.second;
        // push it to the first element of the tuple
        std::get<0>(FT_2_vertex_edges_id[fv]).push_back(v);
        gd_points.emplace(fv);
        min_x = std::min(min_x, fv.getX());
        min_y = std::min(min_y, fv.getY());
    }
    for (const auto& pair : g_edges_values) {
        EdgeId eid = pair.first;
        const FT& fe = pair.second;
        // push it to the first element of the tuple
        std::get<1>(FT_2_vertex_edges_id[fe]).push_back(eid);
        gd_points.emplace(fe);
        min_x = std::min(min_x, fe.getX());
        min_y = std::min(min_y, fe.getY());
    }
    R2 min_point(min_x, min_y);

    // shift/subtract the minimum from x y coordinates such that grid points are non-negative
    for (auto& gd_point : gd_points) {
        gd_points_shifted.emplace(gd_point - min_point);
    }

    // loop over all shifted grid points
    for (const auto& gd_point_shifted : gd_points_shifted) {
        R2 gd_point = gd_point_shifted + min_point;
        // All vertices belong to the projective cover
        std::vector<Vertex> verts_gd = std::get<0>(FT_2_vertex_edges_id[gd_point]);
        if (verts_gd.size()!= 0){
            for (const auto& v: verts_gd){
                betti_0.emplace_back(gd_point);
                // row_idx[v] ← |β0|
                row_idx[v] = betti_0.size();
            }
        }

        // Check edges
        auto y_shifted = gd_point_shifted.getY();
        auto x_shifted = gd_point_shifted.getX();
        auto x = gd_point.getX();
        auto y = gd_point.getY();
        DT.update_max_edge_weight(y_shifted);
        std::vector<EdgeId> edges_ids_gd = std::get<1>(FT_2_vertex_edges_id[gd_point]);
        for (const auto& eid: edges_ids_gd){
            Vertex e_0, e_1;
            auto e = g.get_edge(eid);
            e_0 = e[0]; e_1 = e[1];
            auto s_shifted = DT.time_of_merge_double(e_0, e_1); // y-coordinate
            if (e_0 == e_1){
                betti_0_1.emplace_back(x, y);
                continue; // self loop only affects betti_0_1
            }
            else{
                DT.merge_at_time(e_0, e_1, y_shifted);
            }
            
            if (s_shifted <= y_shifted){
                betti_0_1.emplace_back(gd_point); // The edge is deletable, so it only affects H1
            }else{ // Edge is not deletable, so belongs to relations in resolution
                betti_1.emplace_back(gd_point);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                // if (s < FT::CoordinateMax){ // The edge is cycle-creating
                if (s_shifted < DT.max_edge_weight){ // The edge is cycle-creating
                    betti_2.emplace_back(x, s_shifted + min_y);
                    betti_0_1.emplace_back(x, s_shifted + min_y);
                }
            }        

        }// End loop for each edge at gd_point 
    } // End loop for all grid points
    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M); 
}

// Algorithm 4: Betti tables and minimal presentation of R2-filtered graph
// Return: 4 vectors as Betti tables, 1 sparse Matrix as presentation
template<typename FT>
std::tuple<
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<std::tuple<size_t, size_t, int>>
> compute_MPH0_DTree(const Graph<FT>& g, bool visual_DT = false) {
    std::vector<FT> betti_0, betti_1, betti_2, betti_0_1;
    std::vector<std::tuple<size_t, size_t, int>> M;

    // collapse to vertex minimal
    Graph<FT> g1 = collapse_to_vertex_minimal(g);

    // build the dynamic tree
    DynamicTree<typename FT::CoordinateTP> DT(g1.get_vertices(), visual_DT);
    

    std::unordered_map<Vertex, size_t> row_idx; // row index for M matrix 

    // get f of graph  
    const auto& g1_edges_values = g1.get_edges_values();
    const auto& g1_vert_values = g1.get_vert_values();

    // Construct the dictionary mapping R2 to a tuple of vectors(vertices and edges' ids)
    std::unordered_map<FT, std::tuple<std::vector<Vertex>, std::vector<EdgeId>>, FTHash<FT>> FT_2_vertex_edges_id;
    // Define a set of R2 objects using the custom comparator for lexicographical ordering
    std::set<R2, LexicographicalCompareR2> gd_points;
    std::set<R2, LexicographicalCompareR2> gd_points_shifted;

    // get grid points of all vertices and edges
    // and find the minimum of x and y coordinates
    R2::CoordinateTP min_x = R2::CoordinateMax;
    R2::CoordinateTP min_y = R2::CoordinateMax;
    for (const auto& pair : g1_vert_values) {
        Vertex v = pair.first;
        const FT& fv = pair.second;
        // push it to the first element of the tuple
        std::get<0>(FT_2_vertex_edges_id[fv]).push_back(v);
        gd_points.emplace(fv);
        min_x = std::min(min_x, fv.getX());
        min_y = std::min(min_y, fv.getY());
    }
    for (const auto& pair : g1_edges_values) {
        EdgeId eid = pair.first;
        const FT& fe = pair.second;
        // push it to the first element of the tuple
        std::get<1>(FT_2_vertex_edges_id[fe]).push_back(eid);
        gd_points.emplace(fe);
        min_x = std::min(min_x, fe.getX());
        min_y = std::min(min_y, fe.getY());
    }
    R2 min_point(min_x, min_y);
    
    // shift/subtract the minimum from x y coordinates such that grid points are non-negative
    for (auto& gd_point : gd_points) {
        gd_points_shifted.emplace(gd_point - min_point);
    }

    // loop over all shifted grid points
    for (const auto& gd_point_shifted : gd_points_shifted) {
        R2 gd_point = gd_point_shifted + min_point;
        // All vertices belong to the projective cover
        std::vector<Vertex> verts_gd = std::get<0>(FT_2_vertex_edges_id[gd_point]);
        if (verts_gd.size()!= 0){
            for (const auto& v: verts_gd){
                betti_0.emplace_back(gd_point);
                // row_idx[v] ← |β0|
                row_idx[v] = betti_0.size();
            }
        }

        // Check edges
        auto y_shifted = gd_point_shifted.getY();
        auto x_shifted = gd_point_shifted.getX();
        auto x = gd_point.getX();
        auto y = gd_point.getY();
        DT.update_max_edge_weight(y_shifted);
        std::vector<EdgeId> edges_ids_gd = std::get<1>(FT_2_vertex_edges_id[gd_point]);
        for (const auto& eid: edges_ids_gd){
            Vertex e_0, e_1;
            auto e = g1.get_edge(eid);
            e_0 = e[0]; e_1 = e[1];
            auto s_shifted = DT.time_of_merge_double(e_0, e_1); // y-coordinate
            if (e_0 == e_1){
                betti_0_1.emplace_back(x, y);
                continue; // self loop only affects betti_0_1
            }
            else{
                DT.merge_at_time(e_0, e_1, y_shifted);
            }
            
            if (s_shifted <= y_shifted){
                betti_0_1.emplace_back(gd_point); // The edge is deletable, so it only affects H1
            }else{ // Edge is not deletable, so belongs to relations in resolution
                betti_1.emplace_back(gd_point);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                // if (s < FT::CoordinateMax){ // The edge is cycle-creating
                if (s_shifted < DT.max_edge_weight){ // The edge is cycle-creating
                    betti_2.emplace_back(x, s_shifted + min_y);
                    betti_0_1.emplace_back(x, s_shifted + min_y);
                }
            }        

        }// End loop for each edge at gd_point 
    } // End loop for all grid points
    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M); 
}




// Algorithm 4: Betti tables and minimal presentation of R2-filtered graph
// Return: 4 vectors as Betti tables, 1 sparse Matrix as presentation
template<typename FT>
std::tuple<
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<FT>,
    std::vector<std::tuple<size_t, size_t, int>>,
    std::unordered_map<std::string, std::vector<std::tuple<EdgeId, EdgeId>>>
> compute_MPH0_DTree_debug(const Graph<FT>& g, bool visual_DT = false) {
    std::vector<FT> betti_0, betti_1, betti_2, betti_0_1;
    // Egde matchings for debug
    std::unordered_map<std::string, std::vector<std::tuple<EdgeId, EdgeId>>> edge_matchings;
    // Initialize with 2 empty vectors
    edge_matchings["b_1"] = std::vector<std::tuple<EdgeId, EdgeId>>{};
    edge_matchings["b_2"] = std::vector<std::tuple<EdgeId, EdgeId>>{};
    edge_matchings["b_0_1"] = std::vector<std::tuple<EdgeId, EdgeId>>{};

    Graph<FT> g1 = collapse_to_vertex_minimal(g);

    // typename FT::CoordinateTP;
    DynamicTree<typename FT::CoordinateTP> DT(g1.get_vertices(), visual_DT);

    std::vector<std::tuple<size_t, size_t, int>> M;
    std::unordered_map<Vertex, size_t> row_idx;

    // get f of graph  
    const auto& g1_edges_values = g1.get_edges_values();
    const auto& g1_vert_values = g1.get_vert_values();

    // Construct the dictionary mapping R2 to a tuple of vectors(vertices and edges' ids)
    std::unordered_map<FT, std::tuple<std::vector<Vertex>, std::vector<EdgeId>>, FTHash<FT>> FT_2_vertex_edges_id;
    // Define a set of R2 objects using the custom comparator for lexicographical ordering
    std::set<R2, LexicographicalCompareR2> gd_points;

    for (const auto& pair : g1_vert_values) {
        Vertex v = pair.first;
        const FT& fv = pair.second;
        // push it to the first element of the tuple
        std::get<0>(FT_2_vertex_edges_id[fv]).push_back(v);
        gd_points.emplace(fv);
    }

    for (const auto& pair : g1_edges_values) {
        EdgeId eid = pair.first;
        const FT& fe = pair.second;
        // push it to the first element of the tuple
        std::get<1>(FT_2_vertex_edges_id[fe]).push_back(eid);
        gd_points.emplace(fe);
    }

    for (const auto& gd_point : gd_points) {
        // All vertices belong to the projective cover
        std::vector<Vertex> verts_gd = std::get<0>(FT_2_vertex_edges_id[gd_point]);
        if (verts_gd.size()!= 0){
            for (const auto& v: verts_gd){
                betti_0.emplace_back(gd_point);
                // row_idx[v] ← |β0|
                row_idx[v] = betti_0.size();
            }
        }

        // Check edges
        std::vector<EdgeId> edges_ids_gd = std::get<1>(FT_2_vertex_edges_id[gd_point]);
        for (const auto& eid: edges_ids_gd){
            Vertex e_0, e_1;
            auto e = g1.get_edge(eid);
            e_0 = e[0]; e_1 = e[1];
            // TODO: return the edge ID of the merge 
            auto [s, merge_eid] = DT.time_of_merge_double_debug(e_0, e_1); // y-coordinate 
            auto y = gd_point.getY();
            auto x = gd_point.getX();
            if (e_0 == e_1){
                betti_0_1.emplace_back(x,y);
                edge_matchings["b_0_1"].emplace_back(eid, eid);
                continue; // self loop only affects betti_0_1
            }
            else{
                DT.merge_at_time_debug(e_0, e_1, y, eid);
            }
            
            if (s <= y){
                betti_0_1.emplace_back(gd_point); // The edge is deletable, so it only affects H1
                edge_matchings["b_0_1"].emplace_back(eid, eid);
            }else{ // Edge is not deletable, so belongs to relations in resolution
                betti_1.emplace_back(gd_point);
                edge_matchings["b_1"].emplace_back(eid, eid);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                // if (s < FT::CoordinateMax){ // The edge is cycle-creating
                if (s < DT.max_edge_weight){ // The edge is cycle-creating
                    betti_2.emplace_back(x,s);
                    betti_0_1.emplace_back(x,s);
                    // TODO: change the second e_id to s 
                    edge_matchings["b_2"].emplace_back(eid, merge_eid); 
                    edge_matchings["b_0_1"].emplace_back(eid, merge_eid);
                }
            }        

        }// End loop for each edge at gd_point 
    } // End loop for all grid points
    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M, edge_matchings); 
}


/*
    Algorithm 1: Collapse local collapsible edges inplace
*/
void local_collapse_edges_Grade_Version(GGraph& g) {
    // Initialize dictionary φ with identity map
    std::vector<size_t> vertex_dict; vertex_dict.resize(g.get_nvertices());
    std::iota(vertex_dict.begin(), vertex_dict.end(), 0); // fill with 0, 1, 2, ...

    // Initialize empty set visited
    std::vector<bool> visited(g.get_nvertices(), false);

    // type of (vertex, edge, vertex)
    using VEV = std::tuple<GVertex, GEdge, GVertex>; 
    // Run depth-first search from v on local edges 
    for (size_t v_idx = 0; v_idx < g.get_nvertices(); ++v_idx){
        GVertex v = g.get_gvertex(v_idx);
        if (v.get_id() == -1) {visited[v_idx] = true; continue;} // if the vertex is removed, skip it

        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, GEdge::NULL_GEDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            GVertex v_; GEdge e; GVertex u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_);
            stack.pop();

            // If the vertex has not been visited and not removed, 
            // mark it as visited and process it
            if (!visited[u.get_id()] && u.get_id() != -1) {
                visited[u.get_id()] = true;
                if (e != GEdge::NULL_GEDGE){
                    vertex_dict[u.get_id()] = v.get_id();
                    // remove the edge
                    g.remove_edge(e.get_id());
                }
                // local collapsible
                for (const auto& eid_local: g.get_adj(u.get_id())){
                    e = g.get_gedge(eid_local);
                    // get the other vertex of the edge
                    GVertex x = (e.get_v0() == u.get_id()) ? g.get_gvertex(e.get_v1()): g.get_gvertex(e.get_v0());
                    
                    // Get the grade of the vertex and edge
                    GradePoint fx = x.get_grade();
                    GradePoint fu = u.get_grade();
                    GradePoint fe = e.get_grade();

                    // Collapsible when fx == fe == fu
                    if (fe == fx && fx == fu){
                        stack.push(VEV(v, g.get_gedge(eid_local), x));
                    }
                }
            }
        }
    }

    // update vertices and edges
    g.update_graph(vertex_dict);
    
}


/*
    Algorithm 2: Collapse to vertex-minimal graph 
*/
void collapse_to_vertex_minimal_Grade_Version(GGraph& g) {
    // local collapse edges
    local_collapse_edges_Grade_Version(g);

    // Initialize dictionary φ with identity map
    std::vector<size_t> vertex_dict; vertex_dict.resize(g.get_nvertices());
    std::iota(vertex_dict.begin(), vertex_dict.end(), 0); // fill with 0, 1, 2, ...

    // Initialize empty set visited
    std::vector<bool> visited(g.get_nvertices(), false);

    // Define (vertex, edge, vertex) type 
    using VEV = std::tuple<GVertex, GEdge, GVertex>;

    // Run depth-first search from minimal vertices
    for (size_t v_idx = 0; v_idx < g.get_nvertices(); ++v_idx){
        GVertex v = g.get_gvertex(v_idx);
        // TODO: check if we can use visited[v_idx] to skip?????
        if (v.get_id() == -1 || visited[v_idx]) {visited[v_idx] = true; continue;} // if the vertex is removed, skip it
        GradePoint fv = v.get_grade();

        // Check if v is minimal
        bool check_minimal = true;
        for (const auto& eid : g.get_adj(v_idx)) {
            GEdge e = g.get_gedge(eid);
            GVertex u = (e.get_v0() == v_idx) ? g.get_gvertex(e.get_v1()): g.get_gvertex(e.get_v0());
            GradePoint fe = e.get_grade();
            GradePoint fu = u.get_grade();
            if (fe == fv && fe > fu){ // v is not minimal
                check_minimal = false;
                break;
            }
        }
        if(!check_minimal){continue;} // if v is not minimal, skip to the next vertex

        std::stack<VEV> stack;
        // Push the starting VEV onto the stack
        VEV a(v, GEdge::NULL_GEDGE, v);
        stack.push(a);

        while (!stack.empty()) {
            GVertex v_; GEdge e; GVertex u;
            std::tie(v_, e, u) = stack.top();
            assert(v == v_ && "v != v_");
            stack.pop();

            // If the vertex has not been visited, mark it as visited and process it
            if (!visited[u.get_id()] && u.get_id() != -1) {
                visited[u.get_id()] = true;
                if (e != GEdge::NULL_GEDGE){
                    vertex_dict[u.get_id()] = v.get_id();
                    g.remove_edge(e.get_id());
                }

                // local collapsible
                for (const auto& eid_local: g.get_adj(u.get_id())){
                    e = g.get_gedge(eid_local);
                    GVertex x = (e.get_v0() == u.get_id()) ? g.get_gvertex(e.get_v1()): g.get_gvertex(e.get_v0());
                    // Get the grade of the vertex and edge
                    GradePoint fx = x.get_grade();
                    GradePoint fu = u.get_grade();
                    GradePoint fe = e.get_grade();
                    if (fe == fx && fx > fu){
                        stack.push(VEV(v, g.get_gedge(eid_local), x));
                    }
                }
            }
        }
    }

    // update vertices and edges
    g.update_graph(vertex_dict);
}




// Algorithm 4: Betti tables and minimal presentation of R2-filtered graph
// Return: 4 Betti tables, 1 sparse Matrix as presentation
std::tuple<
    std::vector<std::pair<int, int>>, 
    std::vector<std::pair<int, int>>, 
    std::vector<std::pair<int, int>>, 
    std::vector<std::pair<int, int>>, 
    std::vector<std::tuple<size_t, size_t, int>>
> compute_MPH0_DTree_Grade_Version( 
        GGraph& g,
        bool visual_DT = false) 
{

    // Initialize betti_0, betti_1, betti_2, betti_0_1
    std::vector<std::pair<int, int>> betti_0;
    std::vector<std::pair<int, int>> betti_1;
    std::vector<std::pair<int, int>> betti_2;
    std::vector<std::pair<int, int>> betti_0_1;

    // Sparse Matrix for presentation and its row index
    std::vector<std::tuple<size_t, size_t, int>> M;
    std::unordered_map<VertexId, size_t> row_idx; 

#if MPH0_TIMERS
mph0::collapse_timer.start();
#endif
    // Collapse to vertex minimal
    collapse_to_vertex_minimal_Grade_Version(g);
#if MPH0_TIMERS
mph0::collapse_timer.stop();
#endif

    // dict mapping a GradePoint to (vertices, edges)
    using VEsTuple = std::pair<std::vector<VertexId>, std::vector<EdgeId>>;
    std::unordered_map<GradePoint, VEsTuple, FTHash<GradePoint>> grade_point_2_vertex_edges_id;
    
    // // set of all grade points for iteration
    // std::set<GradePoint, LexicographicalOrderGradePoint> grades_collection;
    std::vector<GradePoint> grades_collection;

    // active vertices labels 
    std::vector<VertexId> active_vertices_ids;

#if MPH0_TIMERS
mph0::create_grades_timer.start();
#endif
    // Loop over all vertices and edges to find acitve vertices and edges
    for (const auto& gv: g.get_gvertices()){
        if (gv.get_id() == -1) {continue;}
        active_vertices_ids.push_back(gv.get_id());
        grade_point_2_vertex_edges_id[gv.get_grade()].first.push_back(gv.get_id());
        grades_collection.push_back(gv.get_grade());
    }
    for (const auto& ge: g.get_gedges()){
        if (ge.get_id() == -1) {continue;}
        grade_point_2_vertex_edges_id[ge.get_grade()].second.push_back(ge.get_id());
        grades_collection.push_back(ge.get_grade());
    }
    std::sort(grades_collection.begin(), grades_collection.end(), LexicographicalOrderGradePoint());
    // remove duplicates
    grades_collection.erase(std::unique(grades_collection.begin(), grades_collection.end()), grades_collection.end());
#if MPH0_TIMERS
mph0::create_grades_timer.stop();
#endif

#if MPH0_TIMERS
mph0::grades_iteration_timer.start();
#endif

    // Build Dynamic Tree
    DynamicTree<int, VertexId, std::hash<VertexId>> DT(active_vertices_ids, visual_DT);

    // loop over all GradePoint in lexicographical ordering
    for (const auto& gd_point : grades_collection) {
        // get the grade point and its x,y ranks
        int x = gd_point.get_x();
        int y = gd_point.get_y();

        // get vertices belong to the grade point
        std::vector<VertexId> verts_gd = grade_point_2_vertex_edges_id[gd_point].first;
        for (const auto& v_id: verts_gd){
            betti_0.emplace_back(x, y);
            // row_idx[v] ← |β0|
            row_idx[v_id] = betti_0.size();
        }

        // Check edges
        DT.update_max_edge_weight(y);
        std::vector<EdgeId> edges_ids_gd = grade_point_2_vertex_edges_id[gd_point].second;
        for (const EdgeId& eid: edges_ids_gd){
            VertexId e_0 = g.get_edge(eid).get_v0(); 
            VertexId e_1 = g.get_edge(eid).get_v1();
            // check if the edge is a self-loop
            if (e_0 == e_1){
                betti_0_1.emplace_back(x, y);
                continue; // self loop only affects betti_0_1
            }
            auto s = DT.time_of_merge_double(e_0, e_1); // y-coordinate
            if (s <= y){
                betti_0_1.emplace_back(x, y); // The edge is deletable, so it only affects H1
            }else{ // Edge is not deletable, so belongs to relations in resolution
                DT.merge_at_time(e_0, e_1, y);
                betti_1.emplace_back(x, y);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                // if (s < FT::CoordinateMax){ // The edge is cycle-creating
                if (s < DT.max_edge_weight){ // The edge is cycle-creating
                    betti_2.emplace_back(x, s);
                    betti_0_1.emplace_back(x, s);
                }
            }        

        }// End loop for each edge at gd_point 
    } // End loop for all grid points
#if MPH0_TIMERS
mph0::grades_iteration_timer.stop();
#endif

    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M); 
}


// Algorithm 4: Betti tables and minimal presentation of R2-filtered graph
// Return: 4 Betti tables, 1 sparse Matrix as presentation
std::tuple<
    std::vector<std::pair<int, int>>, 
    std::vector<std::pair<int, int>>, 
    std::vector<std::pair<int, int>>, 
    std::vector<std::pair<int, int>>, 
    std::vector<std::tuple<size_t, size_t, int>>
> compute_MPH0_TopTree(GGraph& g)
{

    // Initialize betti_0, betti_1, betti_2, betti_0_1
    std::vector<std::pair<int, int>> betti_0;
    std::vector<std::pair<int, int>> betti_1;
    std::vector<std::pair<int, int>> betti_2;
    std::vector<std::pair<int, int>> betti_0_1;

    // Sparse Matrix for presentation and its row index
    std::vector<std::tuple<size_t, size_t, int>> M;
    std::unordered_map<VertexId, size_t> row_idx; 

#if MPH0_TIMERS
mph0::collapse_timer.start();
#endif
    // Collapse to vertex minimal
    collapse_to_vertex_minimal_Grade_Version(g);
#if MPH0_TIMERS
mph0::collapse_timer.stop();
#endif

    // dict mapping a GradePoint to (vertices, edges)
    using VEsTuple = std::pair<std::vector<VertexId>, std::vector<EdgeId>>;
    std::unordered_map<GradePoint, VEsTuple, FTHash<GradePoint>> grade_point_2_vertex_edges_id;
    
    // // set of all grade points for iteration
    // std::set<GradePoint, LexicographicalOrderGradePoint> grades_collection;
    std::vector<GradePoint> grades_collection;

    // active vertices labels 
    std::vector<VertexId> active_vertices_ids;

#if MPH0_TIMERS
mph0::create_grades_timer.start();
#endif
    // Loop over all vertices and edges to find acitve vertices and edges
    for (const auto& gv: g.get_gvertices()){
        if (gv.get_id() == -1) {continue;}
        active_vertices_ids.push_back(gv.get_id());
        grade_point_2_vertex_edges_id[gv.get_grade()].first.push_back(gv.get_id());
        grades_collection.push_back(gv.get_grade());
    }
    for (const auto& ge: g.get_gedges()){
        if (ge.get_id() == -1) {continue;}
        grade_point_2_vertex_edges_id[ge.get_grade()].second.push_back(ge.get_id());
        grades_collection.push_back(ge.get_grade());
    }
    std::sort(grades_collection.begin(), grades_collection.end(), LexicographicalOrderGradePoint());
    // remove duplicates
    grades_collection.erase(std::unique(grades_collection.begin(), grades_collection.end()), grades_collection.end());
#if MPH0_TIMERS
mph0::create_grades_timer.stop();
#endif

#if MPH0_TIMERS
mph0::grades_iteration_timer.start();
#endif
    
    // Build TopTree
    DTree_TopTree<int, VertexId, std::hash<VertexId>> DT(active_vertices_ids);


    // loop over all GradePoint in lexicographical ordering
    for (const auto& gd_point : grades_collection) {
        // get the grade point and its x,y ranks
        int x = gd_point.get_x();
        int y = gd_point.get_y();

        // get vertices belong to the grade point
        std::vector<VertexId> verts_gd = grade_point_2_vertex_edges_id[gd_point].first;
        for (const auto& v_id: verts_gd){
            betti_0.emplace_back(x, y);
            // row_idx[v] ← |β0|
            row_idx[v_id] = betti_0.size();
        }

        // Check edges
        std::vector<EdgeId> edges_ids_gd = grade_point_2_vertex_edges_id[gd_point].second;
        for (const EdgeId& eid: edges_ids_gd){
            VertexId e_0 = g.get_edge(eid).get_v0(); 
            VertexId e_1 = g.get_edge(eid).get_v1();
            // check if the edge is a self-loop
            if (e_0 == e_1){
                betti_0_1.emplace_back(x, y);
                continue; // self loop only affects betti_0_1
            }
            // find the merge time (i.e. the smallest weight in the connected component)
            auto s = DT.time_of_merge(e_0, e_1); // y-coordinate
            if (s <= y){
                betti_0_1.emplace_back(x, y); // The edge is deletable, so it only affects H1
            }else{ // Edge is not deletable, so belongs to relations in resolution
                DT.merge_at_time(e_0, e_1, y);
                betti_1.emplace_back(x, y);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                // if (s < FT::CoordinateMax){ // The edge is cycle-creating
                if (s < DT.max_edge_weight){ // The edge is cycle-creating
                    betti_2.emplace_back(x, s);
                    betti_0_1.emplace_back(x, s);
                }
            }        

        }// End loop for each edge at gd_point 
    } // End loop for all grid points
#if MPH0_TIMERS
mph0::grades_iteration_timer.stop();
#endif

    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M); 
}


// // Algorithm 3: Betti tables and minimal presentation of R2-filtered graph
// // Return: betti_0, betti_1, 1 sparse Matrix as presentation
// std::tuple<
//     std::vector<std::pair<int, int>>, 
//     std::vector<std::pair<int, int>>, 
//     std::vector<std::tuple<size_t, size_t, int>>
// > compute_minimal_presentation( 
//         GGraph& g,
//         bool visual_DT = false) 
// {

//     // Initialize betti_0, betti_1
//     std::vector<std::pair<int, int>> betti_0;
//     std::vector<std::pair<int, int>> betti_1;

//     // Sparse Matrix for presentation and its row index
//     std::vector<std::tuple<size_t, size_t, int>> M;
//     std::unordered_map<Vertex, size_t> row_idx; 

//     // Collapse to vertex minimal
//     collapse_to_vertex_minimal_Grade_Version(g);

//     // Build Dynamic Tree
//     DynamicTree<int> DT(g.get_vertices(), visual_DT);

//     // dict mapping a GradePoint to (vertices, edges)
//     using VEsTuple = std::pair<std::vector<Vertex>, std::vector<EdgeId>>;
//     std::unordered_map<GradePoint, VEsTuple, FTHash<GradePoint>> grade_point_2_vertex_edges_id;
//     // set of all grade points for iteration
//     std::set<GradePoint, LexicographicalOrderGradePoint> grades_collection;

//     // Loop over all vertices and edges to fill it
//     for (const auto& [v, gp]: g.get_vert_grades()){
//         grade_point_2_vertex_edges_id[gp].first.push_back(v);
//         grades_collection.insert(gp);
//     }
//     for (const auto& [eid, gp]: g.get_edges_grades()){
//         grade_point_2_vertex_edges_id[gp].second.push_back(eid);
//         grades_collection.insert(gp);
//     }

//     // loop over all GradePoint in lexicographical ordering
//     for (const auto& gd_point : grades_collection) {
//         // get the grade point and its x,y ranks
//         int x = gd_point.get_x();
//         int y = gd_point.get_y();

//         // get vertices belong to the grade point
//         std::vector<Vertex> verts_gd = grade_point_2_vertex_edges_id[gd_point].first;
//         for (const auto& v: verts_gd){
//             betti_0.emplace_back(x, y);
//             // row_idx[v] ← |β0|
//             row_idx[v] = betti_0.size();
//         }

//         // Check edges
//         DT.update_max_edge_weight(y);
//         std::vector<EdgeId> edges_ids_gd = grade_point_2_vertex_edges_id[gd_point].second;
//         for (const EdgeId& eid: edges_ids_gd){
//             auto e = g.get_edge(eid);
//             Vertex e_0 = e[0]; Vertex e_1 = e[1];

//             if (e_0 == e_1) continue; // self loop only affects betti_0_1

//             // extract the edge weight
//             auto s = DT.time_of_merge_double(e_0, e_1);
//             if (s > y){ // Edge is not deletable
//                 DT.merge_at_time(e_0, e_1, y); // merge the edge at time y
//                 betti_1.emplace_back(x, y);
//                 M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1));
//                 M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
//             }        

//         }// End loop for each edge at gd_point 
//     } // End loop for all grid points
    
//     return std::make_tuple(betti_0, betti_1, M); 
// }
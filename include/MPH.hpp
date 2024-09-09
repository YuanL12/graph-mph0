#pragma once
#include "DynamicTree.hpp"
#include "Graph.hpp"
#include "Poset.hpp"
#include "Dendrogram.hpp"
#include <set>
#include <icecream.hpp>

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
                    std::cout << "Remove "<< e << std::endl;
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
    // update vertices and edges
    g2.update_graph(vertex_dict);
}


/*
    Algorithm 2: Collapse to vertex-minimal graph
    Assumption: g1 and g2 are the same when passed. We will only modify g2. 
*/
template<typename FT>
Graph<FT> collapse_to_vertex_minimal(const Graph<FT>& g) {
    Graph<R2> g1 = Graph<R2>(g);
    localCollapse(g, g1);
    Graph<R2> g2 = Graph<R2>(g1); 

    // Initialize dictionary φ with identity map
    // Initialize empty set visited
    std::unordered_map<int, int> vertex_dict;
    std::set<int> visited;
    for (auto v: g1.get_vertices() ) {
        vertex_dict[v] = v;
    }

    // Define (vertex, edge, vertex) type 
    using VEV = std::tuple<int, Edge, int>;
    // Run depth-first search from minimal vertices
    for (auto v: g1.get_vertices() ) {
        FT fv = g1.get_vertex_value(v);
        bool check_minimal = true;
        for (const auto& eid : g1.get_adj(v)) {
            Edge e = g1.get_edge(eid);
            Vertex u = (e[0] == v) ? e[1]: e[0];
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
                    Vertex x = (e[0] == u) ? e[1]: e[0];
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

    // update vertices 
    g2.update_graph(vertex_dict);
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
    
    // typename FT::CoordinateTP;
    Dendrogram<typename FT::CoordinateTP> D(g1.get_vertices());

    std::vector<std::tuple<size_t, size_t, int>> M;
    std::unordered_map<Vertex, size_t> row_idx;

    // get f of graph  
    std::unordered_map<EdgeId, FT> g1_edges_values = g1.get_edges_values();
    std::unordered_map<Vertex, FT> g1_vert_values = g1.get_vert_values();

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
            auto y = gd_point.getY();
            auto x = gd_point.getX();            
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
                if (s < FT::CoordinateMax){ // The edge is cycle-creating
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
> compute_MPH0_DTree(const Graph<FT>& g, bool visual_DT = false) {
    std::vector<FT> betti_0, betti_1, betti_2, betti_0_1;
    std::vector<std::tuple<size_t, size_t, int>> M;
    Graph<FT> g1 = collapse_to_vertex_minimal(g);

    // typename FT::CoordinateTP;
    DynamicTree<typename FT::CoordinateTP> DT(g1.get_vertices(), visual_DT);

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
            auto s = DT.time_of_merge_double(e_0, e_1); // y-coordinate
            auto y = gd_point.getY();
            auto x = gd_point.getX();
            if (e_0 == e_1){
                betti_0_1.emplace_back(x,y);
                continue; // self loop only affects betti_0_1
            }
            else{
                DT.merge_at_time(e_0, e_1, y);
            }
            
            if (s <= y){
                betti_0_1.emplace_back(gd_point); // The edge is deletable, so it only affects H1
            }else{ // Edge is not deletable, so belongs to relations in resolution
                betti_1.emplace_back(gd_point);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                if (s < FT::CoordinateMax){ // The edge is cycle-creating
                    betti_2.emplace_back(x,s);
                    betti_0_1.emplace_back(x,s);
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
> compute_MPH0_DTree_debug(const Graph<FT>& g, bool visual_DT = false) {
    std::vector<FT> betti_0, betti_1, betti_2, betti_0_1;
    // Egde matchings for debug
    std::unordered_map<std::string, std::vector<std::tuple<EdgeId, EdgeId>>> edge_matchings;
    // Initialize with 2 empty vectors
    edge_matchings["b_2"] = std::vector<std::tuple<size_t, size_t>>{};
    edge_matchings["b_0_1"] = std::vector<std::tuple<size_t, size_t>>{};

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
            auto s = DT.time_of_merge_double(e_0, e_1); // y-coordinate 
            auto y = gd_point.getY();
            auto x = gd_point.getX();
            if (e_0 == e_1){
                betti_0_1.emplace_back(x,y);
                edge_matchings["b_0_1"].emplace_back(eid, eid);
                continue; // self loop only affects betti_0_1
            }
            else{
                DT.merge_at_time(e_0, e_1, y);
            }
            
            if (s <= y){
                betti_0_1.emplace_back(gd_point); // The edge is deletable, so it only affects H1
                edge_matchings["b_0_1"].emplace_back(eid, eid);
            }else{ // Edge is not deletable, so belongs to relations in resolution
                betti_1.emplace_back(gd_point);
                M.emplace_back(std::make_tuple(row_idx[e_0], betti_1.size(), -1)); // TODO: check if the index has repetition.
                M.emplace_back(std::make_tuple(row_idx[e_1], betti_1.size(),  1));
                if (s < FT::CoordinateMax){ // The edge is cycle-creating
                    betti_2.emplace_back(x,s);
                    betti_0_1.emplace_back(x,s);
                    // TODO: change the second e_id to s 
                    edge_matchings["b_2"].emplace_back(eid, eid); 
                    edge_matchings["b_0_1"].emplace_back(eid, eid);
                }
            }        

        }// End loop for each edge at gd_point 
    } // End loop for all grid points
    return std::make_tuple(betti_0, betti_1, betti_2, betti_0_1, M); 
}


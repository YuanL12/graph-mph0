#pragma once
#include "Timer.hpp"
#include <queue>
#include "DynamicTree.hpp"
#include "Poset.hpp"
#include <set>
#include <icecream.hpp>
#include "Grade.hpp"
#include "GGraph.hpp"

/*
    Algorithm 1: Collapse local collapsible edges inplace
*/
void local_collapse_edges_Grade_Version(GGraph& g) {
#if MPH0_TIMERS
mph0::collapse_edge_timer.start();
#endif
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
                        stack.push(VEV(v, e, x));
                    }
                }
            }
        }
    }

    // update vertices and edges
#if MPH0_TIMERS
mph0::update_graph_from_collapse_edge_timer.start();
#endif
    g.update_graph(vertex_dict);
#if MPH0_TIMERS
mph0::update_graph_from_collapse_edge_timer.stop();
#endif
#if MPH0_TIMERS
mph0::collapse_edge_timer.stop();
#endif
}


/*
    Algorithm 2: Collapse to vertex-minimal graph 
*/
void collapse_to_vertex_minimal_Grade_Version(GGraph& g) {
    // print the size of adjacency list 
    std::cout << "size of adjacency list before collapse: " << std::endl;
    g.print_size_of_adjacency();

    // local collapse edges
    local_collapse_edges_Grade_Version(g);

    std::cout << "size of adjacency list after edge collapse (algorithm 1): " << std::endl;
    g.print_size_of_adjacency();


#if MPH0_TIMERS
mph0::collapse_vertex_timer.start();
#endif
    // Initialize dictionary φ with identity map
    std::vector<size_t> vertex_dict(g.get_nvertices());
    std::iota(vertex_dict.begin(), vertex_dict.end(), 0); // fill with 0, 1, 2, ...

    // Initialize empty set visited
    std::vector<bool> visited(g.get_nvertices(), false);

    // Define (vertex, edge, vertex) type 
    using VEV = std::tuple<GVertex, GEdge, GVertex>;

    // Run depth-first search from minimal vertices
    for (size_t v_idx = 0; v_idx < g.get_nvertices(); ++v_idx){
        GVertex v = g.get_gvertex(v_idx);
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
                        stack.push(VEV(v, e, x));
                    }
                }
            }
        }
    }

    // update vertices and edges
#if MPH0_TIMERS
mph0::update_graph_from_collapse_vertex_timer.start();
#endif
    g.update_graph(vertex_dict);
#if MPH0_TIMERS
mph0::update_graph_from_collapse_vertex_timer.stop();
#endif
#if MPH0_TIMERS
mph0::collapse_vertex_timer.stop();
#endif
    
    std::cout << "size of adjacency list after collapse (algorithm 2): " << std::endl;
    g.print_size_of_adjacency();
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

    // Collapse to vertex minimal
    collapse_to_vertex_minimal_Grade_Version(g);

    // dict mapping a GradePoint to (vertices, edges)
    //using VEsTuple = std::pair<std::vector<VertexId>, std::vector<EdgeId>>;
    //std::unordered_map<GradePoint, VEsTuple, FTHash<GradePoint>> grade_point_2_vertex_edges_id;
    
    // // set of all grade points for iteration
    // std::set<GradePoint, LexicographicalOrderGradePoint> grades_collection;
    std::vector<std::tuple<int, int>> grades_collection;
    grades_collection.reserve(g.get_nvertices() + g.get_nedges());

    // active vertices labels 
    std::vector<VertexId> active_vertices_ids;
    active_vertices_ids.reserve(g.get_nvertices());

#if MPH0_TIMERS
mph0::create_grades_timer.start();
#endif
    // Loop over all vertices and edges to find acitve vertices and edges
    for (auto gv: g.get_gvertices()){
        if (gv.get_id() == -1) { continue;}
        active_vertices_ids.push_back(gv.get_id());
        //grade_point_2_vertex_edges_id[gv.get_grade()].first.push_back(gv.get_id());
        grades_collection.emplace_back(0, gv.get_id());
    }
    for (const auto& ge: g.get_gedges()){
        if (ge.get_id() == -1) {continue;}
        //grade_point_2_vertex_edges_id[ge.get_grade()].second.push_back(ge.get_id());
        grades_collection.emplace_back(1, ge.get_id());
    }
    std::sort(grades_collection.begin(), grades_collection.end(),
             [&g](std::tuple<int,int> x, std::tuple<int,int> y)
             {
                const GradePoint& gx = std::get<0>(x) == 0 ? g.get_gvertex(std::get<1>(x)).get_grade() : g.get_gedge(std::get<1>(x)).get_grade();
                const GradePoint& gy = std::get<0>(y) == 0 ? g.get_gvertex(std::get<1>(y)).get_grade() : g.get_gedge(std::get<1>(y)).get_grade();
                return GradePoint::lexicographical_less_than(gx, gy) || (gx == gy && std::get<0>(x) < std::get<0>(y));
             });
#if MPH0_TIMERS
mph0::create_grades_timer.stop();
#endif

#if MPH0_TIMERS
mph0::grades_iteration_timer.start();
#endif
    
    // Build TopTree
    DTree_TopTree<int, VertexId, std::hash<VertexId>> DT(active_vertices_ids);


    // loop over all GradePoint in lexicographical ordering
    for (const auto& gg : grades_collection) {
        GradePoint gd_point = std::get<0>(gg) == 0 ? g.get_gvertex(std::get<1>(gg)).get_grade() : g.get_gedge(std::get<1>(gg)).get_grade();
        // get the grade point and its x,y ranks
        int x = gd_point.get_x();
        int y = gd_point.get_y();

        // get vertices belong to the grade point
        if (std::get<0>(gg) == 0){
            auto v_id = std::get<1>(gg);
            betti_0.emplace_back(x, y);
            // row_idx[v] ← |β0|
            row_idx[v_id] = betti_0.size();
        } else
        {
            auto e_id = std::get<1>(gg);
            VertexId e_0 = g.get_edge(e_id).get_v0(); 
            VertexId e_1 = g.get_edge(e_id).get_v1();
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
};

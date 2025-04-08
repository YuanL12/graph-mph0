import numpy as np
import sys
sys.path.append('../build')
import abmph


# Point cloud to 1-critical filtration
def point_cloud_to_1_critical_filtration(points, x_y_swapped=False):
    """
    Convert a point cloud to a 1-critical filtration.
    Input:
        points: a numpy array of shape (n, d)
    Output:
        vertices: a list of vertices (int)
        edges: a list of edges, each edge is a tuple of two integers
        filt_func_v: a numpy array of shape (n, 2) filtration values for vertices
        filt_func_e: a numpy array of shape (n, 2) filtration values for edges
    """
    # Compute the distance matrix
    D = distance.squareform(distance.pdist(points)) 
    n = D.shape[0]

    # create the degree map
    degrees = np.arange(0, n)
    
    # Create a list of vertices 
    # (n*n corresponding to the pairwise distance matrix, i//n is the index of the point)
    # we consider row-wise indexing
    vertices = np.arange(n*n)

    # For each pair of points, get the sorted rows of the distance matrix
    sorted_edge_lengths = np.sort(D, axis=1)

    # Assign filtration values for the vertices
    filt_func_verties = []
    for idx in range(n):
        # concatenate sorted_edge_lengths[idx] with [0, -1, -2, -3, ...] to form a 2D array,
        # it will be used as the filtration values for the vertices
        filt_func_verties.append(np.column_stack((sorted_edge_lengths[idx], -degrees)))
    filt_func_verties = np.vstack(filt_func_verties)

    # Add edges purely on the vertices
    edges = []
    for idx in range(n):
        for jdx in range(n-1):
            # each row has n-1 edges
            row_start_index = idx*n
            edges.append([row_start_index + jdx, row_start_index + jdx + 1 ])
    edges = np.vstack(edges)

    # Add corresponding egde filtration values
    inds = - np.arange(0, n)
    filt_func_edges = [np.column_stack([sorted_edge_lengths[i][1:], inds[:-1]]) for i in range(n)]
    filt_func_edges = np.vstack(filt_func_edges)
    
    
    # Loop over n choose pairs of vertices to add "true" edges 
    pair_edges = [] # edges from pair of vertices 
    filt_func_edges_pair_vertices = [] # filtration values on them
    for i in range(n):
        for j in range(i+1,n):
            
            # take maximum of edges lengths of v[i] and v[j]
            max_rs = np.max([sorted_edge_lengths[i], sorted_edge_lengths[j]], axis= 0)

            # the edge appears at length that is greater than or equal to the distance d(v_i, v_j)
            for index, r in enumerate(max_rs):
                if r >= D[i,j]:
                    # Add the edge filtration value
                    filt_func_edges_pair_vertices.append([max_rs[index], -degrees[index]])

                    # determine the indices of its two end points in pairwise distance matrix
                    i_PD_idx, j_PD_idx = i*n + index, j*n + index

                    # Add the edge
                    pair_edges.append([i_PD_idx, j_PD_idx])
    
    # Append those edges to above 
    pair_edges = np.vstack(pair_edges)
    filt_func_edges_pair_vertices = np.vstack(filt_func_edges_pair_vertices)
    
    edges = np.vstack([edges, pair_edges])
    filt_func_edges = np.vstack([filt_func_edges, filt_func_edges_pair_vertices])

    # swap the two columns for filtration function of the vertices and edges
    if x_y_swapped:
        filt_func_edges = filt_func_edges[:, [1, 0]]
        filt_func_verties = filt_func_verties[:, [1, 0]]

    return vertices, edges, filt_func_verties, filt_func_edges


def compute_abs_mph0(vertices, edges, node_features, edge_features):
    vertices = np.arange(node_features.shape[0], dtype=np.int32)
    assert node_features.dtype == np.float64 and "node_features should be float64"
    assert edge_features.dtype == np.float64 and "edge_features should be float64"
    # Create the Graph object
    mph_g = abmph.Graph(vertices, node_features, edges, edge_features)
    res = abmph.compute_MPH0_DTree(mph_g) 
    return res

def compute_betti0_betti1_vertex(pd_matrix: np.ndarray) -> tuple:
    """
    Compute Betti 0 and Betti 1 from the vertices of a point cloud.
    :param pd_matrix: (np.ndarray) Pairwise distance matrix of the point cloud.
    :return: (tuple) Betti 0 and Betti 1.
    """
    # given vertices positions, compute betti 0 and betti 1 from the vertices 
    sorted_edge_lengths = np.sort(pd_matrix, axis=1)
    n = pd_matrix.shape[0]
    inds = - np.arange(0, n)
    betti0s_v = [np.column_stack([sorted_edge_lengths[i][0:], inds]) for i in range(n)] # vertex i 
    betti1s_v = [np.column_stack([sorted_edge_lengths[i][1:], inds[:-1]]) for i in range(n)]

    # stack the results into a single array of shape (*, 2)
    betti0s_v = np.vstack(betti0s_v)
    betti1s_v = np.vstack(betti1s_v)
    return betti0s_v, betti1s_v


def contruct_2_filtration_boosted(D: np.ndarray, x_y_swapped=False):
    n = D.shape[0]
    vertices = np.arange(n)
    # for each pair of points, get the sorted rows of the distance matrix
    sorted_edge_lengths = np.sort(D, axis=1)
    # loop over n choose 2 pairs (all edges)
    edges = []
    # each edge will have some copies 
    edge_2_points = {}
    for i in range(n):
        for j in range(i+1,n):
            # take maximum of edges lengths of v[i] and v[j]
            max_rs = np.max([sorted_edge_lengths[i], sorted_edge_lengths[j]], axis= 0)
            degrees = np.arange(0,n,1)
            # find the first edge length that is greater than or equal to the distance between (v[i],v[j])
            for index, r in enumerate(max_rs):
                if r >= D[i,j]:
                    edge_2_points[(i,j)] = [max_rs[index:], -degrees[index:]]
                    break

    # each edge will have n copies with filtration values from the dictionary
    filt_func_edges = []
    for i, j in edge_2_points.keys():
        filtration_values_per_edge = np.column_stack(edge_2_points[(i,j)])
        len_filt_values = filtration_values_per_edge.shape[0]
        edges.append([[i, j] for _ in range(len_filt_values)])
        filt_func_edges.append(filtration_values_per_edge)
    
    filt_func_edges = np.vstack(filt_func_edges)
    edges = np.vstack(edges)

    # filtration function for the vertices
    filt_func_verties = np.zeros((n, 2), dtype=np.float64)
    for idx in range(n):
        filt_func_verties[idx][0] = 0
        filt_func_verties[idx][1] = - n
    
    # swap the two columns for filtration function of the vertices and edges
    if x_y_swapped:
        filt_func_edges = filt_func_edges[:, [1, 0]]
        filt_func_verties = filt_func_verties[:, [1, 0]]

    return vertices, edges, filt_func_edges, filt_func_verties


def safe_concatenate(arr1, arr2):
    """
    Safely concatenate two 2D arrays, handling the case where one or both might be empty.
    """
    # Check if either array is empty
    if arr1.size == 0:
        return arr2  # Return arr2 if arr1 is empty
    elif arr2.size == 0:
        return arr1  # Return arr1 if arr2 is empty
    else:
        return np.concatenate((arr1, arr2), axis=0) 
    
    
def compute_absolute_MPH0_multi_critical_filtration(pd_matrix: np.ndarray) -> tuple:
    """
    compute the absolute persistence homology of a point cloud.
    The 2-filtration is on graphs with two paremeters: edge lengths and vertex degrees.  
    It is a multi-critical filtration, which means edges can appear at multiple filtration values.
    """
    betti0s_v, betti1s_v = compute_betti0_betti1_vertex(pd_matrix)
    vertices, edges, filt_func_e, filt_func_v = contruct_2_filtration_boosted(pd_matrix)
    mph_res = compute_abs_mph0(vertices, edges, filt_func_v, filt_func_e)
    betti1s_e, betti2s_e = np.array(mph_res['b_1']), np.array(mph_res['b_2'])
    betti1s = safe_concatenate(betti1s_v, betti1s_e)
    return betti0s_v, betti1s, betti2s_e

def compute_grid_H0(m, x_range, y_range, betti0s, betti1s, betti2s):
    """
    Compute the H_0 value on a grid. H_0(s, k) = b_0(s, k) - b_1(s, k) + b_2(s, k)
        m (int): grid shape
        x_range (tuple): x range
        y_range (tuple): y range
    """
    # setup the meshgrid
    x = np.linspace(x_range[0], x_range[1], m)
    y = np.linspace(y_range[0], y_range[1], m)
    H0_meshgrid = np.meshgrid(x, y)
    
    # loop over the meshgrid to compute the H_0 value
    H0_values = np.zeros((m, m), dtype=np.int32)
    for i in range(m):
        for j in range(m):
            s, k = H0_meshgrid[0][i][j], H0_meshgrid[1][i][j]
            b0 = np.sum((betti0s[:, 0] <= s) & (betti0s[:, 1] <= k))
            b1 = np.sum((betti1s[:, 0] <= s) & (betti1s[:, 1] <= k))
            b2 = np.sum((betti2s[:, 0] <= s) & (betti2s[:, 1] <= k))
            H0_values[i][j] = b0 - b1 + b2
    return H0_values


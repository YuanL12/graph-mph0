from ._abmph import *
def compute_MPH0_DTree(graph):
    """
    Compute the absolute 2-parameter persistence homology by Dynamic Tree, Return a dictionary 

    This function takes a graph as input and applies a Dynamic Tree-based algorithm to compute
    the MPH0, which is a measure of topological features related to the paths in the graph.

    Args:
        graph (Graph): The input graph for which the MPH0 is to be computed. This should be a 
            graph object which can constructed by ambph.graph()

    Returns:
        MPH0_DTree (dict): A dictionary containing the results of the MPH0 computation, including 
            b_0: bett_0(H_0)
            b_1: bett_1(H_0)
            b_2: bett_2(H_0)
            b_0_1: bett_1(H_0) 

    Examples:
        >>> result = abmph.compute_MPH0_DTree(G)
        >>> print(result)

    Notes:
        Assumption of the graph:....
    """
    b_0, b_1, b_2, b_0_1, M = compute_MPH0_DTree_CXX(graph) 
    return {'b_0': b_0,
            'b_1': b_1,
            'b_2': b_2,
            'b_0_1': b_0_1,
             'M': M
            }


def compute_MPH0_Dengrogram(graph):
    """
    Compute the absolute 2-parameter persistence homology by Dendrogram, Return a dictionary 

    This function takes a graph as input and applies a Dendrogram-based algorithm to compute
    the MPH0, which is a measure of topological features related to the paths in the graph.

    Args:
        graph (Graph): The input graph for which the MPH0 is to be computed. This should be a 
            graph object which can constructed by ambph.graph()

    Returns:
        MPH0_DTree (dict): A dictionary containing the results of the MPH0 computation, including 
            b_0: bett_0(H_0)
            b_1: bett_1(H_0)
            b_2: bett_2(H_0)
            b_0_1: bett_1(H_0) 

    Examples:
        >>> result = abmph.compute_MPH0_DTree(G)
        >>> print(result)

    Notes:
        Assumption of the graph:....
    """
    b_0, b_1, b_2, b_0_1, M = compute_MPH0_Dengrogram_CXX(graph) 
    return {'b_0': b_0,
            'b_1': b_1,
            'b_2': b_2,
            'b_0_1': b_0_1,
             'M': M
            }
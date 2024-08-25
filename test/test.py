import sys
sys.path.append('/root/Codes/graph-mph0/build')
import abmph
import numpy as np

# Define node features and edge features as 2D arrays
node_features = np.array([[1.0, 2.0], [3.0, 4.0]], dtype=float)
edge_features = np.array([[5.0, 6.0], [7.0, 8.0]], dtype=float)

# Define node labels and edges
node_labels = np.array([0, 1], dtype=int)
edges_input = np.array([[0, 1],[1, 0]], dtype=int)

# Create the Graph object
graph = abmph.Graph(node_labels, node_features, edges_input, edge_features)

graph.print_filtration_value()
print("start compute MPH0")
abmph.compute_MPH0_DTree(graph)
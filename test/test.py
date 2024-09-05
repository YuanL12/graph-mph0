import sys
sys.path.append('build')
import abmph
import numpy as np
import pygraphviz as pgv
import matplotlib.pyplot as plt
from PIL import Image

n_nodes = 4

nodes = np.arange(1, n_nodes+1, dtype=int)
node_features = np.zeros(shape=(n_nodes, 2), dtype = np.double)

edges =         np.array([[1,3],     [1, 2],    [2,3],     [1,4],   ], dtype = int)
edge_features = np.array([[6.1,6.7], [5.7,7.6], [7.5,5.3], [8.1,7.6]], dtype = np.double)
edge_str_labels = [f"({row[0]},{row[1]})" for row in edge_features]

# Create a new AGraph (directed graph)
G = pgv.AGraph(strict=False, directed=False)
for i, row in enumerate(edges):
    G.add_edge(row[0], row[1], label=edge_str_labels[i])
# Draw the graph to a file
G.layout(prog='dot',args="-Efontsize=8")  # Use the 'dot' layout engine
G.draw('test_graph.png')
# save grid plot
plt.figure(figsize=(3, 3), dpi=200)
plt.scatter(edge_features[:,0], edge_features[:,1], s = 5)
plt.axis('equal')
plt.xlim(5, 10)
plt.ylim(5, 10)
# Annotate each point with its (x, y) coordinates
for i in range(len(edge_features)):
    x, y = edge_features[i, 0], edge_features[i, 1]
    plt.text(x, y, f'({x}, {y})', fontsize=4, ha='right', va='bottom')

plt.grid(True, linestyle='--', color='gray', alpha=0.2)

plt.savefig("test_grid.png")

# Create the mph Graph object
mph_g = abmph.Graph(nodes, node_features, edges, edge_features)
print("mph graph:")
mph_g.print_adjacency()
mph_g.print_filtration_value()

print("start compute MPH0 by Dengrogram")
print(abmph.compute_MPH0_Dengrogram(mph_g))

print("start compute MPH0 by DTree")
print(abmph.compute_MPH0_DTree(mph_g))
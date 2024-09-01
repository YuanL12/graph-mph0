import sys
sys.path.append('build')
import abmph
import numpy as np
import pygraphviz as pgv
import matplotlib.pyplot as plt
from PIL import Image

np.random.seed(10)
n_nodes = 4
n_edges = 6
nodes = np.arange(1, n_nodes+1, dtype=int)
edges = np.random.randint(1, n_nodes+1, size=(n_edges, 2))
node_features = np.zeros(shape=(n_nodes, 2), dtype = np.double)
edge_features = np.random.randint(1, 10, size=(n_edges, 2)).astype(np.double)
edge_str_labels = [f"({int(row[0])},{int(row[1])})" for row in edge_features]

# Create a new AGraph (directed graph)
G = pgv.AGraph(strict=False, directed=False)

for i, row in enumerate(edges):
    G.add_edge(row[0], row[1], label=edge_str_labels[i])
    
# Draw the graph to a file
G.layout(prog='dot',args="-Efontsize=8")  # Use the 'dot' layout engine
G.draw('graph.png')

# Display the graph using PIL
print("save graph at graph.png")
img = Image.open('graph.png')


# Create the mph Graph object
mph_g = abmph.Graph(nodes, node_features, edges, edge_features)
print("mph graph:")
mph_g.print_
mph_g.print_filtration_value()

print("start compute MPH0 by Dengrogram")
abmph.compute_MPH0_Dengrogram(mph_g)

print("start compute MPH0 by DTree")
abmph.compute_MPH0_DTree(mph_g)
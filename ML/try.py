import sys
sys.path.append('build')
import abmph
import numpy as np
import os
import torch
from torch_geometric.datasets import MoleculeNet

from sklearn.linear_model import LogisticRegression
from sklearn.model_selection import train_test_split


def gaussian_2d(x, y, x0, y0, sigma):
    """Calculate the value of a 2D Gaussian at point (x, y) centered at (x0, y0) with standard deviation sigma."""
    return np.exp(-((x - x0) ** 2 + (y - y0) ** 2) / (2 * sigma ** 2))

def determine_grid_range(x_min, y_min, x_max, y_max, expansion_factor=0.4):
    """Determine the grid range by expanding the min/max by a given factor of the range."""
    # x_min, y_min = np.min(centers, axis=0)
    # x_max, y_max = np.max(centers, axis=0)
    
    # Calculate the range
    x_range = x_max - x_min
    y_range = y_max - y_min
    
    # Expand the range by the given factor
    x_min -= expansion_factor * x_range
    x_max += expansion_factor * x_range
    y_min -= expansion_factor * y_range
    y_max += expansion_factor * y_range
    
    return [x_min, x_max], [y_min, y_max]

def sum_gaussians(centers, x_range, y_range,  sigma, m):
    """Sum over Gaussians centered at given points and discretize into an m x m matrix."""
    # Create a grid of points within the specified range
    x = np.linspace(x_range[0], x_range[1], m)
    y = np.linspace(y_range[0], y_range[1], m)
    X, Y = np.meshgrid(x, y)
    
    # Initialize the matrix to store the sum of Gaussians
    Z = np.zeros((m, m))
    
    # Sum over each Gaussian centered at the given points
    for (x0, y0) in centers:
        Z += gaussian_2d(X, Y, x0, y0, sigma)
    
    return Z

def compute_abs_mph0(input_graph):
    # set graph features
    n_nodes = input_graph.num_nodes
    nodes = np.arange(0, n_nodes, dtype=int)
    edges = input_graph['edge_index'].reshape(-1, 2).numpy()
    node_features = np.zeros(shape=(n_nodes, 2), dtype = np.double)
    edge_features = input_graph['edge_attr'][:,[0,2]].numpy().astype(np.double)
    
    # Create the Graph object
    mph_g = abmph.Graph(nodes, node_features, edges, edge_features)
    res = abmph.compute_MPH0_DTree(mph_g) 
    return res

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


# load data
dataset = MoleculeNet(root='../data/MoleculeNet', name='ClinTox')
print()
print(f'Dataset: {dataset}:')
print('====================')
print(f'Number of graphs: {len(dataset)}')
print(f'Number of features: {dataset.num_features}')
print(f'Number of classes: {dataset.num_classes}')

data = dataset[0]  # Get the first graph object.

print()
print('Gather some statistics about the first graph.')
print(data)
print('=============================================================')

# Gather some statistics about the first graph.
print(f'Number of nodes: {data.num_nodes}')
print(f'Number of edges: {data.num_edges}')
print(f'Average node degree: {data.num_edges / data.num_nodes:.2f}')
print(f'Has isolated nodes: {data.has_isolated_nodes()}')
print(f'Has self-loops: {data.has_self_loops()}')
print(f'Is undirected: {data.is_undirected()}')

# compute mph_0
bettis= []
labels = []
initial_center = compute_abs_mph0(dataset[0])['b_1'][0]
x_min, y_min = initial_center
x_max, y_max = initial_center
for i, graph in enumerate(dataset):
    labels.append(torch.argmax(graph['y']).item())
    abs_res = compute_abs_mph0(graph)
    b_1_arr = np.array(abs_res['b_1'])
    b_2_arr = np.array(abs_res['b_2'])
    bettis.append([b_1_arr, b_2_arr])

    # Update min and max for x and y
    b_arr = safe_concatenate(b_1_arr, b_2_arr)
    if len(b_arr) == 0:
        continue
    x_min = min(x_min, np.min(b_arr[:, 0]))
    x_max = max(x_max, np.max(b_arr[:, 0]))
    y_min = min(y_min, np.min(b_arr[:, 1]))
    y_max = max(y_max, np.max(b_arr[:, 1]))

labels = np.array(labels)


# compute persistence image
imgs = []
sigma = 0.2  # Spread of the Gaussian
m = 100  # Size of the matrix
expansion_factor  = 0.4

# Determine the grid range automatically
x_range, y_range = determine_grid_range(x_min, y_min, 
                                        x_max, y_max,
                                        expansion_factor)
print(f'x_range = {x_range}, y_range = {y_range}')
for b_1, b_2 in bettis:
    centers = b_1
    # Compute the sum of Gaussians with automatic grid range determination
    Z = sum_gaussians(centers, x_range, y_range, sigma, m)
    imgs.append(Z)

imgs_array = np.array([img.flatten() for img in imgs])


# Logistic Regression
X_train, X_test, y_train, y_test = train_test_split(imgs_array, labels, test_size=0.40, random_state=42)
lr = LogisticRegression()
lr.fit(X_train, y_train)
print(f'final accuracy = {lr.score(X_test, y_test)}')
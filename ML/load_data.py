from torch_geometric.datasets import TUDataset
# Load the IMDB-MULTI dataset
def load_IMDB_MULTI(path = '../data/IMDB-MULTI'):
    dataset = TUDataset(root=path, name='IMDB-MULTI')

    # Display basic information about the dataset
    print(f'Dataset: {dataset}')
    print(f'Number of graphs: {len(dataset)}')
    print(f'Number of features: {dataset.num_features}')
    print(f'Number of classes: {dataset.num_classes}')

    print('=============================================================')
    print("The first graph object:")
    data = dataset[0]  # Get the first graph object.
    print(data)
    # Gather some statistics about the first graph.
    print(f'Number of nodes: {data.num_nodes}')
    print(f'Number of edges: {data.num_edges}')
    print(f'Average node degree: {data.num_edges / data.num_nodes:.2f}')
    print(f'Has isolated nodes: {data.has_isolated_nodes()}')
    print(f'Has self-loops: {data.has_self_loops()}')
    print(f'Is undirected: {data.is_undirected()}')
    return dataset



def load_MUTAG(path = '../data/TUDataset'):
    dataset = TUDataset(root=path, name='MUTAG')
    return dataset







"""
Learning metrics for persistence-based summaries and applications for graph classification

Let w:R^2 -> R be a weight function on the R^2 plane
1. k_w(PI_A,PI_B): a kernel between two persistence images called WKPI
2. D_w(A,B): a distance between two persistence diagrams A,B called 
3. TC: total costs computed by the sum of fractional costs of in-class/out-class losses
"""

import numpy as np


def WKPI_kernel(weight_matrix, PI_A, PI_B, sigma):
    """
    Compute the weighted persistence image kernel (WKPI)
    Input: 
        weight_matrix: a matrix of weights of shape m*n
        PI_A: the persistence image of diagram A as a matrix of shape m*n
        PI_B: the persistence image of diagram B as a matrix of shape m*n
        sigma: the bandwidth parameter of Gaussain 
    """
    m, n = PI_A.shape
    K = 0
    for i in range(m):
        for j in range(n):
            K += weight_matrix[i,j] * np.exp(- (PI_A[i,j] - PI_B[i,j])**2 / (2*sigma**2))
    return K

def WKPI_distance(weight_matrix, PI_A, PI_B, sigma):
    """
    Compute the weighted persistence image distance (WKPI)
    Input: 
        weight_matrix: a matrix of weights of shape m*n
        PI_A: the persistence image of diagram A as a matrix of shape m*n
        PI_B: the persistence image of diagram B as a matrix of shape m*n
        sigma: the bandwidth parameter of Gaussain 
    """
    return np.sqrt(WKPI_kernel(weight_matrix, PI_A, PI_A, sigma) + 
                   WKPI_kernel(weight_matrix, PI_B, PI_B, sigma) -
                   2 * WKPI_kernel(weight_matrix, PI_A, PI_B, sigma))

def clustering_weight_loss(weight_matrix, PIs_cluster_dict, sigma):
    """
    Compute the clustering loss of weighted persistence image
    Input: 
        weight_matrix: a matrix of weights of shape m*n
        PIs_cluster_dict: a dictionary of persistence images of each cluster
        sigma: the bandwidth parameter of Gaussain 
    """
    loss = 0
    for cluster in PIs_cluster_dict:
        # get the PIs of the current cluster C_i
        PIs = PIs_cluster_dict[cluster]

        # compute the in-class loss and out-class loss
        in_class_loss = 0
        out_class_loss = 0
        for i in range(len(PIs)):
            # in-class loss 
            for j in range(len(PIs)):
                in_class_loss += WKPI_distance(weight_matrix, PIs[i], PIs[j], sigma)

            # out-class loss
            for other_cluster in PIs_cluster_dict:
                other_PIs = PIs_cluster_dict[other_cluster]
                for other_PI in other_PIs:
                    out_class_loss += WKPI_distance(weight_matrix, PIs[i], other_PI, sigma)
        
        # per cluster fractional loss
        loss += in_class_loss / out_class_loss
        
    return loss

# Jaccard index function on a graph
def jaccard_index(A, B):
    """
    Compute the Jaccard index of two sets A and B
    Input: 
        A: a set of elements
        B: a set of elements
    """
    intersection = A.intersection(B)
    union = A.union(B)
    return len(intersection) / len(union)
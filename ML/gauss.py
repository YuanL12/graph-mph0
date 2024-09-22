import numpy as np
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
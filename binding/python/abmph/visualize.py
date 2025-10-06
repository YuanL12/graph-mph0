import numpy as np
from matplotlib import pyplot as plt
import matplotlib.colors as mcolors


def convert_betti_numbers_to_2d_array(
    betti_numbers: list[tuple[int, int]],
    m: int,
    n: int,
):
    """
    Convert the Betti numbers to a 2D array.

    - betti_numbers: list of betti numbers' indices, e.g., [(0, 0), (0, 1), (1, 0), (1, 1)]
    - m: number of rows in the Betti table
    - n: number of columns in the Betti table
    """
    betti_array = np.zeros((m, n), dtype=np.int32)
    for betti_number in betti_numbers:
        x, y = betti_number  # x, y is the coordinate index
        betti_array[x][y] += 1
    return betti_array


def compute_Hilbert_matrix(absolute_betti_numbers_result_dict, m, n):
    """
    Compute the Hilbert function in matrix form from the absolute betti numbers.
    Note it is not an ideal choice to visualize because the size of m, n is too large.
    Args:
        - absolute_betti_numbers_result_dict: a dictionary of absolute betti numbers,
        e.g., {"b_0": [(0, 0), (0, 1), (1, 0), (1, 1)], "b_1": [(0, 0), (0, 1), (1, 0), (1, 1)], "b_2": [(0, 0), (0, 1), (1, 0), (1, 1)]}
        - m: number of rows in the Betti table
        - n: number of columns in the Betti table
    Returns:
        Hilbert_matrix: a 2D array of the Hilbert matrix
    """
    b_0_array = convert_betti_numbers_to_2d_array(
        absolute_betti_numbers_result_dict["b_0"], m, n
    )
    b_1_array = convert_betti_numbers_to_2d_array(
        absolute_betti_numbers_result_dict["b_1"], m, n
    )
    b_2_array = convert_betti_numbers_to_2d_array(
        absolute_betti_numbers_result_dict["b_2"], m, n
    )

    return b_0_array - b_1_array + b_2_array


def map_graded_betti_numbers_to_filtration_values(
    graded_betti_numbers: list[tuple[int, int]],
    filtration_values_x: list[float],
    filtration_values_y: list[float],
):
    """
    Map the graded betti numbers to the filtration values.
    (x, y) -> (x_value, y_value)
    Args:
        graded_betti_numbers: list of graded betti numbers
        filtration_values_x: list of filtration values in x coordinate
        filtration_values_y: list of filtration values in y coordinate
    Returns:
        graded_betti_numbers_in_filtration_values: list of graded betti numbers in filtration values
    """
    graded_betti_numbers_in_filtration_values = []
    for betti_number in graded_betti_numbers:
        x, y = (
            betti_number  # x, y is the index of the betti number in the filtration_values_x and filtration_values_y
        )
        graded_betti_numbers_in_filtration_values.append(
            (filtration_values_x[x], filtration_values_y[y])
        )
    return graded_betti_numbers_in_filtration_values


def draw_discrete_matrix(
    mat, x_axis_range=None, y_axis_range=None, origin="upper", discrete_colorbar=True
):
    """
    Draw the discrete matrix.

    Args:
        mat (np.ndarray): the matrix to draw
        x_axis_range (tuple): the range of the x axis
        y_axis_range (tuple): the range of the y axis
        origin (str): the origin of the matrix
        discrete_colorbar (bool): whether to draw the discrete color bar
    Returns:
        plt: the plot
        cbar: the color bar
    """
    # Define the boundaries for the discrete color bar
    boundaries = np.arange(np.min(mat), np.max(mat) + 2) - 0.5
    norm = mcolors.BoundaryNorm(boundaries, plt.cm.viridis.N)

    # Plot the H_0 values with discrete color bar
    if (
        x_axis_range is None or y_axis_range is None
    ):  # if no range is provided, use the default range
        plt.imshow(mat, origin=origin, aspect="auto", norm=norm, cmap="viridis")
    else:  # if range is provided, use the provided range
        plt.imshow(
            mat,
            extent=(*x_axis_range, *y_axis_range),
            origin=origin,
            aspect="auto",
            norm=norm,
            cmap="viridis",
        )

    if discrete_colorbar:
        cbar = plt.colorbar(ticks=np.arange(np.min(mat), np.max(mat) + 1))
    else:
        cbar = plt.colorbar()
    # cbar.set_label("$H_0$ values")
    return plt, cbar


if __name__ == "__main__":
    # # example usage
    # res = {
    #     "b_0": [(0, 0), (0, 1), (1, 0), (1, 1)],
    #     "b_1": [(0, 0), (0, 1), (1, 0), (1, 1)],
    #     "b_2": [(0, 0), (0, 1), (1, 0), (1, 1)],
    # }
    # table_x_coords = [0, 0.5, 1]
    # table_y_coords = [0, 0.5, 1]
    # m, n = len(table_x_coords), len(table_y_coords)
    # # here is the Hilbert matrix
    # Hibert_matrix = compute_Hilbert_matrix(res, m, n)
    pass

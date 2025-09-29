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


def compute_approximated_Hilbert_matrix(
    matrix_shape: tuple[int, int],
    row_range: tuple[float, float],
    column_range: tuple[float, float],
    betti0s: np.ndarray,
    betti1s: np.ndarray,
    betti2s: np.ndarray,
):
    """
    Compute the approximated Hilbert function (dimension of H_0) on a pre-defined grid.
    The formula is: H_0(s, k) = b_0(s, k) - b_1(s, k) + b_2(s, k)
    Args:
        matrix_shape (tuple): the shape of the matrix
        row_range (tuple): row value range
        column_range (tuple): column value range
        betti0s (2D np.ndarray ): betti0s, each row is a betti number in the format of (s, k)
        betti1s (2D np.ndarray): betti1s, each row is a betti number
        betti2s (2D np.ndarray): betti2s, each row is a betti number
    Returns:
        H0_values (np.ndarray): H_0 values
    """
    m, n = matrix_shape
    # setup the meshgrid
    xs = np.linspace(row_range[0], row_range[1], m)
    ys = np.linspace(column_range[0], column_range[1], n)
    H0_meshgrid = np.meshgrid(xs, ys, indexing="ij")

    # loop over the meshgrid to compute the H_0 value
    H0_values = np.zeros((m, n), dtype=np.int32)
    for i in range(m):
        for j in range(n):
            s, k = H0_meshgrid[0][i][j], H0_meshgrid[1][i][j]
            b0 = np.sum((betti0s[:, 0] <= s) & (betti0s[:, 1] <= k))
            b1 = np.sum((betti1s[:, 0] <= s) & (betti1s[:, 1] <= k))
            b2 = np.sum((betti2s[:, 0] <= s) & (betti2s[:, 1] <= k))
            H0_values[i][j] = b0 - b1 + b2
    return H0_values


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
    cbar.set_label("$H_0$ values")
    return plt


def draw_Hilbert_matrix_of_degree_rips_filtration(
    res: dict[str, list[tuple[int, int]]],
    table_x_coords: list[float],
    table_y_coords: list[float],
    matrix_shape: tuple[int, int] = (50, 50),
    degree_threshold: int = 3,
):
    # convert the graded betti numbers in indices to filtration values
    graded_betti_numbers_in_filtration_values = {}
    for name in ["b_0", "b_1", "b_2"]:
        graded_betti_numbers_in_filtration_values[name] = (
            map_graded_betti_numbers_to_filtration_values(
                res[name], table_x_coords, table_y_coords
            )
        )

    # compute the H_0 values on a grid
    H_0_values = compute_approximated_Hilbert_matrix(
        matrix_shape=matrix_shape,
        row_range=(np.min(table_x_coords), np.max(table_x_coords)),
        column_range=(np.min(table_y_coords), np.max(table_y_coords)),
        betti0s=np.array(graded_betti_numbers_in_filtration_values["b_0"]),
        betti1s=np.array(graded_betti_numbers_in_filtration_values["b_1"]),
        betti2s=np.array(graded_betti_numbers_in_filtration_values["b_2"]),
    )

    # mask the H_0 values that are less than degree_threshold
    H_0_values_masked = np.where(H_0_values >= degree_threshold, -1, H_0_values)

    # when drawing in Eucledian space, the matrix should be transposed
    H_0_values_masked = H_0_values_masked.T

    # reverse the degree axis
    H_0_values_masked = np.flip(H_0_values_masked, axis=0)

    # draw the H_0 values
    discrete_plot = draw_discrete_matrix(
        H_0_values_masked,
        x_axis_range=(np.min(table_x_coords), np.max(table_x_coords)),
        y_axis_range=(-np.max(table_y_coords), -np.min(table_y_coords)),
        discrete_colorbar=True,
        origin="lower",
    )
    return discrete_plot


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

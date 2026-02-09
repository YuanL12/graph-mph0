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


def compute_cumulative_sum_of_betti_numbers(
    res: dict[str, list[tuple[int, int]]],
    table_x_coords: list[float],
    table_y_coords: list[float],
    target_matrix_shape: tuple[int, int],
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    Compute the cumulative sum of the betti numbers.

    Args:
        res: dictionary of graded betti numbers (indices of the lists of x, y coordinates)
        table_x_coords: list of x coordinates
        table_y_coords: list of y coordinates
        target_matrix_shape: tuple of the target matrix shape

    Returns:
        tuple of the cumulative sum of the betti numbers, e.g.,
            cum_0[i, j] = the number of elements (x, y) in b_0_values that satisfy
            table_x_coords[x] <= matrix_meshgrid[i] and table_y_coords[y] <= matrix_meshgrid[j]

    """
    # convert the graded betti numbers in indices to filtration values
    b_0_values = map_graded_betti_numbers_to_filtration_values(
        res["b_0"], table_x_coords, table_y_coords
    )
    b_0_values = np.array(b_0_values)
    b_1_values = map_graded_betti_numbers_to_filtration_values(
        res["b_1"], table_x_coords, table_y_coords
    )
    b_1_values = np.array(b_1_values)
    b_2_values = map_graded_betti_numbers_to_filtration_values(
        res["b_2"], table_x_coords, table_y_coords
    )
    b_2_values = np.array(b_2_values)

    target_m, target_n = target_matrix_shape
    xs = np.linspace(table_x_coords[0], table_x_coords[-1], target_m)
    ys = np.linspace(table_y_coords[0], table_y_coords[-1], target_n)
    matrix_meshgrid = np.meshgrid(xs, ys, indexing="ij")

    cumsum_b_0 = np.zeros((target_m, target_n), dtype=np.int32)
    cumsum_b_1 = np.zeros((target_m, target_n), dtype=np.int32)
    cumsum_b_2 = np.zeros((target_m, target_n), dtype=np.int32)
    for i in range(target_m):
        for j in range(target_n):
            s, k = matrix_meshgrid[0][i][j], matrix_meshgrid[1][i][j]
            # counts the number of rows (x,y) in b_0_values that satisfy x <= s and y <= k
            cumsum_b_0[i, j] = np.sum((b_0_values[:, 0] <= s) & (b_0_values[:, 1] <= k))
            cumsum_b_1[i, j] = np.sum((b_1_values[:, 0] <= s) & (b_1_values[:, 1] <= k))
            cumsum_b_2[i, j] = np.sum((b_2_values[:, 0] <= s) & (b_2_values[:, 1] <= k))

    return cumsum_b_0, cumsum_b_1, cumsum_b_2


def compute_clipped_Hilbert_matrix(
    res: dict[str, list[tuple[int, int]]],
    table_x_coords: list[float],
    table_y_coords: list[float],
    target_matrix_shape: tuple[int, int],
    clip_min: int = 0,
    clip_max: int = 25,
) -> np.ndarray:
    """
    Compute the approximated Hilbert matrix.
    Args:
        res: dictionary of graded betti numbers (indices of the lists of x, y coordinates)
        table_x_coords: list of x coordinates
        table_y_coords: list of y coordinates
        target_matrix_shape: tuple of the target matrix shape
        clip_min: minimum value of the matrix
        clip_max: maximum value of the matrix
    Returns:
        approximated Hilbert matrix
    """
    cum_0, cum_1, cum_2 = compute_cumulative_sum_of_betti_numbers(
        res, table_x_coords, table_y_coords, target_matrix_shape
    )
    approx_hilbert_matrix = cum_0 - cum_1 + cum_2
    # map the value greater to 4 to 4
    approx_hilbert_matrix = np.clip(approx_hilbert_matrix, clip_min, clip_max)
    return approx_hilbert_matrix


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

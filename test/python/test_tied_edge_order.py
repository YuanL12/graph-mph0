"""H0 must be independent of the order of equally graded edges (issue #1)."""

from __future__ import annotations

from pathlib import Path
from tempfile import TemporaryDirectory

import graph_mph

VERTICES = [(0, 3), (1, 0), (3, 0), (0, 3), (1, 1), (0, 3)]
EDGES = [
    (0, 1, (2, 3)),
    (0, 3, (1, 4)),
    (1, 5, (4, 3)),
    (2, 3, (4, 3)),
    (2, 4, (3, 2)),
    (3, 4, (1, 3)),
    (4, 5, (2, 4)),
]


def _write_firep(path: Path, edges: list[tuple[int, int, tuple[int, int]]]) -> None:
    lines = ["firep", "x", "y", f"{len(edges)} {len(VERTICES)} 0"]
    lines += [f"{x} {y} ; {u} {v}" for u, v, (x, y) in edges]
    lines += [f"{x} {y} ;" for x, y in VERTICES]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def _h0_at(x: int, y: int, edges: list[tuple[int, int, tuple[int, int]]]) -> int:
    with TemporaryDirectory() as directory:
        path = Path(directory) / "case.firep"
        _write_firep(path, edges)
        filtration = graph_mph.BiFiltration("firep", str(path))
        xs = filtration.get_x_coords()
        ys = filtration.get_y_coords()
        betti = graph_mph.compute_MPH0(filtration.ggraph)
        return sum(
            sign * sum(xs[i] <= x and ys[j] <= y for i, j in betti[name])
            for name, sign in [("b_0", 1), ("b_1", -1), ("b_2", 1)]
        )


def test_tied_edge_order_independent_h0():
    swapped = EDGES.copy()
    swapped[2], swapped[3] = swapped[3], swapped[2]

    h0_original = _h0_at(4, 3, EDGES)
    h0_swapped = _h0_at(4, 3, swapped)

    assert h0_original == 2
    assert h0_swapped == 2


if __name__ == "__main__":
    test_tied_edge_order_independent_h0()
    print("OK")

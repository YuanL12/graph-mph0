import argparse
from collections import Counter
from itertools import islice
from pathlib import Path

def read_rivet_output(file_path):
    xi_0_data = []
    xi_1_data = []
    xi_2_data = []

    current_section = None

    with open(file_path, "r") as f:
        for line in f:
            line = line.strip()
            if line.startswith("xi_0:"):
                current_section = xi_0_data
            elif line.startswith("xi_1:"):
                current_section = xi_1_data
            elif line.startswith("xi_2:"):
                current_section = xi_2_data
            elif line and current_section is not None and line.startswith("("):
                # Convert the tuple string to actual tuple of integers
                try:
                    # Remove parentheses and split by comma
                    values = line.strip("()").split(",")
                    # Convert to integers
                    tuple_data = tuple(int(x.strip()) for x in values)
                    current_section.append(tuple_data)
                except ValueError:
                    continue

    return xi_0_data, xi_1_data, xi_2_data


def read_our_betti(file_path):
    betti_0_data = []
    betti_1_data = []
    betti_2_data = []

    current_section = None

    with open(file_path, "r") as f:
        for line in f:
            line = line.strip()

            # Skip empty lines
            if not line:
                continue

            # Check for section headers
            if line == "betti_0:":
                current_section = betti_0_data
            elif line == "betti_1:":
                current_section = betti_1_data
            elif line == "betti_2:":
                current_section = betti_2_data
            elif line == "betti_0_1:":
                current_section = None  # Ignore this section
            # Process tuples
            elif line.startswith("(") and current_section is not None:
                try:
                    # Remove parentheses and split by comma
                    values = line.strip("()").split(",")
                    # Convert to integers
                    tuple_data = tuple(int(x.strip()) for x in values)
                    current_section.append(tuple_data)
                except ValueError:
                    continue

    return betti_0_data, betti_1_data, betti_2_data

def read_firep_ranks(file_path):
    with Path(file_path).open() as stream:
        if stream.readline().strip() != "firep":
            raise ValueError("expected a FIRep file")
        stream.readline()
        stream.readline()
        n_edges, n_vertices, _ = map(int, stream.readline().split())
        grades = (tuple(map(int, line.split(";", 1)[0].split()))
                  for line in islice(stream, n_edges + n_vertices) if line.strip())
        xs, ys = set(), set()
        for x, y in grades:
            xs.add(x)
            ys.add(y)
    return ({value: rank for rank, value in enumerate(sorted(xs))},
            {value: rank for rank, value in enumerate(sorted(ys))})


def read_mpfree_resolution(file_path, input_path):
    x_rank, y_rank = read_firep_ranks(input_path)
    lines = [line.strip() for line in Path(file_path).read_text().splitlines() if line.strip()]
    if len(lines) < 3 or lines[:2] != ["scc2020", "2"]:
        raise ValueError("expected a 2-parameter SCC2020 resolution")
    counts = [int(value) for value in lines[2].split()]
    if counts[-1] != 0 or len(lines[3:]) != sum(counts[:-1]):
        raise ValueError("invalid SCC2020 resolution counts")

    sections = []
    offset = 3
    for count in counts[:-1]:
        grades = Counter()
        for line in lines[offset : offset + count]:
            x, y = (int(value) for value in line.split(";", 1)[0].split())
            grade = x_rank[x], y_rank[y]
            grades[grade] += 1
        sections.append(sorted((*grade, multiplicity) for grade, multiplicity in grades.items()))
        offset += count
    sections.reverse()
    return tuple((sections + [[], [], []])[:3])


def compare_outputs(our_output_path, rivet_output_path):
    ours = tuple(sorted(section) for section in read_our_betti(our_output_path))
    rivet = tuple(sorted(section) for section in read_rivet_output(rivet_output_path))
    labels = ("beta_0", "beta_1", "beta_2")
    mismatches = [label for label, our, ref in zip(labels, ours, rivet) if our != ref]
    if mismatches:
        raise ValueError(f"mismatch in {', '.join(mismatches)}")

def compare_mpfree(our_output_path, mpfree_output_path, input_path):
    ours = tuple(sorted(section) for section in read_our_betti(our_output_path))
    if ours != read_mpfree_resolution(mpfree_output_path, input_path):
        raise ValueError("mismatch in mpfree resolution Betti grades")



if __name__ == "__main__":
    # Example usage
    parser = argparse.ArgumentParser()
    parser.add_argument("our", nargs="?")
    parser.add_argument("rivet", nargs="?")
    parser.add_argument("mpfree", nargs="?")
    parser.add_argument("firep", nargs="?")
    parser.add_argument("--mpfree-only", nargs=2, metavar=("MPFREE", "FIREP"))
    parser.add_argument("--ours-only", metavar="OTHER")
    args = parser.parse_args()
    if args.ours_only:
        if args.rivet or args.mpfree or args.firep or args.mpfree_only:
            parser.error("use OUR --ours-only OTHER")
        ours = tuple(sorted(section) for section in read_our_betti(args.our))
        other = tuple(sorted(section) for section in read_our_betti(args.ours_only))
        if ours != other:
            raise ValueError("mismatch between canonical Betti outputs")
        print(f"PASS: {args.our} == {args.ours_only}")
        raise SystemExit
    if args.mpfree_only:
        if not args.our or args.rivet or args.mpfree or args.firep:
            parser.error("use OUR --mpfree-only MPFREE FIREP")
        compare_mpfree(args.our, *args.mpfree_only)
        print(f"PASS: {args.our} == {args.mpfree_only[0]}")
        raise SystemExit
    if bool(args.our) != bool(args.rivet):
        parser.error("provide both OUR and RIVET outputs")
    if args.our:
        compare_outputs(args.our, args.rivet)
        references = [args.rivet]
        if args.mpfree:
            if not args.firep:
                parser.error("FIREP is required with MPFREE")
            compare_mpfree(args.our, args.mpfree, args.firep)
            references.append(args.mpfree)
        print(f"PASS: {args.our} == {' == '.join(references)}")
        raise SystemExit

    rivet_output_paths = [
        "~/Documents/graph-mph0/annulus_200_ball_density_output_rivet.txt",
        "~/Documents/graph-mph0/annulus_400_ball_density_output_rivet.txt",
        "~/Documents/graph-mph0/annulus_200_degree_output_rivet.txt",
        "~/Documents/graph-mph0/annulus_400_degree_output_rivet.txt",
    ]
    our_output_paths = [
        "~/Documents/graph-mph0/build/annulus_200_ball_density_exact_our_out.txt",
        "~/Documents/graph-mph0/build/annulus_400_ball_density_exact_our_out.txt",
        "~/Documents/graph-mph0/build/annulus_200_degree_exact_our_out.txt",
        "~/Documents/graph-mph0/build/annulus_400_degree_exact_our_out.txt",
    ]

    for rivet_output_path, our_output_path in zip(rivet_output_paths, our_output_paths):
        try:
            # Start reading the results
            our_output_path = Path(our_output_path).expanduser()
            rivet_output_path = Path(rivet_output_path).expanduser()
            betti_0, betti_1, betti_2 = read_our_betti(our_output_path)

            # Print the results
            print("ours betti_0 len:", len(betti_0))
            print("ours betti_1 len:", len(betti_1))
            print("ours betti_2 len:", len(betti_2))

            xi_0, xi_1, xi_2 = read_rivet_output(rivet_output_path)

            # Print the results
            print("rivet betti_0 len:", len(xi_0))
            print("rivet betti_1 len:", len(xi_1))
            print("rivet betti_2 len:", len(xi_2))

            compare_outputs(our_output_path, rivet_output_path)
            print(f"✅ Compare {rivet_output_path} and {our_output_path} passed")
            print("--------------------------------")
        except Exception as e:
            print(f"❌ Compare {rivet_output_path} and {our_output_path} failed")
            raise e

    print("\n✅ All tests passed!!!")

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


# Example usage
rivet_output_path = (
    # "/home/yluo/Documents/graph-mph0/annulus_400_ball_density_output_rivet.txt"
    "/home/yluo/Documents/graph-mph0/annulus_400_degree_output_rivet.txt"
)
output_path = (
    # "/home/yluo/Documents/graph-mph0/build/annulus_400_ball_density_our_out.txt"
    "/home/yluo/Documents/graph-mph0/build/annulus_400_degree_exact_our_out.txt"
)


# Start reading the results
betti_0, betti_1, betti_2 = read_our_betti(output_path)

# Print the results
print("ours betti_0 len:", len(betti_0))
print("ours betti_1 len:", len(betti_1))
print("ours betti_2 len:", len(betti_2))

xi_0, xi_1, xi_2 = read_rivet_output(rivet_output_path)

# Print the results
print("rivet betti_0 len:", len(xi_0))
print("rivet betti_1 len:", len(xi_1))
print("rivet betti_2 len:", len(xi_2))

# compare the two results
print("--------------------------------")
print("Print the difference")
print("Compare betti_0:")
for i in range(len(xi_0)):
    if xi_0[i] != betti_0[i]:
        print(f"i = {i:3d}, \trivet = {xi_0[i]}, \tours = {betti_0[i]}")

print("Compare betti_1:")
for i in range(len(xi_1)):
    if xi_1[i] != betti_1[i]:
        print(f"i = {i:3d}, \trivet = {xi_1[i]}, \tours = {betti_1[i]}")

print("Compare betti_2:")
for i in range(len(xi_2)):
    if xi_2[i] != betti_2[i]:
        print(f"i = {i:3d}, \trivet = {xi_2[i]}, \tours = {betti_2[i]}")

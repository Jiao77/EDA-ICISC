def rotate_point(x, y, center_x, center_y):
    """Rotate a point 90 degrees clockwise around the given center point."""
    # Translate to origin around the center
    x -= center_x
    y -= center_y
    # Rotate 90 degrees clockwise
    new_x, new_y = y, -x
    # Translate back
    new_x += center_x
    new_y += center_y
    return new_x, new_y

def parse_vertices(vertices_str):
    """Parse a string of vertex coordinates into a list of tuples."""
    return [tuple(map(int, vertex.strip("()").split(","))) for vertex in vertices_str.strip().split("),(")]

def rotate_shape(vertices, center_x, center_y):
    """Rotate a shape 90 degrees clockwise around the given center point."""
    return [rotate_point(x, y, center_x, center_y) for x, y in vertices]

def print_vertices(vertices):
    """Print the vertices in the required format on a single line."""
    vertex_str = ",".join(f"({x[0]},{x[1]})" for x in vertices)
    print(vertex_str)

def main():
    # Example input: a Manhattan shape with vertices
    input_vertices = "(8100,11400),(7500,11400),(7500,8100),(3000,8100),(3000,7800),(2700,7800),(2700,6600),(3900,6600),(3900,7500),(7500,7500),(7500,3300),(6900,3300),(6900,1200),(8100,1200)"
    center_x, center_y = 5100, 6900

    # Rotate the shape
    rotated_vertices = rotate_shape(parse_vertices(input_vertices), center_x, center_y)

    # Print the rotated vertices
    print_vertices(rotated_vertices)

if __name__ == "__main__":
    main()
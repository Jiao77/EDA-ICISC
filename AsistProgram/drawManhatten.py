import matplotlib.pyplot as plt
import matplotlib.patches as patches

def draw_manhattan_shape(ax, vertices, edge_color='black'):
    """
    Draw a Manhattan shape on the given axes.

    Parameters:
    - ax: matplotlib.axes.AxesSubplot, the axes to draw on.
    - vertices: list of tuples, the (x, y) coordinates of the vertices.
    """
    # Close the shape by appending the first vertex at the end
    vertices.append(vertices[0])
    
    # Unpack the x and y coordinates
    x, y = zip(*vertices)
    
    # Create a polygon patch and add it to the axes
    polygon = patches.Polygon(vertices, edgecolor=edge_color, facecolor='none', linewidth=2)
    ax.add_patch(polygon)

def plot_pattern(ax, pattern):
    """
    Plot a pattern with multiple layers.

    Parameters:
    - ax: matplotlib.axes.AxesSubplot, the axes to draw on.
    - pattern: dict, a dictionary containing layer information.
    """
    for layer in pattern:
        if layer != 'marker':
            layer_info = pattern[layer]
            vertices = []
            for vertex in layer_info:
                vertices.append((vertex[0], vertex[1]))
            draw_manhattan_shape(ax, vertices)

# Define the pattern from the txt file
pattern1_layer1 = [
    (2100,12600),(2100,2400),(2700,2400),(2700,5700),(7200,5700),(7200,6000),(7500,6000),(7500,7200),(6300,7200),(6300,6300),(2700,6300),(2700,10500),(3300,10500),(3300,12600)
]

pattern1_marker = [(0,0), (10200,0), (10200,13800), (0,13800)]

# Create a figure and an axes.
fig, ax = plt.subplots()
ax.set_xlim(-100, 10300)
ax.set_ylim(-100, 13900)
ax.set_aspect('equal')

# Plot the pattern
plot_pattern(ax, {'layer1': pattern1_layer1, 'marker': pattern1_marker})

# Show the plot
plt.show()
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def parse_layout(file_content):
    layers = file_content.split('layer')
    polygons = {}
    for layer in layers[1:]:  # 第一个元素是空的，因为文件以'layer'开头
        layer_info = layer.strip().split('\n')
        if not layer_info:
            continue
        layer_name = 'layer' + layer_info[0].strip().split('layer')[1].strip(':')
        polygons[layer_name] = []
        for line in layer_info[1:]:
            if line.strip():
                points = line.strip().split(',')
                x_coords, y_coords = zip(*[int(xy) for xy in points])
                polygons[layer_name].append((list(x_coords), list(y_coords)))
    return polygons

def draw_layout(polygons, ax):
    for layer_name, layer_polygons in polygons.items():
        for polygon in layer_polygons:
            x_coords, y_coords = polygon
            patch = patches.Polygon(list(zip(x_coords, y_coords)), closed=True, edgecolor='black', facecolor='none', linewidth=1)
            ax.add_patch(patch)

# 读取文件内容
with open('../testset/small/small_pattern.txt', 'r') as file:
    file_content = file.read()

# 解析版图数据
polygons = parse_layout(file_content)

# 绘制版图
fig, ax = plt.subplots()
ax.set_xlim(0, 10200)
ax.set_ylim(0, 13800)
ax.set_aspect('equal')
draw_layout(polygons, ax)
plt.show()
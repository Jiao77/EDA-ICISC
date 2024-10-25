import matplotlib.pyplot as plt
import matplotlib.patches as patches

class Node:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.left = None
        self.right = None

def parse_kd_tree(data):
    nodes = []
    node_map = {}
    current_node = None

    for line in data.split('\n'):
        if line.startswith('('):
            coords = line[1:-1].split('),(')
            x, y = int(coords[0].split(',')[0].strip()), int(coords[0].split(',')[1].strip())
            current_node = Node(x, y)
            nodes.append(current_node)
            node_map[str(current_node)] = current_node
        elif line.startswith('left:'):
            left_coords = line[5:-1].split('),(')
            x, y = int(left_coords[0].split(',')[0].strip()), int(left_coords[0].split(',')[1].strip())
            left_node = Node(x, y)
            node_map[str(current_node)].left = left_node
            node_map[str(left_node)] = left_node
        elif line.startswith('right:'):
            right_coords = line[6:-1].split('),(')
            x, y = int(right_coords[0].split(',')[0].strip()), int(right_coords[0].split(',')[1].strip())
            right_node = Node(x, y)
            node_map[str(current_node)].right = right_node
            node_map[str(right_node)] = right_node

    return node_map['']

def draw_kd_tree(node, ax, x_offset=0, y_offset=0):
    if node is not None:
        # 计算实际坐标
        x = node.x + x_offset
        y = node.y + y_offset
        # 绘制节点
        rect = patches.Rectangle((x, y), 1, 1, edgecolor='black', facecolor='lightblue')
        ax.add_patch(rect)
        # 递归绘制左右子树
        draw_kd_tree(node.left, ax, x_offset - 5, y_offset + 5)
        draw_kd_tree(node.right, ax, x_offset + 5, y_offset + 5)

# 读取文件内容
with open('../result/smallpatternKdTree.txt', 'r', encoding='utf-16le') as file:
    file_content = file.read()

# 解析KD树数据
root_node = parse_kd_tree(file_content)

# 绘制KD树
fig, ax = plt.subplots()
draw_kd_tree(root_node, ax)
ax.set_xlim(0, 12000)
ax.set_ylim(0, 13800)
ax.set_aspect('equal')
plt.show()
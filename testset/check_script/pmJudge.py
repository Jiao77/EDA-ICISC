import argparse
import re
from collections import defaultdict

# 结构体定义
class Point:
    def __init__(self, x, y):
        self.x = int(x)
        self.y = int(y)
    
    def __repr__(self):
        return f"({self.x},{self.y})"
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    def __lt__(self, other):
        # 先比较 x 坐标，如果相等再比较 y 坐标
        if self.x == other.x:
            return self.y < other.y
        return self.x < other.x
    def __hash__(self):
        return hash(self.x) | hash(self.y)

class Layer:
    def __init__(self, name):
        self.name = name
        self.polygons = []
    
    def __repr__(self):
        return f"Layer(name={self.name}, polygons={self.polygons})"

class Marker:
    def __init__(self, name):
        self.name = name
        self.polygon = []  # marker 自己的 polygon
        self.layers = {}    # marker 下的所有 layer
    
    def __repr__(self):
        return f"Marker(name={self.name}, polygon={self.polygon}, layers={self.layers})"

class Pattern:
    def __init__(self, name):
        self.name = name
        self.markers = {}

    def __repr__(self):
        return f"Pattern(name={self.name}, markers={self.markers})"

# 解析坐标
def parse_point(point_str):
    x, y = re.findall(r'-?\d+', point_str)
    return Point(x, y)

def normalize_poly(current_polygon):
    # 将polygon统一转化为左下角为起始点，逆时针表示
    left_bottom_point = min(current_polygon, key=lambda p: (p.x, p.y))
    idx = current_polygon.index((left_bottom_point))
    # print('idx:', idx)
    current_polygon = current_polygon[idx:] + current_polygon[:idx]

    return current_polygon

# 解析文件
def parse_file(filename):
    patterns = defaultdict(Pattern)
    current_pattern = None
    current_marker = None
    current_layer = None

    with open(filename, 'r') as file:
        for line in file:
            line = line.strip()
            if not line:
                continue

            if line.startswith("pattern"):
                # 解析新的 pattern
                pattern_name = line.strip(':')
                current_pattern = Pattern(pattern_name)
                patterns[pattern_name] = current_pattern
                current_marker = None
                current_layer = None

            elif line.startswith("marker"):
                # 解析新的 marker
                marker_name = line.strip(':')
                current_marker = Marker(marker_name)
                current_layer = None  # 新 marker 开始，清空当前 layer

            elif line.startswith("layer"):
                # 解析新的 layer
                layer_name = line.strip(':')
                current_layer = Layer(layer_name)
                current_marker.layers[layer_name] = current_layer

            else:
                # 解析坐标并决定当前作用域 (marker 的 polygon 或 layer 的 polygon)
                points = re.findall(r'\([^\)]+\)', line)
                polygon = [parse_point(pt) for pt in points]
                polygon = normalize_poly(polygon)
                if current_layer:
                    current_layer.polygons.append(polygon)  # 将多边形添加到 layer 中
                elif current_marker:
                    current_marker.polygon = polygon  # 将多边形添加到 marker 中
                    current_pattern.markers[tuple(polygon)] = current_marker

    return patterns

# 比较两个文件的差异
def compare_patterns(patterns1, patterns2, according_file = 'ans'):
    flag = True
    
    for pattern_name, pattern1 in patterns1.items():
        if pattern_name not in patterns2:
            print(f"Pattern {pattern_name} is missing in the {according_file} file.")
            flag = False
            continue

        pattern2 = patterns2[pattern_name]

        # 比较 markers
        for marker_name, marker1 in pattern1.markers.items():
            if marker_name not in pattern2.markers:
                print(f"Marker {marker1.name} {marker_name} in pattern {pattern_name} is missing in the {according_file} file.")
                flag = False
                continue

            marker2 = pattern2.markers[marker_name]

            # 比较 marker 自身的 polygon
            poly1 = marker1.polygon
            poly2 = marker2.polygon
            if poly1 != poly2:
                flag = False
                print(f"Polygon mismatch in marker {marker_name} of pattern {pattern_name}.")
                print(f"res polygon: {marker1.polygon}")
                print(f"ans polygon: {marker2.polygon}")

            # 比较 layers
            for layer_name, layer1 in marker1.layers.items():
                if layer_name not in marker2.layers:
                    flag = False
                    print(f"Layer {layer_name} in marker {marker_name} of pattern {pattern_name} is missing in the {according_file} file.")
                    continue

                layer2 = marker2.layers[layer_name]
                if len(layer1.polygons) != len(layer2.polygons):
                    flag = False
                    print(f"Polygon count mismatch in layer {layer_name} of marker {marker_name} in pattern {pattern_name}.")
                else:
                    layer1.polygons.sort(key=lambda poly: [point for point in poly])
                    layer2.polygons.sort(key=lambda poly: [point for point in poly])
                    for poly1, poly2 in zip(layer1.polygons, layer2.polygons):
                        if poly1 != poly2:
                            flag = False
                            print(f"Polygon mismatch in layer {layer_name} of marker {marker_name} in pattern {pattern_name}.")
                            print(f"res polygon: {poly1}")
                            print(f"ans polygon: {poly2}")
    return flag
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-res', type=str, help='specify the result file.')
    parser.add_argument('-ans', type=str, help='specify the answer file.')
    args = parser.parse_args()

    # 调用pmJudge函数对比两个文件中的多边形列表
    patterns1 = parse_file(args.res)
    patterns2 = parse_file(args.ans)
    # 比较两个文件
    if compare_patterns(patterns1, patterns2) and compare_patterns(patterns2,patterns1, 'res'):
        print("PASSED.")

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

struct Point {
    int x;
    int y;
};

// 从字符串中读取坐标点并存储到 vector<Point> 中
void readCoordinatesFromString(const std::string& input, std::vector<Point>& points) {
    std::stringstream ss(input);
    std::string coord;

    // 按照','分割每个坐标
    while (std::getline(ss, coord, ')')) {
        // 找到'('和')'并提取坐标
        size_t start = coord.find('(');
        size_t end = coord.size();

        if (start != std::string::npos && end != std::string::npos) {
            int x, y;
            // 解析坐标点
            sscanf(coord.substr(start + 1, end - start - 1).c_str(), "%d,%d", &x, &y);
            points.push_back({x, y});
        }
    }
}

int main() {
    std::vector<Point> points;

    // 假设输入的字符串为以下内容
    std::string input = "(-673768,503732),(-667768,503732),(-667768,512132),(-673768,512132),"
                        "(-673768,510932),(-670168,510932),(-670168,504932),(-673768,504932)";

    // 从字符串读取坐标点
    readCoordinatesFromString(input, points);

    // 输出读取的坐标点以验证
    for (const auto& point : points) {
        std::cout << "Point(x=" << point.x << ", y=" << point.y << ")\n";
    }

    return 0;
}

#include <iostream>
#include <unordered_map>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>
#include <map>
#include <algorithm>
#include <memory>

using namespace std;

/*Data storage type1: 
    storage CornerPoints

    good points: 
        - no need to cumpute when storaging.
        - no need to check if the point is in the polygon.
    
    bad points:
        - need to compute relative distance when shearching.
        - hard to check the edge of marker.
*/

struct Point {
    int x = 0;
    int y = 0;
};

struct Polygon {
    vector<Point> points;
};

/*
Data Storage type2:
    storage the coordinate of the leftest point of the top line and the length and direction of edges

    good points:
        - all good points of type1.
        - easy to search the rotated and mirrored type.
        - easy to match when searching.

    bad points:
        - need to compute every relative distance when storaging.
        - hard to check the edge of marker.
*/

/*
Direction Description:
0 as up
1 as right
2 as down
3 as left
*/

struct edge {
    int length;
    int direction;
};

struct Manhatten {
    //the leftest point of the top line
    int x;
    int y;

    int up;
    int down;
    int left;
    int right;

    vector<edge> edges;
    vector<Point> points; //the points of the polygon

    map<int, int> mEdgeLength;
    int incornerCount;
    int outcornerCount;
};

struct pointInTree {
    int x;
    int y;
    int location;
    int index;

    pointInTree() : x(-1), y(-1), location(0) {}
};

struct KdTree {
    pointInTree root = pointInTree();
    KdTree* parent;
    KdTree* leftChild;
    KdTree* rightChild;
    int dimension;

    KdTree() : parent(NULL), leftChild(NULL), rightChild(NULL), dimension(-1) {}
    bool isEmpty()  {
        return root.x == -1 && root.y == -1;
    }
    bool isLeaf()  {
        return (!isEmpty()) && leftChild == NULL && rightChild == NULL;
    }
    bool isRoot()  {
        return (!isEmpty()) && parent == NULL;
    }
    bool isLeft()  {
        return parent != NULL && parent->leftChild == this;
    }
    bool isRight()  {
        return parent != NULL && parent->rightChild == this;
    }
};

struct layer {
    vector<Manhatten> Manhattens;
    bool marked;
    Manhatten markManhatten;
    int layerNum = 0; //the layer number
    vector<pointInTree> pointsInTree;
    KdTree tree1 = KdTree();
    KdTree* tree = &tree1;
};

struct marker {
    int up;
    int down;
    int left;
    int right;
};

struct Pattern {
    vector<layer> layers;
    marker mark;
    int patternNum = 0;
};

struct patternMap {
    vector<Pattern> patterns;
};

struct potentialMatchingArea {
    int up;
    int down;
    int left;
    int right;

    int rotationMod;

    map <int, bool> matchLayer;
    int matchNum = 0;

    vector<pointInTree> corners;
    map<int, int> ManhattenInArea;
};

struct patternMatchResult {
    vector<potentialMatchingArea> potentialMatchingAreas;
};

struct layout {
    vector <layer> layers;
};

class baseOps {
    public:

    bool checkPointInMarker (Point p, marker mark) {
        if (p.x >= mark.left && p.x <= mark.right && p.y <= mark.up && p.y >= mark.down) {
            return true;
        } else {
            return false;
        }
    }

    bool checkManhattenInMarker (Manhatten ManHT, marker mark) {
        if (((ManHT.left < mark.right) || (ManHT.right > mark.left)) && ((ManHT.up > mark.down) || (ManHT.down < mark.up))) {
            return true;
        } else {
            return false;
        }
    }

    bool checkManhattenAllInMarker (Manhatten ManHT, marker mark) {
        if (ManHT.up < mark.up && ManHT.down > mark.down && ManHT.left > mark.left && ManHT.right < mark.right) {
            return true;
        } else {
            return false;
        }
    }

    Manhatten fixManhatten (Manhatten ManHT, int mod) {
        int maxY = ManHT.y;
        int minY = ManHT.y;
        int minX = ManHT.x;
        int maxX = ManHT.x;
        int currentX = ManHT.x;
        int currentY = ManHT.y;
        int index;

        Point currentPoint = {ManHT.x, ManHT.y};
        Point nextPoint = currentPoint;

        switch (mod) {
            case 0: {
                for (int i = 0; i <ManHT.edges.size(); i++) {
                    edge currentEdge = ManHT.edges[i % ManHT.edges.size()];
                    switch (currentEdge.direction) {
                        case 0: nextPoint.y += currentEdge.length; break;
                        case 1: nextPoint.x += currentEdge.length; break;
                        case 2: nextPoint.y -= currentEdge.length; break;
                        case 3: nextPoint.x -= currentEdge.length; break;
                        default: break;
                    }

                    if (nextPoint.y < minY) {
                        minY = nextPoint.y;
                        currentY = minY;
                        currentX = nextPoint.x;
                        index = i + 1;
                    } else if (nextPoint.y == minY) {
                        if (nextPoint.x > currentX) {
                            currentX = nextPoint.x;
                            index = i + 1;
                        }
                    } else if (nextPoint.y > maxY) {
                        maxY = nextPoint.y;
                    }

                    if (nextPoint.x > maxX) {
                        maxX = nextPoint.x;
                    } else if (nextPoint.x < minX) {
                        minX = nextPoint.x;
                    }
                }

                break;
            }

            case 1: {
                for (int i = 0; i <ManHT.edges.size(); i++) {
                    edge currentEdge = ManHT.edges[i % ManHT.edges.size()];
                    switch (currentEdge.direction) {
                        case 0: nextPoint.y += currentEdge.length; break;
                        case 1: nextPoint.x += currentEdge.length; break;
                        case 2: nextPoint.y -= currentEdge.length; break;
                        case 3: nextPoint.x -= currentEdge.length; break;
                        default: break;
                    }

                    if (nextPoint.x < minX) {
                        minX = nextPoint.x;
                        currentX = minX;
                        currentY = nextPoint.y;
                        index = i + 1;
                    } else if (nextPoint.x == minX) {
                        if (nextPoint.y < currentY) {
                            currentY = nextPoint.y;
                            index = i + 1;
                        }
                    } else if (nextPoint.x > maxX) {
                        maxX = nextPoint.x;
                    }

                    if (nextPoint.y > maxY) {
                        maxY = nextPoint.y;
                    } else if (nextPoint.y < minY) {
                        minY = nextPoint.y;
                    }
                }

                break;
            }

            case 2: {
                for (int i = 0; i < ManHT.edges.size(); i++) {
                    edge currentEdge = ManHT.edges[i % ManHT.edges.size()];
                    switch (currentEdge.direction) {
                        case 0: nextPoint.y += currentEdge.length; break;
                        case 1: nextPoint.x += currentEdge.length; break;
                        case 2: nextPoint.y -= currentEdge.length; break;
                        case 3: nextPoint.x -= currentEdge.length; break;
                        default: break;
                    }
                    
                    if (nextPoint.y > maxY) {
                        maxY = nextPoint.y;
                        currentY = maxY;
                        currentX = nextPoint.x;
                        index = i + 1;
                    } else if (nextPoint.y == maxY) {
                        if (nextPoint.x < currentX) {
                            currentX = nextPoint.x;
                            index = i + 1;
                        }
                    } else if (nextPoint.y < minY) {
                        minY = nextPoint.y;
                    }

                    if (nextPoint.x > maxX) {
                        maxX = nextPoint.x;
                    } else if (nextPoint.x < minX) {
                        minX = nextPoint.x;
                    }
                }

                break;
            }

            case 3: {
                for (int i = 0; i < ManHT.edges.size(); i++) {
                    edge currentEdge = ManHT.edges[i % ManHT.edges.size()];
                    switch (currentEdge.direction) {
                        case 0: nextPoint.y += currentEdge.length; break;
                        case 1: nextPoint.x += currentEdge.length; break;
                        case 2: nextPoint.y -= currentEdge.length; break;
                        case 3: nextPoint.x -= currentEdge.length; break;
                        default: break;
                    }
                    
                    if (nextPoint.x > maxX) {
                        maxX = nextPoint.x;
                        currentX = maxX;
                        currentY = nextPoint.y;
                        index = i + 1;
                    } else if (nextPoint.x == maxX) {
                        if (nextPoint.y > currentY) {
                            currentY = nextPoint.y;
                            index = i + 1;
                        }
                    } else if (nextPoint.x < minX) {
                        minX = nextPoint.x;
                    }

                    if (nextPoint.y > maxY) {
                        maxY = nextPoint.y;
                    } else if (nextPoint.y < minY) {
                        minY = nextPoint.y;
                    }
                }

                break;
            }

            default: {
                cerr << "Warning: mod set wrong" << endl;
                break;
            }
        }

        ManHT.x = currentX;
        ManHT.y = currentY;
        ManHT.up = maxY;
        ManHT.down = minY;
        ManHT.left = minX;
        ManHT.right = maxX;
        ManHT.incornerCount = 0;
        ManHT.outcornerCount = 0;

        vector<edge> newEdges;
        vector<Point> newPoints;
        map<int, int> newMedgeLength;
        for (int i = index; i < ManHT.edges.size() + index; i++) {
            newPoints.push_back(ManHT.points[i%ManHT.edges.size()]);
            newEdges.push_back(ManHT.edges[i%ManHT.edges.size()]);
            if (directionOperetor(ManHT.edges[i % ManHT.edges.size()].direction, ManHT.edges[(i+1) % ManHT.edges.size()].direction, '-') == 1) {
                ManHT.incornerCount ++;
            } else {
                ManHT.outcornerCount ++;
            }
            newMedgeLength[ManHT.edges[i%ManHT.edges.size()].length] ++;
        }
        ManHT.edges = newEdges;
        ManHT.points = newPoints;
        ManHT.x = ManHT.points[0].x;
        ManHT.y = ManHT.points[0].y;
        ManHT.mEdgeLength = newMedgeLength;

        return ManHT;
    }

    int directionOperetor (int dir1, int dir2, char op) {
        switch (op) {
            case '-' : {
                switch (dir1 - dir2) {
                    case -3 : return -1;
                    case -2 : return 2;
                    case -1 :return 1;
                    case 0 : return 0;
                    case 1 : return -1;
                    case 2 : return 2;
                    case 3 : return 1;
                    default: return 3; //error
                }
            }

            case '+' : {
                return (dir1 + dir2) % 4;
            }
        }
        return 4; //error
    }

    Manhatten mirrorManhatten (Manhatten ManHT, marker mark) {
        Manhatten newManHT;
        newManHT.x = mark.right - ManHT.x + mark.left;
        newManHT.y = ManHT.y;
        edge newEdge;
        edge oldEdge;
        Point newPoint;
        Point oldPoint;
        vector<edge> newEdges;
        vector<Point> newPoints;
        newManHT.down = ManHT.down;
        newManHT.up = ManHT.up;
        newManHT.left = mark.right + mark.left - ManHT.right;
        newManHT.right = mark.right + mark.left - ManHT.left;
        newManHT.incornerCount = ManHT.incornerCount;
        newManHT.outcornerCount = ManHT.outcornerCount;
        newManHT.mEdgeLength = ManHT.mEdgeLength;

        for (int i = ManHT.points.size(); i > 0; i--) {
            oldPoint = ManHT.points[i % ManHT.points.size()];
            newPoint.y = oldPoint.y;
            newPoint.x = mark.right + mark.left - oldPoint.x;
            newPoints.push_back(newPoint);
        }

        for (int i = ManHT.edges.size() - 1; i >= 0; i--) {
            oldEdge = ManHT.edges[i];
            newEdge.length = oldEdge.length;
            if (oldEdge.direction % 2 == 1) {
                newEdge.direction = oldEdge.direction;
            } else {
                newEdge.direction = directionOperetor(oldEdge.direction, 2, '+');
            }

            newEdges.push_back(newEdge);
        }
        newManHT.points = newPoints;
        newManHT.edges = newEdges;
        Manhatten newManHT1 = fixManhatten(newManHT, 2);
        return newManHT1;
    }

    patternMap mirrorPatternMap (patternMap old) {
        vector <Pattern> newPatternMap;
        for (auto oldPattern : old.patterns) {
            Pattern newPattern;
            newPattern.mark = oldPattern.mark;
            newPattern.patternNum = oldPattern.patternNum;
            vector <layer> newLayers;
            for (auto oldLayer : oldPattern.layers) {
                layer newLayer;
                newLayer.layerNum = oldLayer.layerNum;
                newLayer.marked = oldLayer.marked;
                if (newLayer.marked) newLayer.markManhatten = mirrorManhatten(oldLayer.markManhatten, oldPattern.mark);
                vector<Manhatten> newManHTs;
                for (auto oldManHT : oldLayer.Manhattens) {
                    Manhatten newManHT = mirrorManhatten(oldManHT, oldPattern.mark);
                    newManHTs.push_back(newManHT);
                }
                newLayer.Manhattens = newManHTs;
                newLayers.push_back(newLayer);
            }
            newPattern.layers = newLayers;
            newPatternMap.push_back(newPattern);
        }
        return {newPatternMap};
    }

    vector<pointInTree> Transpose (vector<pointInTree> data) {
        vector<pointInTree> newData;
        pointInTree newPoint;
        for (auto point : data) {
            newPoint = point;
            newPoint.x = point.y;
            newPoint.y = point.x;
            newData.push_back(newPoint);
        }
        return newData;
    }

    int findMiddlePoint (vector<pointInTree> data, int mod) {
        map<int, int> xMap;
        for (auto point : data) {
            if (mod == 0) xMap[point.x] ++;
            else if (mod == 1) xMap[point.y] ++;
        }
        int num = 0;
        for (auto pair : xMap) {
            num += xMap[pair.first];
            if (num >= data.size() / 2) {
                return pair.first;
            }
        }
        return -1;
    }

    void buildKdTree (KdTree* tree, vector<pointInTree> data, int depth) {
        tree -> root = pointInTree();
        unsigned num = data.size();
        if (num == 0) {
            return ;
        } else if (num == 1) {
            tree -> root = data[0];
            tree -> dimension = 0;
            return ;
        }
        vector<pointInTree> transData = Transpose(data);
        int splitAttribute = depth % 2;
        int splitPoint = findMiddlePoint (data, splitAttribute);

        vector<pointInTree> subset1;
        vector<pointInTree> subset2;

        switch (splitAttribute) {
            case 0: {
                for (int i = 0; i < num; i++) {
                    if (data[i].x == splitPoint && tree->isEmpty()) {
                        tree -> root = data[i];
                        tree -> dimension = 0;
                    } else {
                        if (data[i].x < splitPoint) {
                            subset1.push_back(data[i]);
                        } else {
                            subset2.push_back(data[i]);
                        }
                    }
                }

                break;
            }
            case 1: {
                for (int i = 0; i < num; i++) {
                    if (data[i].y == splitPoint && tree->isEmpty()) {
                        tree -> root = data[i];
                        tree -> dimension = 0;
                    } else {
                        if (data[i].y < splitPoint) {
                            subset1.push_back(data[i]);
                        } else {
                            subset2.push_back(data[i]);
                        }
                    }
                }

                break;
            }
            default: break;
        }
        tree -> leftChild = new KdTree;
        tree -> leftChild -> parent = tree;
        tree -> rightChild = new KdTree;
        tree -> rightChild -> parent = tree;
        buildKdTree(tree -> leftChild, subset1, depth + 1);
        buildKdTree(tree -> rightChild, subset2, depth + 1);
    }

    double measureDistance (pointInTree point1, pointInTree point2, unsigned methed) {
        switch (methed) {
            case 0: {
                double res = 0;
                return sqrt(pow((point1.x - point2.x), 2) + pow((point1.y - point2.y), 2));
                break;
            }
            case 1: {
                double es = 0;
                return abs(point1.x - point2.x) + abs(point1.y - point2.y);
            }
            default: return -1;
        }
    }
};

class Read : baseOps {
    public:

    vector<pointInTree> generatePOintsInTree (layer inLayer) {
        pointInTree currentPoint;
        vector<pointInTree> pointsInTree;
        int i = 0;
        for (auto ManHT : inLayer.Manhattens) {
            currentPoint.x = ManHT.left;
            currentPoint.y = ManHT.up;
            currentPoint.location = 0;
            Manhatten* fatherManHT = &ManHT;
            currentPoint.index = i;
            pointsInTree.push_back(currentPoint);

            currentPoint.x = ManHT.right;
            currentPoint.location = 1;
            pointsInTree.push_back(currentPoint);

            currentPoint.y = ManHT.down;
            currentPoint.location = 2;
            pointsInTree.push_back(currentPoint);

            currentPoint.x = ManHT.left;
            currentPoint.location = 3;
            pointsInTree.push_back(currentPoint);
            i++;
        }
        return pointsInTree;
    }

    layer findMarkManhatten (layer inLayer, marker mark) {
        Manhatten markManHT;
        bool marked = false;
        int maxPointNum = 0;
        int state = -1;

        /*state decription:
        -1: didn't find markManhatten
        1:  hane found markManhatten*/

        for (Manhatten m : inLayer.Manhattens) {
            if (checkManhattenAllInMarker(m, mark)) {
                if (m.points.size() > maxPointNum) {
                    maxPointNum = m.points.size();
                    markManHT = m;
                    marked = true;
                    state = 1;
                }
            }
        }

        inLayer.markManhatten = markManHT;
        inLayer.marked = marked;

        return inLayer;
    }

    Pattern addMarkManhatten (Pattern inPattern) {
        vector<layer> newLayers;
        if (inPattern.layers.size() == 0) {
            cerr << "Error: no layer in pattern" << endl;
        }

        for (layer l : inPattern.layers) {
            l = findMarkManhatten(l, inPattern.mark);
            newLayers.push_back(l);
        }

        inPattern.layers = newLayers;

        return inPattern;
    }
    
    void readCoordinatesFromString(const std::string& input, std::vector<Point>& points) {
        std::stringstream ss(input);
        std::string coord;

        // 按照')'分割每个坐标
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

    Manhatten readEdge (Polygon Pol)  {
        Manhatten ManHT;
        int maxY = Pol.points[0].y;
        int minY = Pol.points[0].y;
        int maxX = Pol.points[0].x;
        int minX = Pol.points[0].x;
        int currentX = Pol.points[0].x;
        int index = 0;

        if (Pol.points.size() < 3) {
            cout << "Error: Not a polygon" << endl;
            return ManHT;
        }

        for (int i = 0; i < Pol.points.size(); i++) {
            Point p1 = Pol.points[i];

            if (p1.y > maxY) {
                maxY = p1.y;
                currentX = p1.x;
            } else if (p1.y == maxY) {
                if (p1.x < currentX) {
                    currentX = p1.x;
                    index = i;
                }
            } else if (p1.y < minY) {
                minY = p1.y;
            }

            if (p1.x > maxX) {
                maxX = p1.x;
            } else if (p1.x < minX) {
                minX = p1.x;
            }
        }

        ManHT.x = Pol.points[index].x;
        ManHT.y = Pol.points[index].y;
        ManHT.up = maxY;
        ManHT.down = minY;
        ManHT.left = minX;
        ManHT.right = maxX;
        ManHT.incornerCount = 0;
        ManHT.outcornerCount = 0;

        for (int i = index; i < Pol.points.size() + index; i++) {
            Point p1 = Pol.points[i % Pol.points.size()];
            Point p2 = Pol.points[(i + 1) % Pol.points.size()];
            ManHT.points.push_back(p1);

            if (p1.x == p2.x) {
                if (p1.y > p2.y) {
                    ManHT.edges.push_back({p1.y - p2.y, 2});
                    ManHT.mEdgeLength[p1.y - p2.y] ++;
                } else if (p2.y > p1.y) {
                    ManHT.edges.push_back({p2.y - p1.y, 0});
                    ManHT.mEdgeLength[p2.y - p1.y] ++;
                } else cout << "Error: Same point" << endl;
            } else if (p1.y == p2.y) {
                if (p1.x > p2.x) {
                    ManHT.edges.push_back({p1.x - p2.x, 3});
                    ManHT.mEdgeLength[p1.x - p2.x] ++;
                } else if (p2.x > p1.x) {
                    ManHT.edges.push_back({p2.x - p1.x, 1});
                    ManHT.mEdgeLength[p2.x - p1.x] ++;
                } else cout << "Error: Same point" << endl;
            } else cout << "Error: Not a Manhattan Plot" << endl;
        }

        for (int i = 0;i < ManHT.edges.size(); i++) {
            if (directionOperetor(ManHT.edges[i].direction, ManHT.edges[(i+1) % ManHT.edges.size()].direction, '-') == 1) {
                ManHT.incornerCount ++;
            } else {
                ManHT.outcornerCount ++;
            }
        }

        return ManHT;
    }

    patternMap readPattern(const string& filename) {
        vector<Polygon> patterns;
        ifstream file(filename);
        string line;
        vector<Point> currentPolygon;
        Manhatten ManHT;
        vector<Manhatten> currentLayer;
        vector<layer> currentPattern;
        vector<Pattern> currentMap;
        marker currentMarker;
        Manhatten currentMarkerManHT;
        patternMap ansMap;
        int layerNum = 1;
        int patternNum = 1;
        bool marked = false;


        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return ansMap;
        }
        
        while (getline(file, line)) {
            istringstream iss(line);
            string firstWord;
            iss >> firstWord;

            if (firstWord.find("pattern") == 0) {
                if (!currentLayer.empty()) {
                    layer nowLayer;
                    nowLayer.Manhattens = currentLayer;
                    nowLayer.layerNum = layerNum;
                    nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
                    buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
                    currentPattern.push_back(nowLayer);
                    layerNum ++;
                    currentLayer.clear();
                }

                if (!currentPattern.empty()) {
                    Pattern nowPattern;
                    nowPattern.patternNum = patternNum;
                    nowPattern.layers = currentPattern;
                    nowPattern.mark = currentMarker;
                    nowPattern = addMarkManhatten(nowPattern);
                    currentMap.push_back(nowPattern);
                    patternNum ++;
                    layerNum = 1;
                    currentPattern.clear();
                }
            } else if (firstWord.find("layer") == 0) {
                // if a new layer starts, push the current layer if it's not empty
                if (!currentLayer.empty()) {
                    layer nowLayer;
                    nowLayer.Manhattens = currentLayer;
                    nowLayer.layerNum = layerNum;
                    nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
                    buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
                    currentPattern.push_back(nowLayer);
                    layerNum ++;
                    currentLayer.clear();
                }
            } else if (firstWord.find("marker") == 0) {
                marked = true;
            } else if (marked) {
                readCoordinatesFromString(line, currentPolygon);

                currentMarker.down = currentMarker.up = currentPolygon[0].y;
                currentMarker.left = currentMarker.right = currentPolygon[0].x;

                for (Point p : currentPolygon) {
                    if (p.y < currentMarker.down) {
                        currentMarker.down = p.y;
                    } else if (p.y > currentMarker.up) {
                        currentMarker.up = p.y;
                    }

                    if (p.x < currentMarker.left) {
                        currentMarker.left = p.x;
                    } else if (p.x > currentMarker.right) {
                        currentMarker.right = p.x;
                    }
                }

                marked = false;
            } else {
                readCoordinatesFromString(line, currentPolygon);
                ManHT = readEdge({currentPolygon});
                currentPolygon.clear();
                currentLayer.push_back(ManHT);
            }
        }

        if (!currentLayer.empty()) {
            layer nowLayer;
            nowLayer.Manhattens = currentLayer;
            nowLayer.layerNum = layerNum;
            nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
            buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
            currentPattern.push_back(nowLayer);
            layerNum ++;
            currentLayer.clear();
        }

        if (!currentPattern.empty()) {
            Pattern nowPattern;
            nowPattern.patternNum = patternNum;
            nowPattern.layers = currentPattern;
            nowPattern.mark = currentMarker;
            nowPattern = addMarkManhatten(nowPattern);
            currentMap.push_back(nowPattern);
        }

        if (currentMap.empty()) {
            cerr << "Error: No patterns found" << endl;
            return ansMap;
        }

        ansMap.patterns = currentMap;

        return ansMap;
    }

    layout readLayout (const string& filename) {
        ifstream file(filename);
        string line;
        vector<Point> currentPolygon;
        Manhatten ManHT;
        vector<Manhatten> currentLayer;
        vector<layer> currentLayout;
        layout result;
        int layerNum = 1;

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return result;
        }

        while (getline(file, line)) {
            istringstream iss(line);
            string firstWord;
            iss >> firstWord;

            if (firstWord.find("layer") == 0) {
                if (!currentLayer.empty()) {

                    if (ManHT.edges.size() != 0) {
                        currentLayer.push_back(ManHT);
                    }
                    layer nowLayer;
                    nowLayer.Manhattens = currentLayer;
                    nowLayer.layerNum = layerNum;
                    nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
                    buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
                    currentLayout.push_back(nowLayer);
                    layerNum ++;
                    currentLayer.clear();
                }
            } else {
                readCoordinatesFromString(line, currentPolygon);
                ManHT = readEdge({currentPolygon});
                currentPolygon.clear();
                currentLayer.push_back(ManHT);
            }
        }

        if (ManHT.edges.size() != 0) {
            currentLayer.push_back(ManHT);
        }

        if (!currentLayer.empty()) {
            layer nowLayer;
            nowLayer.Manhattens = currentLayer;
            nowLayer.layerNum = layerNum;
            nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
            buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
            currentLayout.push_back(nowLayer);
            layerNum ++;
            currentLayer.clear();
        }

        result.layers = currentLayout;

        return result;
    }
};

class FuzzyMatching : baseOps {
    public:

    /*matchByBox return number description
    0: not match
    1: cpmplately match
    2: rotatedly match*/
    int matchByBox (Manhatten ManHT1, Manhatten ManHT2) {
        if (((ManHT1.up - ManHT1.down) == (ManHT2.up - ManHT2.down)) && ((ManHT1.right - ManHT1.left) == (ManHT2.right - ManHT2.left))) {
            return 1;
        } else if (((ManHT1.right - ManHT1.left) == (ManHT2.up - ManHT2.down)) && ((ManHT1.up - ManHT1.down) == (ManHT2.right - ManHT2.left))) {
            return 2;
        } else {
            return 0;
        }
    }

    bool matchByPointNum (Manhatten ManHT1, Manhatten ManHT2) {
        if (ManHT1.edges.size() != ManHT2.edges.size()) {
            return false;
        }

        if (ManHT1.incornerCount == ManHT2.incornerCount) {
            return true;
        } else {
            return false;
        }
    }

    bool matchByMedges (Manhatten ManHT1, Manhatten ManHT2) {
        if (ManHT1.edges.size() != ManHT2.edges.size()) {
            return false;
        }

        for (auto pair : ManHT1.mEdgeLength) {
            if (ManHT2.mEdgeLength[pair.first] != pair.second) {
                return false;
            }
        }

        return true;
    }

    int turnBetweenEdges (Manhatten ManHT, int i) {
        return directionOperetor(ManHT.edges[i%ManHT.edges.size()].direction, ManHT.edges[(i+1)%ManHT.edges.size()].direction, '-');
    }

    int matchByEdges (Manhatten ManHT1, Manhatten ManHT2) {
        int n = ManHT1.edges.size();
        edge firstEdge;
        firstEdge.length = 0;
        for (int i = 0; i < n; i++) {
            if (ManHT2.edges[i].length == ManHT1.edges[0].length) {
                for (int j = i; j < i+n-1; j++) {
                    if ((ManHT2.edges[j%n].length != ManHT1.edges[(j-i)%n].length) || (turnBetweenEdges(ManHT2, j) != turnBetweenEdges(ManHT1, j-i))) {
                        break;
                    }
                    firstEdge = ManHT2.edges[i];
                }
            }
        }

        if (firstEdge.length ==0) return -1; //not match

        return firstEdge.direction;
    }

    int ManhattenMatch (Manhatten ManHT1, Manhatten ManHT2) {
        if (matchByBox(ManHT1, ManHT2) == 0) {
            return -1;
        }

        if (!matchByPointNum(ManHT1, ManHT2)) {
            return -1;
        }

        if (!matchByMedges(ManHT1, ManHT2)) {
            return -1;
        }

        return matchByEdges(ManHT1, ManHT2);
    }

    int potentialAreaMatch(const potentialMatchingArea& Area, const patternMatchResult& result) {
        int i = 0;
        for (const auto& item : result.potentialMatchingAreas) {
            if (item.down == Area.down && item.up == Area.up && 
                item.left == Area.left && item.right == Area.right) {
                return i;
            }
            i++;
        }
        return i; // 返回一个默认构造的实例，或者抛出一个异常
    }

    patternMatchResult findPotentialArea (layout Layout, Pattern pattern) {
        patternMatchResult result;
        if (Layout.layers.size() == 0) {
            cerr << "Layout is empty when finding potential area" << endl;
            return result;
        }

        if (pattern.layers.size() == 0) {
            cerr << "Pattern is empty when finding potential area" << endl;
            return result;
        }
        layer layerInPattern;
        for (auto layerInLayout : Layout.layers) {
            for (auto item : pattern.layers) {
                if (item.layerNum == layerInLayout.layerNum) {
                    layerInPattern = item;
                    break;
                }
            }

            if (layerInPattern.Manhattens.size() == 0) {
                continue;
            }

            if (!layerInPattern.marked) {
                continue;
            }

            for (auto ManhattenInLayout : layerInLayout.Manhattens) {
                int matchMod = ManhattenMatch(ManhattenInLayout, layerInPattern.markManhatten);
                switch (matchMod) {
                    case -1: {
                        continue;
                        break;
                    }

                    case 0: {
                        potentialMatchingArea Area1;
                        Area1.up = ManhattenInLayout.up + layerInPattern.markManhatten.down - pattern.mark.down;
                        Area1.down = ManhattenInLayout.down - pattern.mark.up + layerInPattern.markManhatten.up;
                        Area1.left = ManhattenInLayout.left - pattern.mark.right + layerInPattern.markManhatten.right;
                        Area1.right = ManhattenInLayout.right + layerInPattern.markManhatten.left - pattern.mark.left;
                        Area1.matchLayer[layerInPattern.layerNum] = true;
                        Area1.rotationMod = matchMod;

                        if (potentialAreaMatch(Area1, result) >= result.potentialMatchingAreas.size()) {
                            Area1.matchNum  = 1;
                            result.potentialMatchingAreas.push_back(Area1);
                        } else {
                            if (result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] == false){
                                result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchNum ++;
                            }
                            result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] = true;
                        }

                        break;
                    }

                    case 1: {
                        potentialMatchingArea Area1;
                        Area1.up = ManhattenInLayout.up - layerInPattern.markManhatten.right + pattern.mark.right;
                        Area1.down = ManhattenInLayout.down + pattern.mark.left - layerInPattern.markManhatten.left;
                        Area1.left = ManhattenInLayout.left + pattern.mark.up - layerInPattern.markManhatten.up;
                        Area1.right = ManhattenInLayout.right - layerInPattern.markManhatten.down + pattern.mark.down;
                        Area1.matchLayer[layerInPattern.layerNum] = true;
                        Area1.rotationMod = matchMod;

                        if (potentialAreaMatch(Area1, result) >= result.potentialMatchingAreas.size()) {
                            Area1.matchNum  = 1;
                            result.potentialMatchingAreas.push_back(Area1);
                        } else {
                            if (result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] == false){
                                result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchNum ++;
                            }
                            result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] = true;
                        }

                        break;
                    }

                    case 2: {
                        potentialMatchingArea Area1;
                        Area1.up = ManhattenInLayout.up - layerInPattern.markManhatten.up + pattern.mark.up;
                        Area1.down = ManhattenInLayout.down + pattern.mark.down - layerInPattern.markManhatten.down;
                        Area1.left = ManhattenInLayout.left + pattern.mark.left - layerInPattern.markManhatten.left;
                        Area1.right = ManhattenInLayout.right - layerInPattern.markManhatten.right + pattern.mark.right;
                        Area1.matchLayer[layerInPattern.layerNum] = true;
                        Area1.rotationMod = matchMod;

                        if (potentialAreaMatch(Area1, result) >= result.potentialMatchingAreas.size()) {
                            Area1.matchNum  = 1;
                            result.potentialMatchingAreas.push_back(Area1);
                        } else {
                            if (result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] == false){
                                result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchNum ++;
                            }
                            result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] = true;
                        }

                        break;
                    }

                    case 3: {
                        potentialMatchingArea Area1;
                        Area1.up = ManhattenInLayout.up + layerInPattern.markManhatten.left - pattern.mark.left;
                        Area1.down = ManhattenInLayout.down - pattern.mark.right + layerInPattern.markManhatten.right;
                        Area1.left = ManhattenInLayout.left - pattern.mark.down + layerInPattern.markManhatten.down;
                        Area1.right = ManhattenInLayout.right + layerInPattern.markManhatten.up - pattern.mark.up;
                        Area1.matchLayer[layerInPattern.layerNum] = true;
                        Area1.rotationMod = matchMod;

                        if (potentialAreaMatch(Area1, result) >= result.potentialMatchingAreas.size()) {
                            Area1.matchNum  = 1;
                            result.potentialMatchingAreas.push_back(Area1);
                        } else {
                            if (result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] == false){
                                result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchNum ++;
                            }
                            result.potentialMatchingAreas[potentialAreaMatch(Area1, result)].matchLayer[layerInPattern.layerNum] = true;
                        }

                        break;
                    }

                    default : break;
                }
            }
        }
        return result;
    }
};

class PreciseMatching : baseOps{
    public:

    KdTree* findPointInTree (KdTree* tree, pointInTree point, int depth) {
        if (tree->isEmpty()) {
            cout << "Tree is empty in finding point in tree" << endl;
            return nullptr; 
        }

        if (tree->root.x == point.x && tree->root.y == point.y) return tree;

        if (tree->isLeaf()) {
            cout << "point not find in tree" << endl;
            return nullptr; 
        }

        switch (depth % 2) {
            case 0: {
                if (point.x < tree->root.x) {
                    tree = tree->leftChild;
                } else {
                    tree = tree->rightChild;
                }
                break;
            }
            case 1: {
                if (point.y < tree->root.y) {
                    tree = tree->leftChild;
                } else {
                    tree = tree->rightChild;
                }
                break;
            }
            default: break;
        }

        return findPointInTree (tree, point, depth+1);
    }

    bool checkCornerInPotentialArea (pointInTree point, potentialMatchingArea area) {
        return point.x >= area.left && point.x <= area.right && point.y >= area.down && point.y <= area.up;
    }

    void KdTreeAreaSearch (KdTree* tree, potentialMatchingArea area, int depth, vector<pointInTree> result) {
        if (tree->isEmpty()) return ;
        int dimension = depth % 2;
        int flag;
        switch (dimension) {
            case 0: {
                if (tree -> root.x > area.right) {
                    flag = -1;
                } else if (tree -> root.x < area.left) {
                    flag = 1;
                } else flag = 0;
                break;
            }
            case 1: {
                if (tree -> root.y > area.up) {
                    flag = -1;
                } else if (tree -> root.y < area.down) {
                    flag = 1;
                } else flag = 0;
                break;
            }
            default: break;
        }
        if (flag < 0) {
            KdTreeAreaSearch(tree -> leftChild, area, depth+1, result);
        } else if (flag > 0) {
            KdTreeAreaSearch(tree -> rightChild, area, depth+1, result);
        } else {
            potentialMatchingArea leftArea, rightArea;
            switch (dimension) {
                case 0: {
                    leftArea.up = rightArea.up = area.up;
                    leftArea.down = rightArea.down = area.down;
                    leftArea.right = rightArea.left = tree -> root.x;
                    leftArea.left = area.left;
                    rightArea.right = area.right;
                    break;
                }
                case 1: {
                    leftArea.left = rightArea.left = area.left;
                    leftArea.right = rightArea.right = area.right;
                    leftArea.up = rightArea.down = tree -> root.y;
                    leftArea.down = area.down;
                    rightArea.up = area.up;
                    break;
                }
                default: break;
            }
            KdTreeAreaSearch(tree -> leftChild, leftArea, depth+1, result);
            KdTreeAreaSearch(tree -> rightChild, rightArea, depth+1, result);
            if (checkCornerInPotentialArea(tree->root, area)) {
                result.push_back(tree->root);
            }
        }
    }

    void addManhattensToPotentialArea (layer layer, potentialMatchingArea area) {
        if (area.matchLayer[layer.layerNum] != true) {
            return ;
        }

        if (layer.Manhattens.size() == 0) {
            return ;
        }

        KdTreeAreaSearch(&layer.tree1, area, layer.tree1.dimension, area.corners);

        for (auto points : area.corners) {
            area.ManhattenInArea[points.index] ++;
        }
    }

    vector<Manhatten> cutByMarker (Manhatten ManHT, marker mark) {
        Point currentPoint = {ManHT.x, ManHT.y};
        Point nextPoint = currentPoint;
        vector<edge> currentEdges;
        Manhatten ansMan = ManHT;
        vector<Manhatten> ansMans;

        if (checkManhattenInMarker(ManHT, mark))  {
            ansMans.push_back(ManHT);
            return ansMans;
        }
        int i = 0;
        while (!checkPointInMarker(currentPoint, mark) && i < ManHT.points.size()) {
            i++;
            currentPoint = ManHT.points[i];
        }

        if (i == ManHT.points.size()) {
            int cutEdgeNum = 0;
            vector<Point> inPoints;
            Point inPoint;
            vector<Point> outPoints;
            Point outPoint;
            edge cutEdge;
            vector<edge> cutEdges;
            vector<edge> ansEdges;
            for (int i = 0; i <= ManHT.points.size(); i++) {
                edge currentEdge = ManHT.edges[i];
                currentPoint = ManHT.points[i];
                nextPoint = ManHT.points[i+1];

                switch (currentEdge.direction) {
                    case 0: {
                        if (currentPoint.x > mark.left && currentPoint.x < mark.right) {
                            if (currentPoint.y < mark.down && nextPoint.y > mark.down) {
                                cutEdgeNum ++;
                                inPoint.x = currentPoint.x;
                                inPoint.y = mark.down;
                                inPoints.push_back(inPoint);

                                outPoint.x = currentPoint.x;
                                outPoint.y = mark.up;
                                outPoints.push_back(outPoint);

                                cutEdge.direction = currentEdge.direction;
                                cutEdge.length = mark.up - mark.down;
                                cutEdges.push_back(cutEdge);
                            }
                        }

                        break;
                    }

                    case 1: {
                        if (currentPoint.y > mark.down && currentPoint.y < mark.up) {
                            if (currentPoint.x < mark.left && nextPoint.x > mark.right) {
                                cutEdgeNum ++;
                                inPoint.x = mark.left;
                                inPoint.y = currentPoint.y;
                                inPoints.push_back(inPoint);

                                outPoint.x = mark.right;
                                outPoint.y = currentPoint.y;
                                outPoints.push_back(outPoint);

                                cutEdge.direction = currentEdge.direction;
                                cutEdge.length = mark.right - mark.left;
                                cutEdges.push_back(cutEdge);
                            }
                        } 

                        break;
                    }

                    case 2: {
                        if (currentPoint.x < mark.right && currentPoint.x > mark.left) {
                            if (currentPoint.y > mark.up && nextPoint.y < mark.down) {
                                cutEdgeNum ++;
                                inPoint.x = currentPoint.x;
                                inPoint.y = mark.up;
                                inPoints.push_back(inPoint);

                                outPoint.x = currentPoint.x;
                                outPoint.y = mark.down;
                                outPoints.push_back(outPoint);

                                cutEdge.direction = currentEdge.direction;
                                cutEdge.length = mark.up - mark.down;
                                cutEdges.push_back(cutEdge);
                            }
                        }

                        break;
                    }

                    case 3: {
                        if (currentPoint.y > mark.down && currentPoint.y < mark.up) {
                            if (currentPoint.x > mark.right && nextPoint.x < mark.left) {
                                cutEdgeNum ++;
                                inPoint.x = mark.right;
                                inPoint.y = currentPoint.y;
                                inPoints.push_back(inPoint);

                                outPoint.x = mark.left;
                                outPoint.y = currentPoint.y;
                                outPoints.push_back(outPoint);

                                cutEdge.direction = currentEdge.direction;
                                cutEdge.length = mark.right - mark.left;
                            }
                        }

                        break;
                    }
                }
            }

            if (cutEdgeNum == 0) {
                ansMan.up = mark.up;
                ansMan.down = mark.down;
                ansMan.left = mark.left;
                ansMan.right = mark.right;
                ansMan.x = mark.left;
                ansMan.y = mark.up;
                vector<edge> ansEdges;
                ansEdges.push_back({mark.up - mark.down, 2});
                ansEdges.push_back({mark.right - mark.left, 1});
                ansEdges.push_back({mark.up - mark.down, 0});
                ansEdges.push_back({mark.right - mark.left, 3});

                vector<Point> ansPoints;
                ansPoints.push_back({mark.left, mark.up});
                ansPoints.push_back({mark.left, mark.down});
                ansPoints.push_back({mark.right, mark.down});
                ansPoints.push_back({mark.right, mark.up});

                ansMan.edges = ansEdges;
                ansMan.points = ansPoints;

                ansMans.push_back(ansMan);

                return ansMans;
            } else {
                int i = 0;
                edge currentEdge;
                vector<edge> currentEdges;
                switch (cutEdges[0].direction) {
                    case 0: {
                        while (cutEdgeNum > 1) {
                            cutEdgeNum -= 2;
                            
                        }
                    }
                }
            }
        }

        for (int i = 0; i < ManHT.edges.size(); i++) {
            edge currentEdge = ManHT.edges[i % ManHT.edges.size()];
            switch (currentEdge.direction) {
                case 0: nextPoint.y += currentEdge.length; break;
                case 1: nextPoint.x += currentEdge.length; break;
                case 2: nextPoint.y -= currentEdge.length; break;
                case 3: nextPoint.x -= currentEdge.length; break;
                default: break;
            }
            if (checkPointInMarker(nextPoint, mark)) {
                currentPoint = nextPoint;
                currentEdges.push_back(currentEdge);
            } else  {
                Point outPoint = currentPoint;
                edge outEdge = currentEdge;
                switch (currentEdge.direction) {
                    case 0: {
                        currentEdge.length = mark.up - currentPoint.y;
                        currentPoint.y = mark.up;
                        break;
                    }
                    case 1: {
                        currentEdge.length = mark.right - currentPoint.x;
                        currentPoint.x = mark.right; 
                        break;
                    }
                    case 2: {
                        currentEdge.length = currentPoint.y - mark.down;
                        currentPoint.y = mark.down;
                        break;
                    }
                    case 3: {
                        currentEdge.length = currentPoint.x - mark.left;
                        currentPoint.x = mark.left;
                        break;
                    }
                    default: break;
                }

                currentEdges.push_back(currentEdge);

                while (!checkPointInMarker(outPoint, mark)) {
                    i ++;
                    outEdge = ManHT.edges[i];
                    switch (outEdge.direction) {
                        case 0: outPoint.y += outEdge.length; break;
                        case 1: outPoint.x += outEdge.length; break;
                        case 2: outPoint.y -= outEdge.length; break;
                        case 3: outPoint.x -= outEdge.length; break;
                        default: break;
                    }
                }

                Point inPoint;

                switch (outEdge.direction) {
                    case 0: {
                        inPoint.x = outPoint.x;
                        inPoint.y = mark.down;
                        break;
                    }
                    case 1: {
                        inPoint.x = mark.left;
                        inPoint.y = outPoint.y;
                        break;
                    }
                    case 2: {
                        inPoint.x = outPoint.x;
                        inPoint.y = mark.up;
                        break;
                    }
                    case 3: {
                        inPoint.x = mark.right;
                        inPoint.y = outPoint.y;
                        break;
                    }
                }

                if (directionOperetor(currentEdge.direction, outEdge.direction, '-') == 2) {
                    edge cutEdge;
                    switch (currentEdge.direction) {
                        case 0: {
                            if (inPoint.x > currentPoint.x) {
                                cutEdge.direction = 1;
                                cutEdge.length = inPoint.x - currentPoint.x;
                            } else if (inPoint.x < currentPoint.x) {
                                cutEdge.direction = 3;
                                cutEdge.length = currentPoint.x - inPoint.x;
                            } else {
                                cout << "Error: Invalid edge" << endl;
                            }
                            break;
                        }

                        case 1: {
                            if (inPoint.y > currentPoint.y) {
                                cutEdge.direction = 0;
                                cutEdge.length = inPoint.y - currentPoint.y;
                            } else if (inPoint.y < currentPoint.y) {
                                cutEdge.direction = 2;
                                cutEdge.length = currentPoint.y - inPoint.y;
                            } else {
                                cout << "Error: Invalid edge" << endl;
                            }
                            break;
                        }

                        case 2: {
                            if (inPoint.x > currentPoint.x) {
                                cutEdge.direction = 1;
                                cutEdge.length = inPoint.x - currentPoint.x;
                            } else if (inPoint.x < currentPoint.x) {
                                cutEdge.direction = 3;
                                cutEdge.length = currentPoint.x - inPoint.x;
                            } else {
                                cout << "Error: Invalid edge" << endl;
                            }
                            break;
                        }

                        case 3: {
                            if (inPoint.y > currentPoint.y) {
                                cutEdge.direction = 0;
                                cutEdge.length = inPoint.y - currentPoint.y;
                            } else if (inPoint.y < currentPoint.y) {
                                cutEdge.direction = 2;
                                cutEdge.length = currentPoint.y - inPoint.y;
                            } else {
                                cout << "Error: Invalid edge" << endl;
                            }
                            break;
                        }

                        default: break;
                        
                    }

                    currentEdges.push_back(cutEdge);
                } else if (abs(directionOperetor(currentEdge.direction, outEdge.direction, '-')) == 1) {
                    edge cutEdge1, cutEdge2;
                    switch (currentEdge.direction) {
                        case 0: {
                            switch (outEdge.direction) {
                                case 1: {
                                    cutEdge1.direction = 3;
                                    cutEdge1.length = currentPoint.x - mark.left;
                                    break;
                                }
                                case 3: {
                                    cutEdge1.direction = 1;
                                    cutEdge1.length = mark.right - currentPoint.x;
                                    break;
                                }

                                default: break;
                            }
                            cutEdge2.direction = 2;
                            cutEdge2.length = mark.up - inPoint.y;
                            break;
                        }

                        case1 : {
                            switch (outEdge.direction) {
                                case 0: {
                                    cutEdge1.direction = 2;
                                    cutEdge1.length = currentPoint.y - mark.up;
                                    break;
                                }

                                case 2: {
                                    cutEdge1.direction = 0;
                                    cutEdge1.length = mark.down - currentPoint.y;
                                    break;
                                }

                                default: break;
                            }
                            cutEdge2.direction = 3;
                            cutEdge2.length = mark.right - inPoint.x;
                            break;
                        }

                        case 2: {
                            switch (outEdge.direction) {
                                case 1: {
                                    cutEdge1.direction = 3;
                                    cutEdge1.length = currentPoint.x - mark.left;
                                    break;
                                }
                                case 3: {
                                    cutEdge1.direction = 1;
                                    cutEdge1.length = mark.right - currentPoint.x;
                                    break;
                                }

                                default: break;
                            }
                            cutEdge2.direction = 0;
                            cutEdge2.length = inPoint.y - mark.down;
                            break;
                        }

                        case 3: {
                            switch (outEdge.direction) {
                                case 0: {
                                    cutEdge1.direction = 2;
                                    cutEdge1.length = currentPoint.y - mark.up;
                                    break;
                                }

                                case 2: {
                                    cutEdge1.direction = 0;
                                    cutEdge1.length = mark.down - currentPoint.y;
                                    break;
                                }

                                default: break;
                            }
                            cutEdge2.direction = 3;
                            cutEdge2.length = inPoint.x - mark.left;
                        }

                        default: break;
                    }
                    currentEdges.push_back(cutEdge1);
                    currentEdges.push_back(cutEdge2);
                } else if (directionOperetor(currentEdge.direction, outEdge.direction, '-') == 0) {
                    edge cutEdge1, cutEdge2, cutEdge3;
                    switch (currentEdge.direction) {
                        case 0: {
                            if (ManHT.right >= mark.right) {
                                cutEdge1.direction = 1;
                                cutEdge1.length = mark.right - currentPoint.x;
                                cutEdge2.direction = 2;
                                cutEdge2.length = mark.up - mark.down;
                                cutEdge3.direction = 3;
                                cutEdge3.length = mark.right - currentPoint.x;

                            } else {
                                cutEdge1.direction = 3;
                                cutEdge1.length = currentPoint.x - mark.left;
                                cutEdge2.direction = 2;
                                cutEdge2.length = mark.up - mark.down;
                                cutEdge3.direction = 1;
                                cutEdge3.length = inPoint.x - mark.left;
                            }

                            break;
                        }

                        case 1: {
                            if (ManHT.up >= mark.up) {
                                cutEdge1.direction = 0;
                                cutEdge1.length = mark.up - currentPoint.y;
                                cutEdge2.direction = 3;
                                cutEdge2.length = mark.right - mark.left;
                                cutEdge3.direction = 2;
                                cutEdge3.length = mark.up - inPoint.y;
                            } else {
                                cutEdge1.direction = 2;
                                cutEdge1.length = currentPoint.y - mark.down;
                                cutEdge2.direction = 3;
                                cutEdge2.length = mark.right - mark.left;
                                cutEdge3.direction = 0;
                                cutEdge3.length = inPoint.y - mark.down;
                            }
                            break;
                        }

                        case 2: {
                            if (ManHT.left >= mark.left) {
                                cutEdge1.direction = 1;
                                cutEdge1.length = currentPoint.x - mark.left;
                                cutEdge2.direction = 0;
                                cutEdge2.length = mark.up - mark.down;
                                cutEdge3.direction = 3;
                                cutEdge3.length = inPoint.x - mark.left;
                            } else {
                                cutEdge1.direction = 3;
                                cutEdge1.length = mark.right - currentPoint.x;
                                cutEdge2.direction = 0;
                                cutEdge2.length = mark.up - mark.down;
                                cutEdge3.direction = 1;
                                cutEdge3.length = mark.right - inPoint.x;
                            }
                            break;
                        }

                        case 3: {
                            if (ManHT.up >= mark.up) {
                                cutEdge1.direction = 0;
                                cutEdge1.length = mark.up - currentPoint.y;
                                cutEdge2.direction = 1;
                                cutEdge2.length = mark.right - mark.left;
                                cutEdge3.direction = 2;
                                cutEdge3.length = mark.up - inPoint.y;
                            } else {
                                cutEdge1.direction = 2;
                                cutEdge1.length = currentPoint.y - mark.down;
                                cutEdge2.direction = 1;
                                cutEdge2.length = mark.right - mark.left;
                                cutEdge3.direction = 0;
                                cutEdge3.length = inPoint.y - mark.down;
                            }
                            break;
                        }

                        default: break;
                    }
                    currentEdges.push_back(cutEdge1);
                    currentEdges.push_back(cutEdge2);
                    currentEdges.push_back(cutEdge3);
                }

                currentPoint = outPoint;
                nextPoint = outPoint;
                currentEdge.direction = outEdge.direction;
                switch (outEdge.direction) {
                    case 0: currentEdge.length = outPoint.y - inPoint.y; break;
                    case 1: currentEdge.length = outPoint.x - inPoint.x; break;
                    case 2: currentEdge.length = inPoint.y - outPoint.y; break;
                    case 3: currentEdge.length = inPoint.x - outPoint.x; break;
                    default: break;
                }
                currentEdges.push_back(currentEdge);
            }
        }
        ansMan.edges = currentEdges;
        ansMan.x = currentPoint.x;
        ansMan.y = currentPoint.y;
        ansMan = fixManhatten(ansMan, 2);
        ansMans.push_back(ansMan);

        return ansMans;
    }
};

class test {
    public:
    int outEdges (Manhatten ManHT) {
        if (ManHT.edges.empty()) {
            cout << "Error: No edges" << endl;
            return -1;
        }

        cout << "Manhattan Plot: " << ManHT.x << "," << ManHT.y << endl;
        for (auto edge : ManHT.edges) {
            cout << "Edge: " << edge.length << "," << edge.direction << endl;
        }

        for (auto Point : ManHT.points) {
            cout << "(" << Point.x << "," << Point.y << "),";
        }

        cout << endl;

        cout << "Manhatten's number of inconer points: " << ManHT.incornerCount << endl;
        cout << "Manhatten's number of outcorner points: " << ManHT.outcornerCount << endl;
        for (auto pair : ManHT.mEdgeLength) {
            cout << "There are " << pair.second << " edges whose length is " << pair.first << endl;
        }
        return 0;
    }

    int testReadPattern () {
        Read r;
        auto patternMap = r.readPattern("./testset/small/small_pattern.txt");
        cout << "Pattern Map: " << patternMap.patterns.size() << endl;
        for (auto pattern : patternMap.patterns) {
            int marked = 0;
            cout << "Pattern: " << pattern.patternNum << endl;
            for (auto layer : pattern.layers) {
                cout << "Layer: " << layer.layerNum << endl;
                if (layer.marked) {cout << "This layer have marker manhatten" << endl; marked ++;}
                else cout << "This layer don't have marker manhatten" << endl;
                cout << "Mark Manhatten: " << layer.markManhatten.x << "," << layer.markManhatten.y << endl;
                for (auto ManHT : layer.Manhattens) {
                    cout << "Manhattan Plot: " << ManHT.x << "," << ManHT.y << endl;
                    for (int i = 0; i < ManHT.points.size(); i++) {
                        cout << "Edge" << i << "'length: " << ManHT.edges[i].length << ", direction: " << ManHT.edges[i].direction << endl;
                    }
                }
            }
            cout << "this pattern have " << marked << " markers" << endl;
            cout << "Pattern'marker: " << endl;
            cout << "top: " << pattern.mark.up << endl;
            cout << "bottom: " << pattern.mark.down << endl;
            cout << "left: " << pattern.mark.left << endl;
            cout << "right: " << pattern.mark.right << endl;
            marked = 0;
        }
        return 1;
    }

    int testReadEdge () {
        Read r;
        string MHTPoints1 = "(2100,2400),(2700,2400),(2700,5700),(7200,5700),(7200,6000),(7500,6000),(7500,7200),(6300,7200),(6300,6300),(2700,6300),(2700,10500),(3300,10500),(3300,12600),(2100,12600)";
        string MHTPoints2 = "(6300,6000),(6900,6000),(6900,4200),(8400,4200),(8400,5400),(8100,5400),(8100,12000),(6600,12000),(6600,10800),(6900,10800),(6900,7200),(6300,7200)";
        Polygon Pol;
        r.readCoordinatesFromString(MHTPoints2, Pol.points);
        Manhatten ManHT = r.readEdge(Pol);
        if (ManHT.edges.empty()) {
            cout << "Error: No edges" << endl;
            return -1;
        }

        cout << "Manhattan Plot: " << ManHT.x << "," << ManHT.y << endl;
        for (auto edge : ManHT.edges) {
            cout << "Edge: " << edge.length << "," << edge.direction << endl;
        }

        cout << "Manhatten's number of inconer points: " << ManHT.incornerCount << endl;
        cout << "Manhatten's number of outcorner points: " << ManHT.outcornerCount << endl;
        for (auto pair : ManHT.mEdgeLength) {
            cout << "There are " << pair.second << " edges whose length is " << pair.first << endl;
        }

        return 0;
    }

    int testDirectionOperator () {
        baseOps M;
        cout << M.directionOperetor(1, 3, '-') << endl;
        cout << M.directionOperetor(1, 0, '-') << endl;
        cout << M.directionOperetor(0, 3, '-') << endl;
        cout << M.directionOperetor(3, 0, '-') << endl;
        cout << M.directionOperetor(1, 3, '+') << endl;
        cout << M.directionOperetor(1, 0, '+') << endl;
        return 0;
    }

    int testCutEdges () {
        Read r;
        string MHTPoints1 = "(2100,2400),(2700,2400),(2700,5700),(7200,5700),(7200,6000),(7500,6000),(7500,7200),(6300,7200),(6300,6300),(2700,6300),(2700,10500),(3300,10500),(3300,12600),(2100,12600)";
        string MHTPoints2 = "(6300,6000),(6900,6000),(6900,4200),(8400,4200),(8400,5400),(8100,5400),(8100,12000),(6600,12000),(6600,10800),(6900,10800),(6900,7200),(6300,7200)";
        Polygon Pol;
        r.readCoordinatesFromString(MHTPoints2, Pol.points);
        Manhatten ManHT = r.readEdge(Pol);


        return 0;
    }

    int testManhattenMatch () {
        FuzzyMatching FM;
        Read r;
        baseOps BO;
        string MHTPoints1 = "(2100,2400),(2700,2400),(2700,5700),(7200,5700),(7200,6000),(7500,6000),(7500,7200),(6300,7200),(6300,6300),(2700,6300),(2700,10500),(3300,10500),(3300,12600),(2100,12600)";
        string MHTPoints2 = "(600,9900),(600,9300),(3900,9300),(3900,4800),(4200,4800),(4200,4500),(5400,4500),(5400,5700),(4500,5700),(4500,9300),(8700,9300),(8700,8700),(10800,8700),(10800,9900)";
        string MHTPoints3 = "(8100,11400),(7500,11400),(7500,8100),(3000,8100),(3000,7800),(2700,7800),(2700,6600),(3900,6600),(3900,7500),(7500,7500),(7500,3300),(6900,3300),(6900,1200),(8100,1200)";
        string MHTPoints4 = "(9600,3900),(9600,4500),(6300,4500),(6300,9000),(6000,9000),(6000,9300),(4800,9300),(4800,8100),(5700,8100),(5700,4500),(1500,4500),(1500,5100),(-600,5100),(-600,3900)";

        Polygon Pol1, Pol2, Pol3, Pol4;
        r.readCoordinatesFromString(MHTPoints1, Pol1.points);
        r.readCoordinatesFromString(MHTPoints2, Pol2.points);
        r.readCoordinatesFromString(MHTPoints3, Pol3.points);
        r.readCoordinatesFromString(MHTPoints4, Pol4.points);

        Manhatten ManHT1 = r.readEdge(Pol1);
        Manhatten ManHT2 = r.readEdge(Pol2);
        int rotationMod = FM.ManhattenMatch(ManHT1, ManHT2);

        cout << rotationMod << endl;

        return 0;
    }

    int testFixManhatten () {
        FuzzyMatching FM;
        Read r;
        baseOps BO;
        string MHTPoints1 = "(2100,2400),(2700,2400),(2700,5700),(7200,5700),(7200,6000),(7500,6000),(7500,7200),(6300,7200),(6300,6300),(2700,6300),(2700,10500),(3300,10500),(3300,12600),(2100,12600)";
        Polygon Pol1;
        r.readCoordinatesFromString(MHTPoints1, Pol1.points);
        Manhatten ManHT1 = r.readEdge(Pol1);
        outEdges(ManHT1);
        Manhatten ManHT2 = BO.fixManhatten(ManHT1, 3);
        outEdges(ManHT2);
        return 0;
    }

    int testReadLayout () {
        Read r;
        auto layout1 = r.readLayout("./testset/large/large_layout.txt");

        for (auto layer : layout1.layers) {
            cout << "layer" << layer.layerNum << " have " << layer.Manhattens.size() << " Manhattens" << endl;            
        }

        return 0;
    }

    int testFindPotentialArea () {
        Read r;
        FuzzyMatching FM;
        clock_t start_t, finish_t;
        baseOps BO;
        start_t = clock();
        auto pattern = r.readPattern("./testset/large/largepattern.txt");
        finish_t = clock();
        cout << "Reading large pattern use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
        start_t = clock();
        auto layout = r.readLayout("./testset/large/large_layout.txt");
        finish_t = clock();
        cout << "Reading large layout use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
        cout << "Unmirrored:" << endl;
        for (int i = 0; i < pattern.patterns.size(); i++) {
            start_t = clock();
            auto potentialArea = FM.findPotentialArea(layout, pattern.patterns[i]);
            cout << "Num" << i+1 << " pattern have " << potentialArea.potentialMatchingAreas.size() << " potential areas" << endl;
            finish_t = clock();
            cout << "Finding potential areas in Num" << i+1 << " pattern use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
            map<int, int> Mnum;
            for (auto area : potentialArea.potentialMatchingAreas) {
                Mnum[area.matchLayer.size()] ++;
            }
            for (auto pair : Mnum) {
                cout << "There are " << pair.second << " areas whose match num is " << pair.first << endl;
            }
        }
        auto mirrorMap = BO.mirrorPatternMap(pattern);
        cout << "Mirrored:" << endl;
        for (int i = 0; i < mirrorMap.patterns.size(); i++) {
            start_t = clock();
            auto potentialArea = FM.findPotentialArea(layout, mirrorMap.patterns[i]);
            cout << "Num" << i+1 << " pattern have " << potentialArea.potentialMatchingAreas.size() << " potential areas" << endl;
            finish_t = clock();
            cout << "Finding potential areas in Num" << i+1 << " pattern use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
            map<int, int> Mnum;
            for (auto area : potentialArea.potentialMatchingAreas) {
                Mnum[area.matchLayer.size()] ++;
            }
            for (auto pair : Mnum) {
                cout << "There are " << pair.second << " areas whose match num is " << pair.first << endl;
            }
        }
        return 0;
    }

    int testmMirrorManhatten () {
        FuzzyMatching FM;
        Read r;
        baseOps BO;
        string MHTPoints1 = "(2100,2400),(2700,2400),(2700,5700),(7200,5700),(7200,6000),(7500,6000),(7500,7200),(6300,7200),(6300,6300),(2700,6300),(2700,10500),(3300,10500),(3300,12600),(2100,12600)";
        Polygon Pol1;
        r.readCoordinatesFromString(MHTPoints1, Pol1.points);
        Manhatten ManHT1 = r.readEdge(Pol1);
        outEdges(ManHT1);
        marker mark;
        mark.down = mark.left = 0;
        mark.up = 13200;
        mark.right = 10200;
        Manhatten ManHT2 = BO.mirrorManhatten(ManHT1, mark);
        outEdges(ManHT2);
        return 0;
    }

    void printKdTree(KdTree* tree, int depth) {
        // 检查 tree 是否为 NULL，如果是，则直接返回
        if (tree == NULL) {
            return;
        }

        cout << "(" << tree->root.x << "," << tree->root.y << ")" << endl;

        // 检查是否是叶子节点
        if (tree->leftChild == NULL && tree->rightChild == NULL) {
            return;
        } else {
            if (tree->leftChild != NULL) {
                for (int i = 0; i < depth + 1; i++) {
                    cout << "\t";
                }
                cout << "left:";
                printKdTree(tree->leftChild, depth + 1);
            }
            // 不需要在这里打印 endl，因为左子树和右子树的打印都会以 endl 结尾

            if (tree->rightChild != NULL) {
                for (int i = 0; i < depth + 1; i++) {
                    cout << "\t";
                }
                cout << "right:";
                printKdTree(tree->rightChild, depth + 1);
            }
        }
    }

    int testKdTree () {
        Read r;
        auto pattern = r.readPattern("./testset/small/small_pattern.txt");
        for (auto pat : pattern.patterns) {
            int i = 0;
            int j = 0;
            i ++;
            cout << "pattern" << i << " :" << endl;
            for (auto lay : pat.layers) {
                j++;
                cout << "layer" << j << " :" << endl;
                printKdTree(&lay.tree1, 0);
            }
            j = 0;
            cout << endl;
        }
        return 0;
    }

    int testFindPointInTree () {
        Read r;
        PreciseMatching PM;
        auto patternMap = r.readPattern("./testset/small/small_pattern.txt");
        auto layer = patternMap.patterns[0].layers[0];
        pointInTree point;
        point.x = 4800;
        point.y = 12000;
        KdTree* findedTree = PM.findPointInTree(&layer.tree1, point, 0);
        printKdTree(findedTree, 0);
        return 0;
    }

    int testAddManhattenToPotentialArea () {
        Read r;
        PreciseMatching PM;
        FuzzyMatching FM;
        baseOps BO;

        auto patternMap = r.readPattern("./testset/small/small_pattern.txt");
        auto layer = patternMap.patterns[0].layers[0];
        potentialMatchingArea area;
        area.down = 0;
        area.left = 1000;
        area.up = 13800;
        area.right = 10200;

        PM.addManhattensToPotentialArea(layer, area);

        for (auto point : area.corners) {
            cout << "(" << point.x << "," << point.y << ") , " ;
        }

        cout << endl;

        for (auto pair : area.ManhattenInArea) {
            cout << "Manhatten " << pair.first << " have " << pair.second << "corners in area" << endl;
        }

        return 0;
    }
};

int main () {
    clock_t start_t, finish_t;
    start_t = clock();
    test t;
    // cout << t.testReadPattern() << endl;
    // t.testDirectionOperator();
    // t.testReadEdge();
    // t.testReadPattern();
    // t.testFixManhatten();
    // t.testManhattenMatch();
    // t.testReadLayout();
    // t.testFindPotentialArea();
    // t.testmMirrorManhatten();
    // t.testKdTree();
    t.testFindPointInTree();
    // t.testAddManhattenToPotentialArea();
    finish_t = clock();
    cout << "Time: " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << endl;
    return 0;
}
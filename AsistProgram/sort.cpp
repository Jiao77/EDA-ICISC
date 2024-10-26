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
        return leftChild == NULL && rightChild == NULL;
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

struct matchInfo {
    vector<pointInTree> corners;
    map<int, int> ManhattenInArea;
};

struct potentialMatchingArea {
    int up;
    int down;
    int left;
    int right;

    int rotationMod;

    map <int, bool> matchLayer;
    int matchNum = 0;

    map<int, matchInfo> layerMatchInfo;

};

struct patternMatchResult {
    vector<potentialMatchingArea> potentialMatchingAreas;
};

struct layout {
    vector <layer> layers;
};

struct greaterPoint {
    bool operator() (pointInTree p1, pointInTree p2) {
        if (p1.x < p2.x) {
            return true;
        } else if (p1.x == p2.x) {
            if (p1.y < p2.y) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
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

int main () {
    Read r;
    auto patternMap = r.readPattern("../testset/small/small_pattern.txt");
    auto layer = patternMap.patterns[0].layers[0];
    auto corners = layer.pointsInTree;
    sort(corners.begin(), corners.end(), greaterPoint());
    for (auto point : corners) {
        cout << "(" << point.x << "," << point.y << ")" << endl;
    }
    return 0;
}
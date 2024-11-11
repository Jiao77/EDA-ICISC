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
#include <boost/geometry.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/assign.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/foreach.hpp>
#include <thread>


using namespace std;
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::d2::point_xy<int, boost::geometry::cs::cartesian> BPoint; 
typedef bg::model::box<BPoint> BBox; 
typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<int>, 0, 0> BPolygon;
typedef std::pair<BBox, unsigned> BValue;
typedef bgi::rtree<BValue, bgi::linear<16>> RTree;

class baseOps {
    public:

    struct Point {
        int x = 0;
        int y = 0;
    };

    struct Polygon {
        vector<Point> points;
    };

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
        BPolygon boostpoly;
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

    struct layer {
        //BPolygon boostpoly;
        vector<Manhatten> Manhattens;
        bool marked;
        Manhatten markManhatten;
        vector<Manhatten> comManhattens;
        int layerNum = 0; //the layer number
        vector<pointInTree> pointsInTree;
        RTree rtree;
        //RTree rtree1 = RTree();
        //KdTree* rtree = rtree1;
        //KdTree tree1 = KdTree();
        //KdTree* tree = &tree1;
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
        map<int, int> index;

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

    void buildRTree (RTree rtree, vector<Manhatten> Manhattens) {
        unsigned num = Manhattens.size();
        
        for (int i = 0; i < num; i++){
            BBox box;
            BPolygon poly;
            for (int j=0;j<Manhattens[i].points.size();j++){
                bg::append(poly.outer(), BPoint(Manhattens[i].points[j].x, Manhattens[i].points[j].y));
                //cout << "point" << Manhattens[i].points[j].x<< "," << Manhattens[i].points[j].y << endl;
            }
            //cout << "num" << i << endl;
            boost::geometry::envelope(poly, box);
            //std::cout <<"num" << i <<"box:" << boost::geometry::dsv(box) << "poly:"<<boost::geometry::dsv(poly) << std::endl;
            
            //box.min_corner() = BPoint(Manhattens[i].left, Manhattens[i].down);  // 设置最小点为(2, 2)
            //box.max_corner() = BPoint(Manhattens[i].right,Manhattens[i].up);
            rtree.insert(std::make_pair(box, i));
        }
        //std::cout << "rtreesize1 " << rtree.size() <<  std::endl;
        
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
                inLayer.comManhattens.push_back(m);
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
                    sort(nowLayer.pointsInTree.begin(), nowLayer.pointsInTree.end(), greaterPoint());
                    // buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
                    // buildRTree(nowLayer.rtree, nowLayer.Manhattens);
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
                    sort(nowLayer.pointsInTree.begin(), nowLayer.pointsInTree.end(), greaterPoint());
                    //buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
                    //buildRTree(nowLayer.rtree, nowLayer.Manhattens);
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
            sort(nowLayer.pointsInTree.begin(), nowLayer.pointsInTree.end(), greaterPoint());
            // buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
            // buildRTree(nowLayer.rtree, nowLayer.Manhattens);
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
                    //std::cout << "layerNum1: " << layerNum <<  std::endl;
                    //std::cout << "rtreesize1 " << nowLayer.rtree.size() <<  std::endl;
                    nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
                    sort(nowLayer.pointsInTree.begin(), nowLayer.pointsInTree.end(), greaterPoint());
                    // buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
                    
                    //buildRTree(nowLayer.rtree, currentLayer);

                    unsigned num = currentLayer.size();
        
                    for (int i = 0; i < num; i++){
                        BBox box;
                        BPolygon poly;
                        for (int j=0;j<currentLayer[i].points.size();j++){
                            bg::append(poly.outer(), BPoint(currentLayer[i].points[j].x, currentLayer[i].points[j].y));
                            //cout << "point" << Manhattens[i].points[j].x<< "," << Manhattens[i].points[j].y << endl;
                        }
                        //currentLayer[i].boostpoly = poly;
                        //cout << "num" << i << endl;
                        boost::geometry::envelope(poly, box);
                        //std::cout <<"num" << i <<"box:" << boost::geometry::dsv(box) << "poly:"<<boost::geometry::dsv(poly) << std::endl;
            
                        //box.min_corner() = BPoint(Manhattens[i].left, Manhattens[i].down);  // 设置最小点为(2, 2)
                        //box.max_corner() = BPoint(Manhattens[i].right,Manhattens[i].up);
                        nowLayer.rtree.insert(std::make_pair(box, i));
                 }
                    //std::cout << "rtreesize1 " << nowLayer.rtree.size() <<  std::endl;
                    //std::cout << "rtreesize2 " << nowLayer.rtree.size() <<  std::endl;
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
            //std::cout << "layerNum2: " << layerNum <<  std::endl;
            nowLayer.pointsInTree = generatePOintsInTree(nowLayer);
            sort(nowLayer.pointsInTree.begin(), nowLayer.pointsInTree.end(), greaterPoint());
            //buildKdTree(nowLayer.tree, nowLayer.pointsInTree, 0);
            //buildRTree(nowLayer.rtree, currentLayer);
            //buildrtree!!!
            unsigned num = currentLayer.size();
        
                    for (int i = 0; i < num; i++){
                        BBox box;
                        BPolygon poly;
                        for (int j=0;j<currentLayer[i].points.size();j++){
                            bg::append(poly.outer(), BPoint(currentLayer[i].points[j].x, currentLayer[i].points[j].y));
                            //cout << "point" << Manhattens[i].points[j].x<< "," << Manhattens[i].points[j].y << endl;
                        }
                        //currentLayer[i].boostpoly = poly;
                        //cout << "num" << i << endl;
                        boost::geometry::envelope(poly, box);
                        //std::cout <<"num" << i <<"box:" << boost::geometry::dsv(box) << "poly:"<<boost::geometry::dsv(poly) << std::endl;
            
                        nowLayer.rtree.insert(std::make_pair(box, i));
                 }
                    //std::cout << "rtreesize1 " << nowLayer.rtree.size() <<  std::endl;
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
            } else if (layerInPattern.markManhatten.edges.size() == 4) {
                continue;
            }

            for (auto ManhattenInLayout : layerInLayout.Manhattens) {
                int i = 0;
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
                        Area1.index[layerInLayout.layerNum] = i;

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
                        Area1.index[layerInLayout.layerNum] = i;

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
                        Area1.index[layerInLayout.layerNum] = i;

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
                        Area1.index[layerInLayout.layerNum] = i;

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
                i++;
            }
        }
        return result;
    }
};

class test : baseOps {
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

int testfunc (int i, patternMap pattern, FuzzyMatching FM, layout layout) {
        //for (int i = 0; i < 2; i++) {
        clock_t start_t, finish_t;
            start_t = clock();
            auto potentialArea = FM.findPotentialArea(layout, pattern.patterns[i]);
            cout << "Num" << i+1 << " pattern have " << potentialArea.potentialMatchingAreas.size() << " potential areas" << endl;
            finish_t = clock();
            cout << "Finding potential areas in Num" << i+1 << " pattern use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
            int boolcount=0;
            Pattern CP = pattern.patterns[i];
            ///* 潜在匹配区域储存!!!
                for (int j = 0; j < potentialArea.potentialMatchingAreas.size(); j++) {
                    //分区域
                    int x1=potentialArea.potentialMatchingAreas[j].left;
                    int y1=potentialArea.potentialMatchingAreas[j].down;
                    int x2=potentialArea.potentialMatchingAreas[j].right;
                    int y2=potentialArea.potentialMatchingAreas[j].up;
                    //分layer
                    //std::cout << "spatial query box:" << j+1 << " is (" << x1 << "," << y1 <<"),(" << x2 << "," << y2 << ")" << std::endl;
                    for (int k = 0; k< layout.layers.size(); k++){
                        //cout << "intersect with polygons in layer" << k+1 << "：" << endl;
                        BBox query_box(BPoint(x1, y1), BPoint(x2, y2));
	                    std::vector<BValue> result_s;
                        //std::cout << "rtree size:"<< layout.layers[k].rtree.size() <<  std::endl;
	                    layout.layers[k].rtree.query(bgi::intersects(query_box), std::back_inserter(result_s));
                        
	                    //std::cout << "spatial query result:" << std::endl;
                        //using boost::geometry::dsv;
                        //std::vector<boost::geometry::model::multi_polygon<BPolygon>> CL;
                        boost::geometry::model::multi_polygon<BPolygon> CL;
                        //bg::model::multi_polygon<boost::geometry::model::multi_polygon<BPolygon>> CL;
                        int nresult=result_s.size();
	                    for (int l=0; l<nresult; l++){                  
                            BPolygon poly;
                            std::vector<BPolygon> booland;
                            //BPolygon booland;
                            for (int jj=0;jj<layout.layers[k].Manhattens[result_s[l].second].points.size();jj++){
                                bg::append(poly.outer(), BPoint(layout.layers[k].Manhattens[result_s[l].second].points[jj].x, layout.layers[k].Manhattens[result_s[l].second].points[jj].y));
                                //cout << "point" << Manhattens[i].points[j].x<< "," << Manhattens[i].points[j].y << endl;
                                //std::cout << boost::geometry::dsv(booland) << std::endl;
                            }
                            boost::geometry::intersection(poly, query_box, booland);
                            BOOST_FOREACH(BPolygon const& boolp, booland)
                            {
                                CL.push_back(boolp);
                            }
                            
                        }
                        //int currentlayerpatternsize = CP.layers[k].Manhattens.size();
                        boost::geometry::model::multi_polygon<BPolygon> CCP;
                        
                        //*
	                    for (int m=0; m<CP.layers[k].Manhattens.size(); m++){                  
                            BPolygon patternpoly;
                            
                            for (int jj=0;jj<CP.layers[k].Manhattens[m].points.size();jj++){
                                bg::append(patternpoly.outer(), BPoint(CP.layers[k].Manhattens[m].points[jj].x+x1, CP.layers[k].Manhattens[m].points[jj].y+y1));
                                //cout << "point" << Manhattens[i].points[j].x<< "," << Manhattens[i].points[j].y << endl;
                                //std::cout << boost::geometry::dsv(booland) << std::endl;
                            }
                            CCP.push_back(patternpoly);
                        }
                        
                        boost::geometry::model::multi_polygon<BPolygon> booxor;
                        boost::geometry::sym_difference(CCP, CL, booxor);
                        //*/

                        //std::cout
                        //    << "layer " << k << " XOR:" << std::endl
                        //    << boost::geometry::dsv(booxor) << std::endl;
                        boolcount=boolcount+1;    
                        //cout << "bool caculate" << boolcount << "times" << endl;
                        //std::cout << boost::geometry::dsv(booland) << std::endl;
                    }
                }
                //cout << "bool caculate" << boolcount << "times" << endl;
            //*/
            map<int, int> Mnum;
            for (auto area : potentialArea.potentialMatchingAreas) {
                Mnum[area.matchLayer.size()] ++;
            }
            for (auto pair : Mnum) {
                cout << "bool caculate " << boolcount << " times, There are " << pair.second << " areas whose match num is " << pair.first << endl;
            }
            return 0;
        }

    /*
    int testFindPotentialArea () {
        Read r;
        FuzzyMatching FM;
        //PreciseMatching PM;
        clock_t start_t, finish_t;
        baseOps BO;
        start_t = clock();
        auto pattern = r.readPattern("./testset/small/small_pattern.txt");
        finish_t = clock();
        cout << "Reading small pattern use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
        start_t = clock();
        auto layout = r.readLayout("./testset/small/small_layout.txt");
        finish_t = clock();
        cout << "Reading small layout use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
        cout << "Unmirrored:" << endl;
        //分pattern
        for (int i = 0; i < pattern.patterns.size(); i++) {
        //for (int i = 0; i < 2; i++) {
            testfunc(i, pattern, FM, layout);
        }
        return 0;
    }
    */

};

int main () {
    clock_t mstart_t, mfinish_t;
    mstart_t = clock();
    test t;
    //t.testReadLayout();
    //t.testFindPotentialArea();
    Read r;
    FuzzyMatching FM;
        //PreciseMatching PM;
    clock_t start_t, finish_t;
    baseOps BO;
    start_t = clock();
    auto pattern = r.readPattern("./testset/small/small_pattern.txt");
    finish_t = clock();
    cout << "Reading small pattern use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
    start_t = clock();
    auto layout = r.readLayout("./testset/small/small_layout.txt");
    finish_t = clock();
    cout << "Reading small layout use " << (double)(finish_t - start_t) / CLOCKS_PER_SEC << " s" << endl;
    cout << "Unmirrored:" << endl;
    //分pattern
    for (int i = 0; i < pattern.patterns.size(); i++) {
    //for (int i = 0; i < 2; i++) {
        t.testfunc(i, pattern, FM, layout);
    }
    mfinish_t = clock();
    cout << "Time: " << (double)(mfinish_t - mstart_t) / CLOCKS_PER_SEC << endl;
    return 0;
}

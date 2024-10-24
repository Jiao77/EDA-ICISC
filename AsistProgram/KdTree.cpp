#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

struct KdTree {
    vector <double> root;
    KdTree* parent;
    KdTree* leftChild;
    KdTree* rightChild;
    KdTree() {
        parent = leftChild = rightChild = NULL;
    }
    bool isEmpty() {
        return root.empty();
    }
    bool isRoot() {
        return (!isEmpty()) && parent == NULL;
    }
    bool isLeft() {
        return parent -> leftChild -> root == root;
    }
    bool isRight() {
        return parent -> rightChild -> root == root;
    }
};

int data[6][2] = {{2,3},{5,4},{9,6},{4,7},{8,1},{7,2}};

template<typename T>
vector<vector<T>> Transpose(vector<vector<T>> Matrix) {
    unsigned row = Matrix.size();
    unsigned col = Matrix[0].size();
    vector<vector<T>> Trans(col, vector<T>(row, 0));
    for (unsigned i = 0; i < col; i++) {
        for (unsigned j = 0;j < row; j++) {
            Trans[i][j] = Matrix[j][i];
        }
    }
    return Trans;
}

template <typename T>
T findMiddleValue (vector<T> vec) {
    sort(vec.begin(), vec.end());
    auto pos = vec.size() / 2;
    return vec[pos];
}

void buildKdTree (KdTree* tree, vector<vector<double>> data, unsigned depth) {
    unsigned samplesNum = data.size();
    if (samplesNum == 0) {
        return ;
    }
    if (samplesNum == 1) {
        tree -> root = data[0];
        return ;
    }

    unsigned k = data[0].size();
    vector<vector<double>> transData = Transpose(data);
    unsigned splitAttribute = depth % k;
    vector<double> splitAttributeValues = transData[splitAttribute];
    double splitValue = findMiddleValue(splitAttributeValues);
    
}
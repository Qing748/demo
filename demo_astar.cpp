#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_map>
#include <algorithm>

using namespace std;

// 定义方向：上、下、左、右
const int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

// 定义节点结构体
struct Node {
    int x, y; // 坐标
    int g, h; // G值和H值
    Node* parent; // 父节点指针

    Node(int x, int y, int g, int h, Node* parent = nullptr)
        : x(x), y(y), g(g), h(h), parent(parent) {}

    // 计算F值
    int f() const { return g + h; }

    // 重载小于运算符，用于优先队列排序
    bool operator<(const Node& other) const {
        return f() > other.f();
    }
};

// 判断坐标是否合法
bool isValid(int x, int y, int rows, int cols, const vector<vector<int>>& grid) {
    return x >= 0 && x < rows && y >= 0 && y < cols && grid[x][y] == 0;
}

// A*算法实现
vector<pair<int, int>> aStar(const vector<vector<int>>& grid, pair<int, int> start, pair<int, int> end) {
    int rows = grid.size();
    int cols = grid[0].size();

    // 创建起点和终点节点
    Node* startNode = new Node(start.first, start.second, 0, abs(start.first - end.first) + abs(start.second - end.second));
    Node* endNode = new Node(end.first, end.second, 0, 0);

    // 开放列表和关闭列表
    priority_queue<Node*> openList;
    unordered_map<int, unordered_map<int, bool>> closedList;

    // 将起点加入开放列表
    openList.push(startNode);

    while (!openList.empty()) {
        // 获取F值最小的节点
        Node* current = openList.top();
        openList.pop();

        // 如果到达终点，构建路径
        if (current->x == endNode->x && current->y == endNode->y) {
            vector<pair<int, int>> path;
            Node* temp = current;
            while (temp != nullptr) {
                path.push_back({temp->x, temp->y});
                temp = temp->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        // 将当前节点加入关闭列表
        closedList[current->x][current->y] = true;

        // 遍历相邻节点
        for (const auto& dir : directions) {
            int newX = current->x + dir[0];
            int newY = current->y + dir[1];

            if (isValid(newX, newY, rows, cols, grid) && !closedList[newX][newY]) {
                int g = current->g + 1;
                int h = abs(newX - endNode->x) + abs(newY - endNode->y);
                Node* neighbor = new Node(newX, newY, g, h, current); // 使用current作为父节点
                openList.push(neighbor);
            }
        }
    }

    // 如果没有路径，返回空路径
    return {};
}

int main() {
    // 定义较大的地图，0表示可通行，1表示障碍物
    vector<vector<int>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 0, 0, 0, 1, 1, 0},
        {0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
        {0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 0}
    };

    // 设置起点和终点
    pair<int, int> start = {0, 0};
    pair<int, int> end = {9, 9};

    vector<pair<int, int>> path = aStar(grid, start, end);

    if (!path.empty()) {
        cout << "路径找到：\n";
        for (const auto& p : path) {
            cout << "(" << p.first << ", " << p.second << ")\n";
        }
    } else {
        cout << "未找到路径。\n";
    }

    return 0;
}

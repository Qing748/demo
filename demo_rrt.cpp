#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <SFML/Graphics.hpp> // 用于可视化

// 定义环境大小和障碍物
const int width = 100;
const int height = 100;
const int step_size = 2;
const float goal_bias = 0.1; // 目标偏向的概率

struct Node {
    float x, y;
    Node* parent;
    Node(float x, float y) : x(x), y(y), parent(nullptr) {}
};

// 障碍物检查
bool check_collision(Node* p1, Node* p2, const std::vector<std::tuple<int, int, int>>& obstacles) {
    for (const auto& obstacle : obstacles) {
        int ox, oy, r;
        std::tie(ox, oy, r) = obstacle;
        // 计算线段到圆心的距离
        float d = std::abs((p2->y - p1->y) * p1->x - (p2->x - p1->x) * p1->y + p2->x * p1->y - p2->y * p1->x) /
                  std::sqrt(std::pow(p2->x - p1->x, 2) + std::pow(p2->y - p1->y, 2));
        if (d <= r) {
            return true; // 碰撞
        }
    }
    return false;
}

// 随机采样目标点
Node* random_sample(Node* goal) {
    float rx, ry;
    if (rand() % 100 < goal_bias * 100) {
        rx = goal->x;
        ry = goal->y;
    } else {
        rx = rand() % width;
        ry = rand() % height;
    }
    return new Node(rx, ry);
}

// 扩展树
Node* extend_tree(Node* nearest, Node* random_point, std::vector<Node*>& tree, const std::vector<std::tuple<int, int, int>>& obstacles) {
    // 计算方向并扩展节点
    float dx = random_point->x - nearest->x;
    float dy = random_point->y - nearest->y;
    float dist = std::sqrt(dx * dx + dy * dy);
    
    // 计算扩展步长
    if (dist > step_size) {
        dx = dx / dist * step_size;
        dy = dy / dist * step_size;
    }

    Node* new_node = new Node(nearest->x + dx, nearest->y + dy);
    new_node->parent = nearest;

    // 检查扩展的节点是否与障碍物碰撞
    if (!check_collision(nearest, new_node, obstacles)) {
        tree.push_back(new_node);
        return new_node;
    }
    delete new_node;
    return nullptr;
}

// 绘制树和障碍物
void draw_tree(const std::vector<Node*>& tree, sf::RenderWindow& window, const std::vector<std::tuple<int, int, int>>& obstacles) {
    // 绘制障碍物
    for (const auto& obstacle : obstacles) {
        int ox, oy, r;
        std::tie(ox, oy, r) = obstacle;
        sf::CircleShape circle(r);
        circle.setFillColor(sf::Color::Red);
        circle.setPosition(ox - r, oy - r);
        window.draw(circle);
    }

    // 绘制树
    for (size_t i = 1; i < tree.size(); ++i) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(tree[i - 1]->x, tree[i - 1]->y), sf::Color::Blue),
            sf::Vertex(sf::Vector2f(tree[i]->x, tree[i]->y), sf::Color::Blue)
        };
        window.draw(line, 2, sf::Lines);
    }
}

int main() {
    srand(time(0));

    // 定义起点和终点
    Node* start = new Node(10, 10);
    Node* goal = new Node(90, 90);

    // 定义障碍物，格式为 (x, y, 半径)
    std::vector<std::tuple<int, int, int>> obstacles = {
        {40, 40, 10}, {60, 60, 10}, {70, 50, 10}
    };

    // 创建树
    std::vector<Node*> tree;
    tree.push_back(start);

    // 使用SFML来可视化
    sf::RenderWindow window(sf::VideoMode(500, 500), "RRT Path Planning");

    bool found = false;
    Node* last_node = nullptr;

    // RRT算法
    for (int i = 0; i < 1000; ++i) {
        Node* random_point = random_sample(goal);
        Node* nearest = tree.back();
        Node* new_node = extend_tree(nearest, random_point, tree, obstacles);

        // 如果扩展到目标区域
        if (new_node && std::sqrt(std::pow(new_node->x - goal->x, 2) + std::pow(new_node->y - goal->y, 2)) < step_size) {
            found = true;
            last_node = new_node;
            break;
        }
        delete random_point;

        // 清除并更新窗口
        window.clear();
        draw_tree(tree, window, obstacles);
        window.display();
        sf::sleep(sf::milliseconds(10));  // 延迟，用于可视化
    }

    if (found) {
        // 从目标节点反向追踪路径
        std::vector<Node*> path;
        while (last_node != nullptr) {
            path.push_back(last_node);
            last_node = last_node->parent;
        }

        // 打印路径
        std::cout << "找到路径！路径节点为：\n";
        std::reverse(path.begin(), path.end());
        for (const auto& node : path) {
            std::cout << "(" << node->x << ", " << node->y << ")\n";
        }
    } else {
        std::cout << "未找到路径！\n";
    }

    // 关闭窗口
    window.close();
    return 0;
}

#include "Npc.h"

Npc::Npc(const Position& pos, const std::string& repr, int health)
    : Entity(pos, repr), health(health), maxHealth(health) {}

bool Npc::isAlive() const { return health > 0; }

void Npc::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
}

#include "../Board.h"
#include <queue>
#include <map>
#include <cmath>

#include "../Board.h"
#include <queue>
#include <map>
#include <cmath>
#include <set>

struct Node {
    Position pos;
    int g, h;
    int f() const { return g + h; }
    bool operator>(const Node& other) const { return f() > other.f(); }
};

void Npc::moveToward(const Position& target, Board& board) {
    if (position.x == target.x && position.y == target.y) return;

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;
    std::map<int, Position> came_from;
    std::map<int, int> g_score;

    auto get_key = [&](const Position& p) { return p.y * board.width + p.x; };
    auto heuristic = [&](const Position& p) {
        return std::abs(p.x - target.x) + std::abs(p.y - target.y);
    };

    open_set.push({position, 0, heuristic(position)});
    g_score[get_key(position)] = 0;

    Position bestNode = position;
    int bestDist = heuristic(position);
    int searched = 0;
    int maxSearch = 400; // Increased search depth for A*

    // 8 directions (including diagonals) for smoother movement
    int dx[] = {1, -1, 0, 0, 1, 1, -1, -1};
    int dy[] = {0, 0, 1, -1, 1, -1, 1, -1};

    while (!open_set.empty() && searched < maxSearch) {
        Node current = open_set.top();
        open_set.pop();
        searched++;

        if (current.pos == target) {
            bestNode = current.pos;
            break;
        }

        int d = heuristic(current.pos);
        if (d < bestDist) {
            bestDist = d;
            bestNode = current.pos;
        }

        for (int i = 0; i < 8; i++) {
            Position next(current.pos.x + dx[i], current.pos.y + dy[i]);
            if (next.x < 0 || next.x >= board.width || next.y < 0 || next.y >= board.height) continue;

            // Check if cell is free. Diagonals also check if adjacent cardinal cells are blocked to avoid clipping
            bool isFree = board.isPositionFree(next, 1, 1);
            if (next == target) isFree = true; // Can move into target cell to attack

            if (isFree) {
                int tentative_g = g_score[get_key(current.pos)] + ((i < 4) ? 10 : 14); // 10 for cardinal, 14 for diagonal
                int nextKey = get_key(next);
                if (g_score.find(nextKey) == g_score.end() || tentative_g < g_score[nextKey]) {
                    came_from[nextKey] = current.pos;
                    g_score[nextKey] = tentative_g;
                    open_set.push({next, tentative_g, heuristic(next)});
                }
            }
        }
    }

    if (!(bestNode == position)) {
        Position step = bestNode;
        while (!(came_from[get_key(step)] == position)) {
            step = came_from[get_key(step)];
        }
        position = step;
    }
}

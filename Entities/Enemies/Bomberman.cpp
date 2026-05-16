#include "Entities/Enemies/Bomberman.h"
#include "Board.h"
#include "Entities/Units/Unit.h"
#include "Buildings/Wall.h"
#include <algorithm>
#include <climits>

// Bomberman représentation : 💣 (U+1F4A3)
// Vie élevée, dégâts massifs contre les murs, vitesse moyenne
Bomberman::Bomberman(const Position& pos, const std::string& name)
    : Enemy(pos, "\U0001F4A3", 10, 50, 4, 20, name) {
    // attackCooldownMax=20 pour équilibrer sa puissance destructrice
}

// Trouve le mur vivant le plus proche en distance de Manhattan
Building* Bomberman::findTargetWall(Board& board) {
    Building* target = nullptr;
    int bestDist = INT_MAX;
    for (auto* w : board.walls) {
        if (!w->isAlive()) continue;
        int d = std::abs(w->position.x - position.x)
              + std::abs(w->position.y - position.y);
        if (d < bestDist) {
            bestDist = d;
            target = w;
        }
    }
    return target;
}

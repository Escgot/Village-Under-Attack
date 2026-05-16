#include "Unit.h"
#include "../../Board.h"
#include "../../Entities/Enemies/Raider.h"
#include "../../Entities/Enemies/Bomberman.h"
#include <algorithm>
#include <climits>
#include <cmath>

Unit::Unit(const Position& pos, const std::string& repr, int health, int damage, int range, UnitType type, const std::string& name, int attackCooldownMax, int moveCooldownMax)
    : Npc(pos, repr, health), damage(damage), range(range), attackCooldown(0), attackCooldownMax(attackCooldownMax), 
      moveCooldown(0), moveCooldownMax(moveCooldownMax), type(type), name(name) {}

void Unit::update(Board& board) {
    attackCooldown = std::max(0, attackCooldown - 1);
    moveCooldown = std::max(0, moveCooldown - 1);

    // STRICT DEFENSE RULE 1: Find nearest enemy target (Raider or Bomberman)
    Npc* nearestEnemy = nullptr;
    int minDist = INT_MAX;

    // Cherche parmi les Raiders
    for (Raider* r : board.raiders) {
        if (!r->isAlive()) continue;
        int dist = std::abs(r->position.x - position.x) + std::abs(r->position.y - position.y);
        if (dist < minDist) {
            minDist = dist;
            nearestEnemy = r;
        }
    }

    // Cherche parmi les Bombermen
    for (Bomberman* bm : board.bombermen) {
        if (!bm->isAlive()) continue;
        int dist = std::abs(bm->position.x - position.x) + std::abs(bm->position.y - position.y);
        if (dist < minDist) {
            minDist = dist;
            nearestEnemy = bm;
        }
    }

    if (nearestEnemy && minDist <= range && attackCooldown == 0) {
        // STRICT DEFENSE RULE 2: Cooldown check before attack
        performAttack(*nearestEnemy);
        attackCooldown = attackCooldownMax;
    } else if (nearestEnemy && moveCooldown == 0) {
        moveToward(nearestEnemy->position, board);
        moveCooldown = moveCooldownMax;
    }
}

bool Unit::canAttack(const Npc& target) const {
    int dist = std::abs(target.position.x - position.x) + std::abs(target.position.y - position.y);
    return dist <= range;
}

void Unit::performAttack(Npc& target) {
    // STRICT DEFENSE RULE 3: Damage target (no friendly fire assumed)
    target.takeDamage(damage);
}

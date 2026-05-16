#include "Enemy.h"

Enemy::Enemy(const Position& pos, const std::string& repr, int health, int damage, int speed, int attackCooldownMax, const std::string& name)
    : Npc(pos, repr, health), attackDamage(damage), moveInterval(speed), moveTimer(0), attackCooldown(0), attackCooldownMax(attackCooldownMax), name(name) {}

void Enemy::moveToward(const Position& target, Board& board) {
    moveTimer++;
    if (moveTimer < moveInterval) return;
    moveTimer = 0;

    // Appelle le mouvement de base de Npc
    Npc::moveToward(target, board);
}

#pragma once
#include "Entities/Npc.h"

class Enemy : public Npc {
public:
    int attackDamage;
    int moveTimer;
    int moveInterval;
    int attackCooldown;
    int attackCooldownMax;
    std::string name;

    Enemy(const Position& pos, const std::string& repr, int health, int damage, int speed, int attackCooldownMax = 20, const std::string& name = "Enemy");
    virtual ~Enemy() = default;

    bool isRaider() const { return name.find("Raider") != std::string::npos || name.find("-") != std::string::npos; }

    // Override moveToward with timer-based movement
    void moveToward(const Position& target, Board& board) override;
};

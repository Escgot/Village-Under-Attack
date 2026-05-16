#pragma once
#include "../Npc.h"
#include "UnitType.h"

class Board;      // Forward declaration
class Bomberman;  // Forward declaration

class Unit : public Npc {
public:
    int damage;
    int range;
    int attackCooldown;
    int attackCooldownMax;
    int moveCooldown;
    int moveCooldownMax;
    UnitType type;
    std::string name;

    Unit(const Position& pos, const std::string& repr, int health, int damage, int range, UnitType type, const std::string& name = "Soldier", int attackCooldownMax = 20, int moveCooldownMax = 1);
    virtual ~Unit() = default;

    virtual void update(Board& board);
    virtual bool canAttack(const Npc& target) const;
    virtual void performAttack(Npc& target);
};

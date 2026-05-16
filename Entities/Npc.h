#pragma once
#include "Entity.h"

class Board;

class Npc : public Entity {
public:
    int health;
    int maxHealth;

    Npc(const Position& pos, const std::string& repr, int health);
    virtual ~Npc() = default;

    bool isAlive() const;
    void takeDamage(int dmg);

    // Intelligent movement using Board collision data
    virtual void moveToward(const Position& target, Board& board);
};

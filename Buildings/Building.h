#pragma once
#include <string>
#include "Position.h"

class Building {
public:
    int sizeX;
    int sizeY;
    std::string repr;
    int costGold;
    int costElixir;
    int maxInstances;
    Position position;
    int health;
    int maxHealth;
    int level = 1;
    float visualX, visualY;

    Building(int sizeX, int sizeY, const std::string& repr,
             int costGold, int costElixir, int maxInstances,
             const Position& pos, int health);

    virtual ~Building() = default;

    virtual bool upgrade(int& playerGold, int& playerElixir) { return false; }

    // Check if a position collides with this building
    bool collidesWithPosition(const Position& pos) const;

    // Check if another building collides with this building
    bool collidesWithBuilding(const Building& other) const;

    virtual std::string getRepr() const;

    bool isAlive() const;
    void takeDamage(int dmg);
    void updateVisuals(float dt);
};

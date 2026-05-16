#include "Building.h"

Building::Building(int sizeX, int sizeY, const std::string& repr,
                   int costGold, int costElixir, int maxInstances,
                   const Position& pos, int health)
    : sizeX(sizeX), sizeY(sizeY), repr(repr),
      costGold(costGold), costElixir(costElixir),
      maxInstances(maxInstances), position(pos),
      health(health), maxHealth(health) {}

bool Building::collidesWithPosition(const Position& pos) const {
    int halfX = sizeX / 2;
    int halfY = sizeY / 2;
    int minX = position.x - halfX;
    int maxX = position.x + halfX;
    int minY = position.y - halfY;
    int maxY = position.y + halfY;
    return (pos.x >= minX && pos.x <= maxX &&
            pos.y >= minY && pos.y <= maxY);
}

bool Building::collidesWithBuilding(const Building& other) const {
    int halfX = sizeX / 2;
    int halfY = sizeY / 2;
    int otherHalfX = other.sizeX / 2;
    int otherHalfY = other.sizeY / 2;

    int minX = position.x - halfX;
    int maxX = position.x + halfX;
    int minY = position.y - halfY;
    int maxY = position.y + halfY;

    int oMinX = other.position.x - otherHalfX;
    int oMaxX = other.position.x + otherHalfX;
    int oMinY = other.position.y - otherHalfY;
    int oMaxY = other.position.y + otherHalfY;

    return !(maxX < oMinX || oMaxX < minX || maxY < oMinY || oMaxY < minY);
}

std::string Building::getRepr() const {
    return repr;
}

bool Building::isAlive() const {
    return health > 0;
}

void Building::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
}

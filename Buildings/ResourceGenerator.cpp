#include "ResourceGenerator.h"

ResourceGenerator::ResourceGenerator(int sizeX, int sizeY, const std::string& repr,
                                     int costGold, int costElixir, int maxInstances,
                                     const Position& pos, int health, int capacity)
    : Building(sizeX, sizeY, repr, costGold, costElixir, maxInstances, pos, health),
      current(0), capacity(capacity) {}

bool ResourceGenerator::isFull() const {
    return current >= capacity;
}

void ResourceGenerator::generate(int amount) {
    current += amount;
    if (current > capacity) current = capacity;
}

int ResourceGenerator::collect() {
    int amount = current;
    current = 0;
    return amount;
}

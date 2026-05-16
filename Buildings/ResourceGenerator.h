#pragma once
#include "Building.h"

class ResourceGenerator : public Building {
public:
    int current;
    int capacity;

    ResourceGenerator(int sizeX, int sizeY, const std::string& repr,
                      int costGold, int costElixir, int maxInstances,
                      const Position& pos, int health, int capacity = 100);

    bool isFull() const;
    void generate(int amount);
    int collect(); // returns collected amount and resets to 0
};

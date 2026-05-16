#pragma once
#include "Buildings/Building.h"
#include "Entities/Units/UnitType.h"
#include <queue>
#include <memory>

class Unit; // Forward declaration

class Barracks : public Building {
public:
    static const int STATIC_COST_GOLD   = 150;
    static const int STATIC_COST_ELIXIR = 150;
    static const int STATIC_MAX_INSTANCES = 2;
    static const int STATIC_HEALTH = 400;

    // Training queue (max 3 units queued)
    std::queue<UnitType> trainQueue;
    int trainTimer;
    static const int TRAIN_INTERVAL = 60; // ticks per unit

    Barracks(const Position& pos);

    bool trainUnit(UnitType type);   // Enqueue a unit (returns false if queue full)
    bool isTraining() const;

    // Returns the repr: shows training progress if active
    std::string getRepr() const override;

    bool upgrade(int& playerGold, int& playerElixir) override;
};

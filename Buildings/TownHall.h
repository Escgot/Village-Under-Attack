#pragma once
#include "Building.h"

class TownHall : public Building {
public:
    static const int STATIC_COST_GOLD    = 0;
    static const int STATIC_COST_ELIXIR  = 0;
    static const int STATIC_MAX_INSTANCES = 1;
    static const int STATIC_HEALTH       = 50;

    TownHall(const Position& pos);
    bool upgrade(int& playerGold, int& playerElixir) override;
};

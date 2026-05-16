#pragma once
#include "Building.h"

class Wall : public Building {
public:
    static const int STATIC_COST_GOLD    = 10;
    static const int STATIC_COST_ELIXIR  = 0;
    static const int STATIC_MAX_INSTANCES = 200;
    static const int STATIC_HEALTH       = 20;

    Wall(const Position& pos);
};

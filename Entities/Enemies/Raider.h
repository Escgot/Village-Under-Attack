#pragma once
#include "Enemy.h"

class Raider : public Enemy {
public:
    Raider(const Position& pos, const std::string& name = "Raider");
};

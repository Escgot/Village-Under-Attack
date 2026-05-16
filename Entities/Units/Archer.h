#pragma once
#include "Unit.h"

class Archer : public Unit {
public:
    Archer(const Position& pos, const std::string& name = "Archer");
};


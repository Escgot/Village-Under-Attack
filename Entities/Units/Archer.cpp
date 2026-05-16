#include "Archer.h"

Archer::Archer(const Position& pos, const std::string& name)
    : Unit(pos, "A", 20, 10, 15, UnitType::ARCHER, name, 30, 3) {}


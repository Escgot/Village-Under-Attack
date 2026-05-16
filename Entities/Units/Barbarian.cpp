#include "Barbarian.h"

Barbarian::Barbarian(const Position& pos, const std::string& name)
    : Unit(pos, "R", 50, 15, 1, UnitType::BARBARIAN, name, 15, 1) {}


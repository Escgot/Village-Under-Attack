#include "Mage.h"

Mage::Mage(const Position& pos, const std::string& name)
    : Unit(pos, "M", 40, 30, 8, UnitType::MAGE, name, 50, 4) {} // HP: 40, Dmg: 30, Range: 8, Type: MAGE, Cooldowns: 50, 4

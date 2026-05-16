#include "Mage.h"

Mage::Mage(const Position& pos, const std::string& name)
    : Unit(pos, UnitType::MAGE, name, "M", 40, 50, 4) {} // HP: 40, AttackCooldown: 50, MoveCooldown: 4

#include "Raider.h"

// Raider repr: 😈 (U+1F608)
Raider::Raider(const Position& pos, const std::string& name)
    : Enemy(pos, "\U0001F608", 35, 5, 20, 20, name) {}

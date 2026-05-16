#include "Wall.h"

// Repr: 🧱 (U+1F9F1)
Wall::Wall(const Position& pos)
    : Building(1, 1, "\U0001F9F1", 10, 0, 200, pos, 20) {}

#include "GoldMine.h"

// Not full repr: ⛏ (U+26CF), Full repr: 🪙 (U+1FA99)
GoldMine::GoldMine(const Position& pos)
    : ResourceGenerator(3, 3, "\u26CF", 0, 100, 3, pos, 30) {}

std::string GoldMine::getRepr() const {
    return isFull() ? "\U0001FA99" : "\u26CF";
}

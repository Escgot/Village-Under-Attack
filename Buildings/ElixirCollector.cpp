#include "ElixirCollector.h"

// Not full: 🧪 (U+1F9EA), Full: 💧 (U+1F4A7) — using magic potion emojis
ElixirCollector::ElixirCollector(const Position& pos)
    : ResourceGenerator(3, 3, "\U0001F9EA", 100, 0, 3, pos, 30) {}

std::string ElixirCollector::getRepr() const {
    return isFull() ? "\U0001F4A7" : "\U0001F9EA";
}

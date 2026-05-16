#include "Buildings/Barracks.h"
#include <memory>

// Barracks repr: 🏚 (U+1F3DA) — dilapidated house = caserne
Barracks::Barracks(const Position& pos)
    : Building(3, 3,
               "\U0001F3DA",          // 🏚
               STATIC_COST_GOLD,
               STATIC_COST_ELIXIR,
               STATIC_MAX_INSTANCES,
               pos,
               STATIC_HEALTH),
      trainTimer(0) {}

bool Barracks::trainUnit(UnitType type) {
    if (type == UnitType::MAGE && level < 2) return false; // Unlock at level 2
    if (trainQueue.size() >= 3) return false;
    trainQueue.push(type);
    return true;
}

bool Barracks::isTraining() const {
    return !trainQueue.empty();
}

std::string Barracks::getRepr() const {
    if (isTraining()) return "\U0001F3E1\u2694";
    return repr;
}

bool Barracks::upgrade(int& playerGold, int& playerElixir) {
    if (level >= 2) return false;
    if (playerGold >= 300 && playerElixir >= 300) {
        playerGold -= 300;
        playerElixir -= 300;
        level = 2;
        maxHealth += 200;
        health += 200;
        return true;
    }
    return false;
}

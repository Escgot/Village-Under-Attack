#include "TownHall.h"

TownHall::TownHall(const Position& pos)
    : Building(5, 5, "\U0001F3DB", 0, 0, 1, pos, 100) {}

bool TownHall::upgrade(int& playerGold, int& playerElixir) {
    if (level >= 2) return false;
    if (playerGold >= 500) {
        playerGold -= 500;
        level = 2;
        maxHealth *= 2;
        health = maxHealth; // fully heal on upgrade
        return true;
    }
    return false;
}

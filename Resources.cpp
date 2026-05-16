#include "Resources.h"

Resources::Resources(int gold, int elixir) : gold(gold), elixir(elixir) {}

bool Resources::canAfford(int goldCost, int elixirCost) const {
    return gold >= goldCost && elixir >= elixirCost;
}

void Resources::add(int goldAmount, int elixirAmount) {
    gold += goldAmount;
    elixir += elixirAmount;
}

void Resources::consume(int goldAmount, int elixirAmount) {
    gold -= goldAmount;
    elixir -= elixirAmount;
}

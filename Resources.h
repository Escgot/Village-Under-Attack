#pragma once

class Resources {
public:
    int gold;
    int elixir;

    Resources(int gold = 0, int elixir = 0);

    bool canAfford(int goldCost, int elixirCost) const;
    void add(int goldAmount, int elixirAmount);
    void consume(int goldAmount, int elixirAmount);
};

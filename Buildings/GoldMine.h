#pragma once
#include "ResourceGenerator.h"

class GoldMine : public ResourceGenerator {
public:
    static const int STATIC_COST_GOLD    = 0;
    static const int STATIC_COST_ELIXIR  = 100;
    static const int STATIC_MAX_INSTANCES = 3;

    GoldMine(const Position& pos);

    std::string getRepr() const override;
};

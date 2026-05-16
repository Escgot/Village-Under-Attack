#pragma once
#include "ResourceGenerator.h"

class ElixirCollector : public ResourceGenerator {
public:
    static const int STATIC_COST_GOLD    = 100;
    static const int STATIC_COST_ELIXIR  = 0;
    static const int STATIC_MAX_INSTANCES = 3;

    ElixirCollector(const Position& pos);

    std::string getRepr() const override;
};

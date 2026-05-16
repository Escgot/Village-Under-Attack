#pragma once
#include <string>
#include "Position.h"

class Entity {
public:
    Position position;
    std::string repr;

    Entity(const Position& pos, const std::string& repr);
    virtual ~Entity() = default;

    virtual std::string getRepr() const;
};

#pragma once
#include <string>
#include "Position.h"

class Entity {
public:
    Position position;
    float visualX, visualY; // Interpolated positions for smooth rendering
    std::string repr;

    Entity(const Position& pos, const std::string& repr);
    virtual ~Entity() = default;

    virtual std::string getRepr() const;
    void updateVisuals(float dt); // Lerp visual position to grid position
};

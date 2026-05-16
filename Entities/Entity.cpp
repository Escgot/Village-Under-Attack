#include "Entity.h"

Entity::Entity(const Position& pos, const std::string& repr)
    : position(pos), visualX((float)pos.x), visualY((float)pos.y), repr(repr) {}

std::string Entity::getRepr() const {
    return repr;
}

void Entity::updateVisuals(float dt) {
    // Smoother interpolation (Lerp) toward the target grid position
    // Speed factor 10.0f can be adjusted for faster/slower sliding
    float lerpSpeed = 10.0f;
    visualX += ((float)position.x - visualX) * lerpSpeed * dt;
    visualY += ((float)position.y - visualY) * lerpSpeed * dt;
}

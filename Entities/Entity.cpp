#include "Entity.h"

Entity::Entity(const Position& pos, const std::string& repr)
    : position(pos), repr(repr) {}

std::string Entity::getRepr() const {
    return repr;
}

#pragma once
#include "Unit.h"

class Barbarian : public Unit {
public:
    Barbarian(const Position& pos, const std::string& name = "Barbarian");
};


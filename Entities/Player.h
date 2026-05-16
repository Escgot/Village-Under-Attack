#pragma once
#include "Entity.h"
#include "Resources.h"

class Player : public Entity {
public:
    Resources resources;

    Player(const Position& pos);

    void move(int dx, int dy, int boardW, int boardH);
};

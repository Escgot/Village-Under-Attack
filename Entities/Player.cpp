#include "Player.h"

// Player repr: 🧑 (U+1F9D1)
Player::Player(const Position& pos)
    : Entity(pos, "\U0001F9D1"), resources(400, 400) {}

void Player::move(int dx, int dy, int boardW, int boardH) {
    int nx = position.x + dx;
    int ny = position.y + dy;
    if (nx >= 0 && nx < boardW) position.x = nx;
    if (ny >= 0 && ny < boardH) position.y = ny;
}

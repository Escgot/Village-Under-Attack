#pragma once
#include "Enemy.h"

class Building; // forward declaration
class Board;    // forward declaration

// Bomberman (Ennemi) — Détruit les murs
class Bomberman : public Enemy {
public:
    Bomberman(const Position& pos, const std::string& name = "Bomber");

    // Recherche le mur le plus proche pour l'attaquer
    Building* findTargetWall(Board& board);
};


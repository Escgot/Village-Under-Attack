#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Position.h"
#include "Buildings/Building.h"
#include "Buildings/TownHall.h"
#include "Buildings/Wall.h"
#include "Buildings/GoldMine.h"
#include "Buildings/ElixirCollector.h"
#include "Buildings/ResourceGenerator.h"
#include "Entities/Player.h"
#include "Entities/Enemies/Raider.h"
#include "Entities/Enemies/Bomberman.h"
#include "Buildings/Barracks.h"
#include "Entities/Units/Unit.h"
#include "WaveManager.h"

class Board {
public:
    int width;
    int height;

    std::unique_ptr<TownHall>               townHall;
    std::vector<std::unique_ptr<Wall>>      walls;
    std::vector<std::unique_ptr<GoldMine>>  goldMines;
    std::vector<std::unique_ptr<ElixirCollector>> elixirCollectors;
    std::vector<std::unique_ptr<Raider>>    raiders;
    std::vector<std::unique_ptr<Bomberman>> bombermen;
    std::unique_ptr<Player>                 player;
    std::vector<std::unique_ptr<Barracks>>  barracks;
    std::vector<std::unique_ptr<Unit>>      defenders;

    int score;
    int tick;
    int raiderspawnInterval;
    int resourceGenInterval;
    bool gameOver;
    bool victory;

    // Wave System
    WaveManager waveManager;

    std::string lastMessage;

    Board(int width, int height);
    ~Board();

    bool isPositionFree(const Position& pos, int sizeX, int sizeY, Building* ignore = nullptr) const;

    bool buildWall(const Position& pos);
    bool buildGoldMine(const Position& pos);
    bool buildElixirCollector(const Position& pos);
    void collectResources();

    bool buildBarracks(const Position& pos);
    bool trainInBarracks(UnitType type);  // NEW: train unit in nearest barracks
    bool upgradeBuilding(const Position& pos);

    void update();
    void updateVisuals(float dt);
    void render() const;

    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);

    std::string getPanelLine(int lineIndex) const;

    void spawnRaider();
    void spawnBomberman();

private:
    void updateEnemies();
    void generateResources();
    void updateBarracks();
    void updateDefenders();
    void cleanDeadDefenders();
    void cleanDeadBuildings();
    std::string getCellContent(int x, int y) const;
    // Returns a 2-column-wide cell string (pads single-char with a space)
    std::string padCell(const std::string& s) const;
};

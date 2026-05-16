#include "Board.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <cmath>
#include <iomanip>
#include "Entities/Units/Archer.h"
#include "Entities/Units/Barbarian.h"
#include "Entities/Units/Mage.h"

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define WHITE   "\033[37m"
#define DIM     "\033[2m"
#define BG_DARK "\033[48;5;235m"
#define BG_RESET "\033[49m"

Board::Board(int width, int height)
    : width(width), height(height),
      score(0), tick(0),
      raiderspawnInterval(80),
      resourceGenInterval(10),
      gameOver(false), victory(false),
      lastMessage("Bienvenue ! Deplacez-vous et construisez.")
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    townHall = new TownHall(Position(width / 2, height / 2));
    player   = new Player(Position(width / 2 + 5, height / 2 + 5));
}

Board::~Board() {
    delete townHall;
    delete player;
    for (auto* w : walls)            delete w;
    for (auto* g : goldMines)        delete g;
    for (auto* e : elixirCollectors) delete e;
    for (auto* r : raiders)          delete r;
    for (auto* b : barracks)         delete b;
    for (auto* u : defenders)        delete u;
    for (auto* bm : bombermen)       delete bm;
}

bool Board::isPositionFree(const Position& pos, int sizeX, int sizeY, Building* ignore) const {
    auto overlaps = [&](Building* b) -> bool {
        if (b == ignore) return false;
        int halfX  = sizeX / 2,       halfY  = sizeY / 2;
        int oHalfX = b->sizeX / 2,    oHalfY = b->sizeY / 2;
        int minX  = pos.x - halfX,    maxX  = pos.x + halfX;
        int minY  = pos.y - halfY,    maxY  = pos.y + halfY;
        int oMinX = b->position.x - oHalfX, oMaxX = b->position.x + oHalfX;
        int oMinY = b->position.y - oHalfY, oMaxY = b->position.y + oHalfY;
        return !(maxX < oMinX || oMaxX < minX || maxY < oMinY || oMaxY < minY);
    };
    if (overlaps(townHall)) return false;
    for (auto* w : walls)            if (overlaps(w)) return false;
    for (auto* g : goldMines)        if (overlaps(g)) return false;
    for (auto* e : elixirCollectors) if (overlaps(e)) return false;
    for (auto* b : barracks)         if (overlaps(b)) return false;
    int halfX = sizeX / 2, halfY = sizeY / 2;
    if (pos.x - halfX < 0 || pos.x + halfX >= width)  return false;
    if (pos.y - halfY < 0 || pos.y + halfY >= height) return false;
    return true;
}

bool Board::buildWall(const Position& pos) {
    if ((int)walls.size() >= Wall::STATIC_MAX_INSTANCES) {
        lastMessage = "ERREUR: Maximum de murs atteint (200) !"; return false;
    }
    if (!player->resources.canAfford(Wall::STATIC_COST_GOLD, Wall::STATIC_COST_ELIXIR)) {
        lastMessage = "ERREUR: Pas assez d'or ! (besoin: 10 or)"; return false;
    }
    if (!isPositionFree(pos, 1, 1)) {
        lastMessage = "ERREUR: Position occupee ou hors limites !"; return false;
    }
    walls.push_back(new Wall(pos));
    player->resources.consume(Wall::STATIC_COST_GOLD, Wall::STATIC_COST_ELIXIR);
    score += 10;
    lastMessage = "OK: Mur construit ! (-10 or, +10 score)";
    return true;
}

bool Board::buildGoldMine(const Position& pos) {
    if ((int)goldMines.size() >= GoldMine::STATIC_MAX_INSTANCES) {
        lastMessage = "ERREUR: Maximum de mines atteint (3) !"; return false;
    }
    if (!player->resources.canAfford(GoldMine::STATIC_COST_GOLD, GoldMine::STATIC_COST_ELIXIR)) {
        lastMessage = "ERREUR: Pas assez d'elixir ! (besoin: 100 elixir)"; return false;
    }
    if (!isPositionFree(pos, 3, 3)) {
        lastMessage = "ERREUR: Position occupee ! Eloignez-vous du TownHall."; return false;
    }
    goldMines.push_back(new GoldMine(pos));
    player->resources.consume(GoldMine::STATIC_COST_GOLD, GoldMine::STATIC_COST_ELIXIR);
    score += 10;
    lastMessage = "OK: GoldMine construite ! (-100 elixir, +10 score)";
    return true;
}

bool Board::buildElixirCollector(const Position& pos) {
    if ((int)elixirCollectors.size() >= ElixirCollector::STATIC_MAX_INSTANCES) {
        lastMessage = "ERREUR: Maximum de collectors atteint (3) !"; return false;
    }
    if (!player->resources.canAfford(ElixirCollector::STATIC_COST_GOLD, ElixirCollector::STATIC_COST_ELIXIR)) {
        lastMessage = "ERREUR: Pas assez d'or ! (besoin: 100 or)"; return false;
    }
    if (!isPositionFree(pos, 3, 3)) {
        lastMessage = "ERREUR: Position occupee ! Eloignez-vous du TownHall."; return false;
    }
    elixirCollectors.push_back(new ElixirCollector(pos));
    player->resources.consume(ElixirCollector::STATIC_COST_GOLD, ElixirCollector::STATIC_COST_ELIXIR);
    score += 10;
    lastMessage = "OK: ElixirCollector construit ! (-100 or, +10 score)";
    return true;
}

bool Board::buildBarracks(const Position& pos) {
    if ((int)barracks.size() >= Barracks::STATIC_MAX_INSTANCES) {
        lastMessage = "ERREUR: Maximum de casernes atteint (2) !"; return false;
    }
    if (!player->resources.canAfford(Barracks::STATIC_COST_GOLD, Barracks::STATIC_COST_ELIXIR)) {
        lastMessage = "ERREUR: Pas assez de ressources ! (besoin: 150 or + 150 elixir)"; return false;
    }
    if (!isPositionFree(pos, 3, 3)) {
        lastMessage = "ERREUR: Position occupee !"; return false;
    }
    barracks.push_back(new Barracks(pos));
    player->resources.consume(Barracks::STATIC_COST_GOLD, Barracks::STATIC_COST_ELIXIR);
    score += 20;
    lastMessage = "OK: Caserne construite ! [A]=Archer(50 elixir) [H]=Barbare(30 elixir)";
    return true;
}

// NEW: Train a unit — player must be standing on/near a barracks
bool Board::trainInBarracks(UnitType type) {
    // Find a barracks the player is standing on
    Barracks* target = nullptr;
    for (auto* b : barracks) {
        if (b->collidesWithPosition(player->position)) {
            target = b;
            break;
        }
    }
    if (!target) {
        lastMessage = "INFO: Allez sur une Caserne pour entrainer des troupes !";
        return false;
    }
    if ((int)target->trainQueue.size() >= 3) {
        lastMessage = "ERREUR: File d'entrainement pleine (max 3) !";
        return false;
    }
    int costElixir = 30;
    std::string name = "Barbare";
    if (type == UnitType::ARCHER) { costElixir = 50; name = "Archer"; }
    else if (type == UnitType::MAGE) { costElixir = 150; name = "Mage"; }

    if (!player->resources.canAfford(0, costElixir)) {
        lastMessage = "ERREUR: Pas assez d'elixir ! (" + name + ": " + std::to_string(costElixir) + ")";
        return false;
    }
    
    if (!target->trainUnit(type)) {
        lastMessage = "ERREUR: File pleine ou Batiment doit etre ameliore (Niveau 2 requis) !";
        return false;
    }

    player->resources.consume(0, costElixir);
    lastMessage = "OK: " + name + " en entrainement (-" + std::to_string(costElixir) + " elixir, " +
                  std::to_string(60 * ((int)target->trainQueue.size())) + " ticks)";
    return true;
}

void Board::collectResources() {
    bool collected = false;
    for (auto* g : goldMines) {
        if (g->collidesWithPosition(player->position)) {
            int amt = g->collect();
            if (amt > 0) {
                player->resources.add(amt, 0);
                score += amt;
                lastMessage = "OK: +" + std::to_string(amt) + " or collecte !";
                collected = true;
            } else {
                lastMessage = "INFO: La mine est vide, attendez qu'elle se remplisse.";
                collected = true;
            }
        }
    }
    for (auto* e : elixirCollectors) {
        if (e->collidesWithPosition(player->position)) {
            int amt = e->collect();
            if (amt > 0) {
                player->resources.add(0, amt);
                score += amt;
                lastMessage = "OK: +" + std::to_string(amt) + " elixir collecte !";
                collected = true;
            } else {
                lastMessage = "INFO: Le collector est vide, attendez.";
                collected = true;
            }
        }
    }
    if (!collected) {
        lastMessage = "INFO: Allez sur une GoldMine ou ElixirCollector pour collecter !";
    }
}

bool Board::upgradeBuilding(const Position& pos) {
    auto tryUpgrade = [&](Building* b) {
        if (b && b->collidesWithPosition(pos)) {
            if (b->upgrade(player->resources.gold, player->resources.elixir)) {
                lastMessage = "SUCCES: Batiment ameliore au niveau " + std::to_string(b->level) + " !";
                return true;
            } else {
                lastMessage = "ECHEC: Pas assez de ressources ou niveau max atteint.";
                return true; // We found the building but couldn't upgrade
            }
        }
        return false;
    };

    if (tryUpgrade(townHall)) return true;
    for (auto* b : barracks) if (tryUpgrade(b)) return true;
    for (auto* g : goldMines) if (tryUpgrade(g)) return true;
    for (auto* e : elixirCollectors) if (tryUpgrade(e)) return true;
    
    lastMessage = "INFO: Aucun batiment ameliorable ici.";
    return false;
}

void Board::update() {
    if (gameOver) return;
    tick++;
    waveManager.update(*this);

    if (tick % resourceGenInterval == 0) generateResources();
    if (!waveManager.isWaveActive) {
        if (tick % raiderspawnInterval  == 0) spawnRaider();
        if (tick % (raiderspawnInterval * 3) == 0) spawnBomberman();
    }
    
    updateEnemies();
    updateBarracks();
    updateDefenders();
    cleanDeadDefenders();
    cleanDeadBuildings();
    if (!townHall->isAlive()) gameOver = true;
}

void Board::cleanDeadBuildings() {
    walls.erase(std::remove_if(walls.begin(), walls.end(), [](Wall* w){ if(!w->isAlive()){ delete w; return true; } return false; }), walls.end());
    goldMines.erase(std::remove_if(goldMines.begin(), goldMines.end(), [](GoldMine* g){ if(!g->isAlive()){ delete g; return true; } return false; }), goldMines.end());
    elixirCollectors.erase(std::remove_if(elixirCollectors.begin(), elixirCollectors.end(), [](ElixirCollector* e){ if(!e->isAlive()){ delete e; return true; } return false; }), elixirCollectors.end());
    barracks.erase(std::remove_if(barracks.begin(), barracks.end(), [](Barracks* b){ if(!b->isAlive()){ delete b; return true; } return false; }), barracks.end());
}

void Board::generateResources() {
    for (auto* g : goldMines)        g->generate(5);
    for (auto* e : elixirCollectors) e->generate(5);
}

void Board::spawnRaider() {
    static const std::vector<std::string> names = {"Skull-Crusher", "Blade-Master", "Night-Stalker", "Blood-Seeker", "Iron-Jaw"};
    Position spawnPos;
    int side = std::rand() % 4;
    switch(side) {
        case 0: spawnPos = {std::rand() % width, 0};           break;
        case 1: spawnPos = {std::rand() % width, height - 1};  break;
        case 2: spawnPos = {0, std::rand() % height};          break;
        default: spawnPos = {width - 1, std::rand() % height}; break;
    }
    std::string name = names[std::rand() % names.size()];
    raiders.push_back(new Raider(spawnPos, name));
}

void Board::spawnBomberman() {
    static const std::vector<std::string> names = {"Boom-Boy", "Wall-Breaker", "Fuse-Lighter", "Blast-Radius", "Powder-Keg"};
    Position spawnPos;
    int side = std::rand() % 4;
    switch(side) {
        case 0: spawnPos = {std::rand() % width, 0};           break;
        case 1: spawnPos = {std::rand() % width, height - 1};  break;
        case 2: spawnPos = {0, std::rand() % height};          break;
        default: spawnPos = {width - 1, std::rand() % height}; break;
    }
    std::string name = names[std::rand() % names.size()];
    bombermen.push_back(new Bomberman(spawnPos, name));
    lastMessage = "ALERTE: " + name + " est apparu ! Il detruit les murs !";
}

void Board::updateEnemies() {
    // Raiders: Reactive AI
    for (auto* r : raiders) {
        if (!r->isAlive()) continue;
        r->attackCooldown = std::max(0, r->attackCooldown - 1);

        // React to Defenders first
        Unit* closestDef = nullptr;
        int defDist = INT_MAX;
        for (auto* u : defenders) {
            if (!u->isAlive()) continue;
            int d = std::abs(u->position.x - r->position.x) + std::abs(u->position.y - r->position.y);
            if (d < defDist) { defDist = d; closestDef = u; }
        }

        if (closestDef && defDist <= 4) { // Aggro range for defenders
            if (defDist <= 1) {
                if (r->attackCooldown == 0) {
                    int dmg = (closestDef->type == UnitType::BARBARIAN) ? 15 : 7;
                    closestDef->takeDamage(dmg);
                    r->attackCooldown = r->attackCooldownMax;
                }
            } else {
                r->moveToward(closestDef->position, *this);
            }
        } else {
            // Target nearest building
            Building* target = townHall;
            int bDist = std::abs(townHall->position.x - r->position.x) + std::abs(townHall->position.y - r->position.y);
            
            for (auto* m : goldMines) {
                int d = std::abs(m->position.x - r->position.x) + std::abs(m->position.y - r->position.y);
                if (d < bDist) { bDist = d; target = m; }
            }
            for (auto* e : elixirCollectors) {
                int d = std::abs(e->position.x - r->position.x) + std::abs(e->position.y - r->position.y);
                if (d < bDist) { bDist = d; target = e; }
            }

            if (bDist <= 1 || target->collidesWithPosition(r->position)) {
                if (r->attackCooldown == 0) {
                    int dmg = 3; // Mines/Collectors
                    if (target == townHall) dmg = 5;
                    else if (std::find(walls.begin(), walls.end(), target) != walls.end()) dmg = 1;
                    target->takeDamage(dmg);
                    r->attackCooldown = r->attackCooldownMax;
                }
            } else {
                r->moveToward(target->position, *this);
            }
        }
    }

    // Bomberman: Priority Walls, then Reactive
    for (auto* bm : bombermen) {
        if (!bm->isAlive()) continue;
        bm->attackCooldown = std::max(0, bm->attackCooldown - 1);

        // Check if engaged by defender
        Unit* closestDef = nullptr;
        int defDist = INT_MAX;
        for (auto* u : defenders) {
            if (!u->isAlive()) continue;
            int d = std::abs(u->position.x - bm->position.x) + std::abs(u->position.y - bm->position.y);
            if (d < defDist) { defDist = d; closestDef = u; }
        }

        if (closestDef && defDist <= 1) { // Engaged
            if (bm->attackCooldown == 0) {
                closestDef->takeDamage(10);
                bm->attackCooldown = bm->attackCooldownMax;
            }
        } else {
            Building* target = nullptr;
            if (!walls.empty()) target = bm->findTargetWall(*this);
            if (!target) target = townHall; // Fallback to TownHall

            int dist = std::abs(target->position.x - bm->position.x) + std::abs(target->position.y - bm->position.y);
            if (dist <= 1 || target->collidesWithPosition(bm->position)) {
                if (bm->attackCooldown == 0) {
                    int dmg = 1;
                    if (target == townHall) dmg = 2;
                    else if (std::find(walls.begin(), walls.end(), target) != walls.end()) dmg = 50;
                    target->takeDamage(dmg);
                    bm->attackCooldown = bm->attackCooldownMax;
                }
            } else {
                bm->moveToward(target->position, *this);
            }
        }
    }

    // Cleanup
    raiders.erase(std::remove_if(raiders.begin(), raiders.end(), [](Raider* r){ if(!r->isAlive()){ delete r; return true; } return false; }), raiders.end());
    bombermen.erase(std::remove_if(bombermen.begin(), bombermen.end(), [](Bomberman* bm){ if(!bm->isAlive()){ delete bm; return true; } return false; }), bombermen.end());
}

void Board::cleanDeadDefenders() {
    defenders.erase(std::remove_if(defenders.begin(), defenders.end(), [](Unit* u){ if(!u->isAlive()){ delete u; return true; } return false; }), defenders.end());
}

void Board::updateDefenders() {
    for (auto* u : defenders) {
        if (!u->isAlive()) continue;
        u->attackCooldown = std::max(0, u->attackCooldown - 1);
        u->moveCooldown = std::max(0, u->moveCooldown - 1);

        Enemy* target = nullptr;
        int tDist = INT_MAX;
        int range = 1;
        if (u->type == UnitType::ARCHER) range = 5;
        else if (u->type == UnitType::MAGE) range = 8;

        auto checkEnemy = [&](Enemy* e) {
            int d = std::abs(e->position.x - u->position.x) + std::abs(e->position.y - u->position.y);
            if (d < tDist) { tDist = d; target = e; }
        };
        for (auto* r : raiders) checkEnemy(r);
        for (auto* bm : bombermen) checkEnemy(bm);

        if (target && tDist <= range) {
            if (u->attackCooldown == 0) {
                int dmg = 0;
                if (u->type == UnitType::ARCHER) {
                    dmg = 10; // vs both
                } else if (u->type == UnitType::MAGE) {
                    dmg = 30; // High magic damage
                } else {
                    dmg = (target->isRaider()) ? 15 : 7;
                }
                target->takeDamage(dmg);
                u->attackCooldown = u->attackCooldownMax;
            }
        } else if (target) {
            if (u->moveCooldown == 0) {
                u->moveToward(target->position, *this);
                u->moveCooldown = u->moveCooldownMax;
            }
        }
    }
}

void Board::updateBarracks() {
    static const std::vector<std::string> archerNames = {"Lyra", "Kael", "Arwen", "Robin", "Legolas", "Sylvanas", "Vane", "Artemis"};
    static const std::vector<std::string> barbNames = {"Magnus", "Throg", "Grog", "Conan", "Guts", "Bjorn", "Ragnar", "Ulf"};
    static const std::vector<std::string> mageNames = {"Gandalf", "Merlin", "Jaina", "Medivh", "Khadgar", "Geralt", "Yennefer", "Triss"};

    for (auto* b : barracks) {
        if (!b->trainQueue.empty()) {
            b->trainTimer++;
            if (b->trainTimer >= Barracks::TRAIN_INTERVAL) {
                UnitType type = b->trainQueue.front();
                Unit* u = nullptr;
                Position spawnPos = b->position;
                spawnPos.x += (std::rand() % 3) - 1;
                spawnPos.y += (std::rand() % 3) - 1;
                if (spawnPos.x < 0 || spawnPos.x >= width)  spawnPos.x = b->position.x;
                if (spawnPos.y < 0 || spawnPos.y >= height) spawnPos.y = b->position.y;

                std::string uName;
                if (type == UnitType::ARCHER) {
                    uName = archerNames[std::rand() % archerNames.size()];
                    u = new Archer(spawnPos, uName);
                } else if (type == UnitType::MAGE) {
                    uName = mageNames[std::rand() % mageNames.size()];
                    u = new Mage(spawnPos, uName);
                } else {
                    uName = barbNames[std::rand() % barbNames.size()];
                    u = new Barbarian(spawnPos, uName);
                }

                if (u) {
                    defenders.push_back(u);
                    b->trainQueue.pop();
                    b->trainTimer = 0;
                    lastMessage = "OK: " + uName + " pret au combat !";
                }
            }
        }
    }
}

// ── Affichage ─────────────────────────────────────────────────────────────────

// padCell: ensures the visual output is always exactly 2 terminal columns wide.
// Emojis (4-byte UTF-8) are 2 columns wide -> return as-is.
// Single ASCII chars (like 'A','R','B') are 1 column -> append a space.
// Empty cell -> two spaces.
std::string Board::padCell(const std::string& s) const {
    if (s.empty()) return "  ";
    // Detect multi-byte (likely emoji = 2 wide) vs single ASCII char
    unsigned char first = static_cast<unsigned char>(s[0]);
    if (first < 0x80) {
        // Pure ASCII: single char occupies 1 column -> pad with space
        return s + " ";
    }
    // Multi-byte UTF-8: emoji = 2 terminal columns -> return as-is
    return s;
}

std::string Board::getCellContent(int x, int y) const {
    Position pos(x, y);

    // Player
    if (player->position == pos)
        return player->getRepr(); // emoji -> 2 wide OK

    // Enemies
    for (auto* r : raiders)
        if (r->position == pos)  return r->getRepr();
    for (auto* bm : bombermen)
        if (bm->position == pos) return bm->getRepr();

    // Defenders (Archer 'A', Barbarian 'B' are single chars -> padded in render)
    for (auto* u : defenders)
        if (u->position == pos)  return u->getRepr();

    // Buildings (centres)
    if (townHall->position == pos) return townHall->getRepr();
    for (auto* g : goldMines)
        if (g->position == pos)    return g->getRepr();
    for (auto* e : elixirCollectors)
        if (e->position == pos)    return e->getRepr();
    for (auto* b : barracks)
        if (b->position == pos)    return b->getRepr(); // now emoji 🏚
    for (auto* w : walls)
        if (w->position == pos)    return w->getRepr();

    // Building body tiles (non-centre occupied cells)
    if (townHall->collidesWithPosition(pos))           return "\033[48;5;236m \033[0m ";
    for (auto* g : goldMines)
        if (g->collidesWithPosition(pos))              return "\033[48;5;236m \033[0m ";
    for (auto* e : elixirCollectors)
        if (e->collidesWithPosition(pos))              return "\033[48;5;236m \033[0m ";
    for (auto* b : barracks)
        if (b->collidesWithPosition(pos))              return "\033[48;5;236m \033[0m ";

    return ""; // empty cell
}

std::string Board::getPanelLine(int lineIndex) const {
    std::ostringstream oss;

    // Count total queued units across all barracks
    int totalQueued = 0;
    int trainingProgress = 0;
    bool anyTraining = false;
    for (auto* b : barracks) {
        totalQueued += (int)b->trainQueue.size();
        if (!b->trainQueue.empty()) {
            anyTraining = true;
            trainingProgress = b->trainTimer;
        }
    }

    switch(lineIndex) {
        case 0:  oss << BOLD << WHITE << "=== Village Under Attack ===" << RESET; break;
        case 1:  oss << DIM  << "Tick: " << tick << "   Score: " << score << RESET; break;
        case 2:  oss << " "; break;
        case 3:  oss << BOLD << YELLOW  << "Or:     " << RESET << YELLOW  << player->resources.gold   << RESET; break;
        case 4:  oss << BOLD << MAGENTA << "Elixir: " << RESET << MAGENTA << player->resources.elixir << RESET; break;
        case 5:  oss << " "; break;
        case 6: {
            int hp = townHall->health, mhp = townHall->maxHealth;
            int pct = (mhp > 0) ? (hp * 100 / mhp) : 0;
            const char* col = (pct > 50) ? GREEN : (pct > 20 ? YELLOW : RED);
            oss << BOLD << WHITE << "TownHall: " << RESET << col << hp << "/" << mhp << RESET;
            break;
        }
        case 7:  oss << " "; break;
        case 8:  oss << DIM << WHITE << "-- Entites --" << RESET; break;
        case 9:  oss << RED     << std::setw(14) << std::left << "  Raiders:"   << raiders.size()   << RESET; break;
        case 10: oss << YELLOW  << std::setw(14) << std::left << "  Bombermen:" << bombermen.size() << RESET; break;
        case 11: oss << CYAN    << std::setw(14) << std::left << "  Murs:"      << walls.size()     << RESET; break;
        case 12: oss << GREEN   << std::setw(14) << std::left << "  Defenseurs:" << defenders.size() << RESET; break;
        case 13: oss << MAGENTA << std::setw(14) << std::left << "  Casernes:"  << barracks.size()  << RESET; break;
        case 14: {
            if (anyTraining) {
                oss << CYAN << "  En entrainement: " << totalQueued
                    << " (" << trainingProgress << "/" << Barracks::TRAIN_INTERVAL << " ticks)" << RESET;
            } else {
                oss << DIM << "  Pas de troupes en formation" << RESET;
            }
            break;
        }
        case 15: oss << " "; break;
        case 16: oss << DIM << "-- Construire --" << RESET; break;
        case 17: oss << DIM << "[G] GoldMine    (100 elixir)" << RESET; break;
        case 18: oss << DIM << "[E] ElixirColl. (100 or)"     << RESET; break;
        case 19: oss << DIM << "[W] Mur         (10 or)"      << RESET; break;
        case 20: oss << DIM << "[B] Caserne     (150 or+elix)" << RESET; break;
        case 21: oss << " "; break;
        case 22: oss << DIM << "-- Sur une Caserne --" << RESET; break;
        case 23: oss << DIM << "[A] Entrainer Archer  (50 elix)" << RESET; break;
        case 24: oss << DIM << "[H] Entrainer Barbare (30 elix)" << RESET; break;
        case 25: oss << " "; break;
        case 26: oss << DIM << "[C] Collecter (sur mine)"     << RESET; break;
        case 27: oss << DIM << "[fleches] Deplacer"           << RESET; break;
        case 28: oss << DIM << "[Q] Quitter"                  << RESET; break;
        default: break;
    }
    return oss.str();
}

#include <fstream>

bool Board::saveGame(const std::string& filename) {
    std::ofstream ofs(filename);
    if (!ofs) return false;

    // Global Stats
    ofs << "STATS " << score << " " << tick << " " << (int)waveManager.waveCount << "\n";
    
    // Player
    ofs << "PLAYER " << player->position.x << " " << player->position.y << " " 
        << player->resources.gold << " " << player->resources.elixir << "\n";

    // TownHall
    ofs << "TOWNHALL " << townHall->position.x << " " << townHall->position.y << " "
        << townHall->health << " " << townHall->level << "\n";

    // Buildings
    for (auto* w : walls) ofs << "WALL " << w->position.x << " " << w->position.y << " " << w->health << "\n";
    for (auto* g : goldMines) ofs << "GOLDMINE " << g->position.x << " " << g->position.y << " " << g->health << " " << g->level << "\n";
    for (auto* e : elixirCollectors) ofs << "ELIXIR " << e->position.x << " " << e->position.y << " " << e->health << " " << e->level << "\n";
    for (auto* b : barracks) ofs << "BARRACKS " << b->position.x << " " << b->position.y << " " << b->health << " " << b->level << "\n";

    // Units
    for (auto* u : defenders) {
        std::string type = "BARBARIAN";
        if (u->type == UnitType::ARCHER) type = "ARCHER";
        else if (u->type == UnitType::MAGE) type = "MAGE";
        ofs << "DEFENDER " << type << " " << u->position.x << " " << u->position.y << " " << u->health << " " << u->name << "\n";
    }
    for (auto* r : raiders) ofs << "RAIDER " << r->position.x << " " << r->position.y << " " << r->health << " " << r->name << "\n";
    for (auto* bm : bombermen) ofs << "BOMBERMAN " << bm->position.x << " " << bm->position.y << " " << bm->health << " " << bm->name << "\n";

    lastMessage = "OK: Partie sauvgardee dans " + filename;
    return true;
}

bool Board::loadGame(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        lastMessage = "ERREUR: Impossible d'ouvrir " + filename;
        return false;
    }

    // Clear current state
    for (auto* w : walls) delete w;            walls.clear();
    for (auto* g : goldMines) delete g;        goldMines.clear();
    for (auto* e : elixirCollectors) delete e; elixirCollectors.clear();
    for (auto* b : barracks) delete b;         barracks.clear();
    for (auto* r : raiders) delete r;          raiders.clear();
    for (auto* bm : bombermen) delete bm;       bombermen.clear();
    for (auto* u : defenders) delete u;        defenders.clear();

    std::string line;
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        std::string tag;
        ss >> tag;

        if (tag == "STATS") {
            int wave;
            ss >> score >> tick >> wave;
            waveManager.waveCount = wave;
        } else if (tag == "PLAYER") {
            ss >> player->position.x >> player->position.y >> player->resources.gold >> player->resources.elixir;
        } else if (tag == "TOWNHALL") {
            ss >> townHall->position.x >> townHall->position.y >> townHall->health >> townHall->level;
        } else if (tag == "WALL") {
            Position p; int hp;
            ss >> p.x >> p.y >> hp;
            auto* w = new Wall(p); w->health = hp;
            walls.push_back(w);
        } else if (tag == "GOLDMINE") {
            Position p; int hp, lvl;
            ss >> p.x >> p.y >> hp >> lvl;
            auto* g = new GoldMine(p); g->health = hp; g->level = lvl;
            goldMines.push_back(g);
        } else if (tag == "ELIXIR") {
            Position p; int hp, lvl;
            ss >> p.x >> p.y >> hp >> lvl;
            auto* e = new ElixirCollector(p); e->health = hp; e->level = lvl;
            elixirCollectors.push_back(e);
        } else if (tag == "BARRACKS") {
            Position p; int hp, lvl;
            ss >> p.x >> p.y >> hp >> lvl;
            auto* b = new Barracks(p); b->health = hp; b->level = lvl;
            barracks.push_back(b);
        } else if (tag == "DEFENDER") {
            std::string type, name; Position p; int hp;
            ss >> type >> p.x >> p.y >> hp >> name;
            Unit* u = nullptr;
            if (type == "ARCHER") u = new Archer(p, name);
            else if (type == "MAGE") u = new Mage(p, name);
            else u = new Barbarian(p, name);
            if (u) { u->health = hp; defenders.push_back(u); }
        } else if (tag == "RAIDER") {
            Position p; int hp; std::string name;
            ss >> p.x >> p.y >> hp >> name;
            auto* r = new Raider(p, name); r->health = hp;
            raiders.push_back(r);
        } else if (tag == "BOMBERMAN") {
            Position p; int hp; std::string name;
            ss >> p.x >> p.y >> hp >> name;
            auto* bm = new Bomberman(p, name); bm->health = hp;
            bombermen.push_back(bm);
        }
    }

    lastMessage = "OK: Partie chargee depuis " + filename;
    gameOver = false;
    return true;
}

void Board::render() const {
    // ── Constantes de mise en page ────────────────────────────────────────────
    // Le board occupe : 1 espace + "|" + width*2 colonnes + "|" = width*2 + 3 colonnes
    // Le panel commence à la colonne PANEL_COL (1-based pour ANSI \033[row;colH)
    // On ajoute 2 espaces de marge entre le board et le panel
    const int BOARD_COLS  = 2 + width * 2 + 1; // " |" + cells + "|"  (en colonnes visuelles)
    const int PANEL_COL   = BOARD_COLS + 3;     // 2 espaces de séparateur + 1 (1-based)
    const int PANEL_LINES = 29;                 // lignes 0..28
    // Nombre total de lignes affichées : 1 (top border) + height + 1 (bot border)
    const int BOARD_ROWS  = height + 2;

    std::ostringstream buf;

    // Effacement complet et retour en (1,1)
    buf << "\033[2J\033[H";

    // ── Ligne 1 : bordure haute du board ─────────────────────────────────────
    // format : " +" + width*"--" + "+"
    buf << " +";
    for (int x = 0; x < width; x++) buf << "--";
    buf << "+";
    // Panel ligne 0 à droite (sur la même ligne écran = ligne 1)
    buf << "\033[1;" << PANEL_COL << "H" << getPanelLine(0) << "\033[K";
    buf << "\n";

    // ── Lignes 2..(height+1) : rangées du board + panel à droite ─────────────
    for (int y = 0; y < height; y++) {
        // --- Contenu board sur cette ligne ---
        buf << " |";
        for (int x = 0; x < width; x++) {
            std::string cell = getCellContent(x, y);
            if (cell.empty()) {
                buf << "  ";
            } else {
                unsigned char first = static_cast<unsigned char>(cell[0]);
                if (first < 0x80) {
                    // ASCII pur : 1 colonne visuelle → on pad avec un espace
                    if (cell.size() == 1)
                        buf << cell << " ";
                    else
                        buf << cell; // ex: "\033[...m \033[0m " = 2 cols visuelles
                } else {
                    // UTF-8 multi-octet (emoji) = 2 colonnes visuelles → tel quel
                    buf << cell;
                }
            }
        }
        buf << "|"; // bordure droite — le curseur est EXACTEMENT après width*2+3 cols

        // --- Panel à droite : on se positionne à PANEL_COL sur cette même ligne ---
        int screenRow = y + 2; // ligne écran 1-based : 1=top border, 2=first game row
        int panelIdx  = y + 1; // panel ligne 0 déjà affichée sur la top border
        if (panelIdx < PANEL_LINES) {
            buf << "\033[" << screenRow << ";" << PANEL_COL << "H"
                << getPanelLine(panelIdx) << "\033[K";
        }
        buf << "\n";
    }

    // ── Ligne height+2 : bordure basse du board ───────────────────────────────
    buf << " +";
    for (int x = 0; x < width; x++) buf << "--";
    buf << "+";
    // Panel : lignes restantes (panelIdx = height+1 .. PANEL_LINES-1)
    {
        int screenRow = height + 2;
        int panelIdx  = height + 1;
        if (panelIdx < PANEL_LINES) {
            buf << "\033[" << screenRow << ";" << PANEL_COL << "H"
                << getPanelLine(panelIdx) << "\033[K";
        }
    }
    buf << "\n";

    // ── Lignes panel restantes (si le panel est plus long que le board) ────────
    // BOARD_ROWS = height + 2 lignes (top + rows + bot)
    // Les lignes panel 0..(BOARD_ROWS-1) ont déjà été affichées ci-dessus.
    // Les suivantes sont affichées sous le board à droite (position absolue).
    for (int panelIdx = BOARD_ROWS; panelIdx < PANEL_LINES; panelIdx++) {
        int screenRow = panelIdx + 1; // décalage : la ligne 0 du panel = ligne écran 1
        buf << "\033[" << screenRow << ";" << PANEL_COL << "H"
            << getPanelLine(panelIdx) << "\033[K";
    }

    // ── Message de feedback : sous le board (colonne 1) ──────────────────────
    int msgRow = BOARD_ROWS + 1; // juste sous la bordure basse
    buf << "\033[" << msgRow << ";1H";
    if (lastMessage.rfind("OK:", 0) == 0)
        buf << GREEN << BOLD << lastMessage << RESET;
    else if (lastMessage.rfind("ERREUR:", 0) == 0)
        buf << RED << BOLD << lastMessage << RESET;
    else if (lastMessage.rfind("ALERTE:", 0) == 0)
        buf << YELLOW << BOLD << lastMessage << RESET;
    else
        buf << DIM << lastMessage << RESET;
    buf << "\033[K\n";

    // ── GAME OVER : sous le message ───────────────────────────────────────────
    if (gameOver) {
        buf << "\033[" << (msgRow + 2) << ";1H"
            << RED << BOLD
            << "╔══════════════════════════════╗\n"
            << "║         GAME  OVER !         ║\n"
            << "║   Le TownHall est detruit !  ║\n"
            << "╚══════════════════════════════╝"
            << RESET << "\n";
    }

    std::cout << buf.str();
    std::cout.flush();
}

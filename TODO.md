# Village Under Attack - Partie 2: Amélioration et Défense ✅ COMPLETE

## Plan Steps

### 1. Fix Barracks ✅
- [x] Fix `Buildings/Barracks.h`: Changed `popTrainedUnit()` return type to `std::unique_ptr<Unit>`
- [x] Fixed all relative includes to project-root based includes

### 2. Fix Unit Stats ✅
- [x] Edit `Entities/Units/Barbarian.cpp`: Reduced damage from 25 to 8 (Weak melee per spec)

### 3. Convert Bomberman to Enemy ✅
- [x] Created `Entities/Enemies/Bomberman.h`: Enemy subclass that targets walls
- [x] Created `Entities/Enemies/Bomberman.cpp`: Wall-destroying logic with `findTargetWall()`
- [x] Deleted old player `Entities/Units/Bomberman.h` and `.cpp`

### 4. Improve Enemy Combat System ✅
- [x] Edit `Entities/Enemies/Enemy.h`: Added `attackCooldown` / `attackCooldownMax`
- [x] Edit `Entities/Enemies/Enemy.cpp`: Implemented cooldown logic
- [x] `Npc.h`/`Npc.cpp`: Added `moveToward()` as virtual method

### 5. Fix Board.h ✅
- [x] Added `#include` for Raider.h and Bomberman.h
- [x] Removed duplicate private declarations
- [x] Added `bombermen` vector
- [x] Added `spawnBomberman()` declaration
- [x] Made `render()` public

### 6. Fix & Improve Board.cpp ✅
- [x] Added includes for Archer.h and Barbarian.h
- [x] Fixed `getPanelLine()`: complete cases 0-21, no duplicates
- [x] Fixed `updateEnemies()`: separate raider/bomberman logic with cooldowns
- [x] Added `spawnBomberman()` function (spawns every 3x raider interval)
- [x] Updated render: show bombermen count
- [x] Updated `getCellContent()`: render bombermen with 💣

### 7. Fix CMakeLists.txt ✅
- [x] Completed file with all source files

### 8. Build & Test ✅
- [x] Compiled successfully with g++ -std=c++17
- [x] game.exe generated (250KB)

---

## Combat Rules Implemented (with strict comments)

| Unit | Type | Range | Damage | Behavior |
|------|------|-------|--------|----------|
| **Archer** | Player Unit | 4 | 12 | Ranged attack, targets nearest enemy |
| **Barbarian** | Player Unit | 1 | 8 | Melee attack, weak but tanky (50 HP) |
| **Raider** | Enemy | 1 | 2 | Targets units first, then buildings |
| **Bomberman** | Enemy | 1 | 15 | Targets walls exclusively, high damage |

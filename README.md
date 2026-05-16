# 🏰 Village Under Attack: Premium Edition (V.2026)

![Version](https://img.shields.io/badge/version-2026.Premium-blueviolet?style=for-the-badge)
![Engine](https://img.shields.io/badge/Engine-Custom_C%2B%2B_/_SDL3-blue?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Complete-success?style=for-the-badge)

A high-performance, visually stunning **Tactical Base-Building & Tower Defense Survival** game built from the ground up in C++ using the cutting-edge **SDL3** library. Defend your village, manage resources, and lead your troops against relentless waves of enemies.

---

## 💎 Premium Features

*   **Glassmorphic UI**: High-end immediate-mode GUI with real-time blur, glow effects, and dynamic shadows.
*   **Tactical Depth**: Strategic unit targeting (Archers vs. Raiders, Barbarians as tanks).
*   **Dynamic Economy**: Build Gold Mines and Elixir Collectors to fund your defense.
*   **Military Management**: Train specialized units in Barracks and upgrade buildings to increase survivability.
*   **Advanced A* Pathfinding**: Units now use intelligent pathfinding to navigate around buildings and walls without getting stuck.
*   **Vibrant Visuals**: Powered by SDL3 rendering with particle effects, screen shake, and smooth animations.
*   **Survive the Wave**: Face different enemy types, including standard Raiders and building-targeting Bombermen.

---

## 🎮 How to Play

### Objectives
1.  **Protect the Town Hall**: If it falls, the village is lost.
2.  **Resource Cycle**: Collect Gold and Elixir from your generators.
3.  **Build Defenses**: Erect Walls and Barracks to slow down and eliminate threats.
4.  **Army Composition**: Balance your squad with melee Barbarians and ranged Archers.

### Tactical Controls
| Key | Action |
|-----|--------|
| **Arrow Keys** | Move The Commander |
| **[C]** | Collect Resources (Gold/Elixir) from nearby mines |
| **[G]** | Construct a **Gold Mine** |
| **[E]** | Construct an **Elixir Collector** |
| **[W]** | Erect a **Wall** for defense |
| **[B]** | Build a **Military Barracks** |
| **[A]** | Recruit an **Archer** (Ranged) |
| **[H]** | Recruit a **Barbarian** (Melee) |
| **[M]** | Recruit a **Mage** (Magic) |
| **[U]** | **Upgrade** the building at your current position |
| **[S]** | **Save** the current village state |
| **[L]** | **Load** a previously saved village |
| **[P]** | Start Game (from Menu) |
| **[R]** | Restart (from Game Over) |

---

## 🛠️ Technical Stack

*   **Language**: C++17 (or higher)
*   **Graphics**: [SDL3](https://www.libsdl.org/) (Simple DirectMedia Layer 3)
*   **Image Loading**: [stb_image](https://github.com/nothings/stb)
*   **Build System**: CMake

---

## 🚀 Getting Started

### Prerequisites
- A C++ compiler (GCC/Clang/MSVC)
- SDL3 development libraries installed on your system.

### Compiling (CMake)
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Compiling (Manual/Batch)
If you are on Windows and have the SDL3 DLLs in the project root, you can use the provided script:
```powershell
.\compile_sdl.bat
```

---

## 🏗️ Project Architecture

The project follows a clean Object-Oriented Design:
- **`Entities/`**: Base classes for Units, Enemies, and the Player.
- **`Buildings/`**: Specialized classes for Resource Generators, Walls, and Barracks.
- **`Board.cpp/h`**: The heart of the game logic, managing collisions, spawning, and combat.
- **`sdl_render.h`**: A custom premium UI framework for modern SDL3 applications.

---

## 📜 Credits
- **Developer**: escgot
- **Engine**: SDL3 Premium

---

*Developed with passion in May 2026.* ⚔️🛡️

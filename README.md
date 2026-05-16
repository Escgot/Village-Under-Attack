# 🏰 Village Under Attack: Premium Edition (V.2026)

![Version](https://img.shields.io/badge/version-2026.Premium-blueviolet?style=for-the-badge)
![Engine](https://img.shields.io/badge/Engine-Custom_C%2B%2B_/_SDL3-blue?style=for-the-badge)
![Memory](https://img.shields.io/badge/Memory-Smart_Pointers_/_RAII-success?style=for-the-badge)

A high-performance, visually stunning **Tactical Base-Building & Tower Defense Survival** game built from the ground up in Modern C++ using the cutting-edge **SDL3** library. Defend your village, manage resources, and lead your troops against relentless waves of enemies.

---

## 💎 Premium Features

*   **✨ Visual Interpolation (Lerp)**: Entities move fluidly between grid cells using high-frequency linear interpolation, eliminating "snapping" and jitter.
*   **⏱️ Frame-Rate Independence**: Integrated **Delta-Time (dt)** logic ensures game speed and animations remain consistent across different monitor refresh rates (60Hz to 144Hz+).
*   **🛡️ Modern C++ Architecture**: Refactored core engine to use `std::unique_ptr` and `std::shared_ptr`, ensuring zero memory leaks and robust state management.
*   **🔊 Spatial Audio**: Implemented a positional sound system that scales volume based on the player's proximity to combat and construction.
*   **🔍 Advanced A* Pathfinding**: Intelligent 8-direction navigation allows units to path around complex building layouts and wall fortifications.
*   **💎 Glassmorphic UI**: High-end immediate-mode GUI with real-time blur, pulsing glows, and responsive micro-animations.
*   **💾 State Persistence**: Fully functional Save/Load system to preserve your village across sessions.

---

## 🎮 How to Play

### Objectives
1.  **Protect the Town Hall**: If it falls, your legacy ends.
2.  **Resource Cycle**: Harvest Gold and Elixir from specialized mines.
3.  **Build Defenses**: Erect Walls and Barracks to slow down and eliminate threats.
4.  **Army Composition**: Recruit melee Barbarians, ranged Archers, and powerful Mages.

### Tactical Controls
| Key | Action |
|-----|--------|
| **Arrow Keys** | Move The Commander |
| **[C]** | Collect Resources (Gold/Elixir) |
| **[G]** | Construct a **Gold Mine** (100 Elixir) |
| **[E]** | Construct an **Elixir Collector** (100 Gold) |
| **[W]** | Erect a **Wall** (10 Gold) |
| **[B]** | Build a **Military Barracks** (150 Gold + Elixir) |
| **[A] / [H] / [M]** | Recruit **Archer** / **Barbarian** / **Mage** |
| **[U]** | **Upgrade** building at current position |
| **[S] / [L]** | **Save** / **Load** Village State |
| **[ESC]** | Menu / Back |

---

## 🛠️ Technical Stack

*   **Language**: Modern C++17 (RAII, Smart Pointers)
*   **Graphics/Audio**: [SDL3](https://www.libsdl.org/)
*   **Pathfinding**: Custom A* (Chebyshev distance)
*   **Animation**: Linear Interpolation (Lerp) with Delta-Time
*   **Image Loading**: stb_image

---

## 🏗️ Project Architecture

The project follows a clean, decoupled Object-Oriented Design:
- **`Entities/`**: Intelligent Npcs, Units, and Enemy hierarchies.
- **`Buildings/`**: Resource management and defensive structures.
- **`Board.cpp/h`**: Centralized logic controller for combat and spatial awareness.
- **`sdl_render.h`**: A custom premium UI framework for modern SDL3 applications.

---

## 🚀 Installation & Build

### Prerequisites
- A C++17 compliant compiler (MinGW, GCC, or MSVC)
- SDL3 Development Libraries

### Build via Batch (Windows)
```powershell
.\compile_sdl.bat
```

---

## 📜 Credits
- **Lead Developer**: escgot
- **Engine**: SDL3 Premium Architecture
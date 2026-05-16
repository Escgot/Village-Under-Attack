cmake_minimum_required(VERSION 3.14)
project(VillageUnderAttack CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UTF-8 on MSVC
if(MSVC)
    add_compile_options(/utf-8)
endif()

# The playable SDL game is built from main_sdl.cpp (same sources as compile_sdl.bat).
# CMake optional: requires SDL3 CMake config (set SDL3_DIR to your SDL3 build prefix).
find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found via CMake — skipping game_sdl_premium target.")
    message(STATUS "On Windows, build with: compile_sdl.bat  ->  game_sdl_premium.exe")
    return()
endif()

add_executable(game_sdl_premium
    main_sdl.cpp
    Board.cpp
    Position.cpp
    Resources.cpp
    WaveManager.cpp
    Buildings/Building.cpp
    Buildings/TownHall.cpp
    Buildings/GoldMine.cpp
    Buildings/ElixirCollector.cpp
    Buildings/Barracks.cpp
    Buildings/Wall.cpp
    Buildings/ResourceGenerator.cpp
    Entities/Entity.cpp
    Entities/Npc.cpp
    Entities/Player.cpp
    Entities/Enemies/Enemy.cpp
    Entities/Enemies/Raider.cpp
    Entities/Enemies/Bomberman.cpp
    Entities/Units/Unit.cpp
    Entities/Units/Archer.cpp
    Entities/Units/Barbarian.cpp
    Entities/Units/Mage.cpp
)

target_include_directories(game_sdl_premium PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(game_sdl_premium PRIVATE SDL3::SDL3)

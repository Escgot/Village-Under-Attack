@echo off
set GPP="C:\Program Files\CodeBlocks\MinGW\bin\g++.exe"
set SDL_PATH="D:\projets\escgot\SDL3-3.4.4\x86_64-w64-mingw32"
set INCS=-I. -I%SDL_PATH%\include
set LIBS=-L%SDL_PATH%\lib -lSDL3

echo Compiling Village Under Attack (SDL Version)...

%GPP% main_sdl.cpp Board.cpp Position.cpp Resources.cpp WaveManager.cpp ^
Buildings/Building.cpp Buildings/TownHall.cpp Buildings/GoldMine.cpp Buildings/ElixirCollector.cpp ^
Buildings/Barracks.cpp Buildings/Wall.cpp Buildings/ResourceGenerator.cpp ^
Entities/Entity.cpp Entities/Npc.cpp Entities/Player.cpp ^
Entities/Enemies/Enemy.cpp Entities/Enemies/Raider.cpp Entities/Enemies/Bomberman.cpp ^
Entities/Units/Unit.cpp Entities/Units/Archer.cpp Entities/Units/Barbarian.cpp Entities/Units/Mage.cpp ^
%INCS% %LIBS% -std=c++17 -o game_sdl_premium.exe > compile_log_sdl.txt 2>&1

if exist game_sdl_premium.exe (
    echo SUCCESS - game_sdl_premium.exe created.
) else (
    echo FAILED - check compile_log_sdl.txt
    type compile_log_sdl.txt
)

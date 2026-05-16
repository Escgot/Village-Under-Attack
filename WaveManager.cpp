#include "WaveManager.h"
#include "Board.h"

WaveManager::WaveManager() : waveCount(0), ticksUntilNextWave(600), isWaveActive(false) {}

void WaveManager::update(Board& board) {
    if (!isWaveActive) {
        ticksUntilNextWave--;
        if (ticksUntilNextWave <= 0) {
            isWaveActive = true;
            waveCount++;
            ticksUntilNextWave = 150; // Wave duration
        }
    } else {
        ticksUntilNextWave--;
        // Spawn horde during wave
        if (board.tick % 10 == 0) board.spawnRaider();
        if (board.tick % 30 == 0) board.spawnBomberman();
        
        if (ticksUntilNextWave <= 0) {
            isWaveActive = false;
            ticksUntilNextWave = 600; // Time between waves
        }
    }
}

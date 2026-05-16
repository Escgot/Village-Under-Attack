#pragma once
class Board;

class WaveManager {
public:
    int waveCount;
    int ticksUntilNextWave;
    bool isWaveActive;

    WaveManager();
    void update(Board& board);
};

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <SDL3/SDL.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

#include "Board.h"
#include "Entities/Units/UnitType.h"
#include "sdl_render.h"

enum class State { MAIN_MENU, CREDITS, BACKGROUND_SELECT, INSTRUCTIONS, PLAYING, GAME_OVER };

namespace {
constexpr const char* kWindowTitlePlaying = "Village Under Attack - Premium";
constexpr const char* kWindowTitleGameOver = "VILLAGE UNDER ATTACK — YOUR TOWN HALL HAS FALLEN";
}

static int g_loadedTextures = 0;
SDL_Texture* loadTexture(SDL_Renderer* r, const char* relativePath) {
    std::vector<std::string> paths;
    const char* basePath = SDL_GetBasePath();
    if (basePath) paths.push_back(std::string(basePath) + relativePath);
    paths.push_back(std::string("./") + relativePath);
    paths.push_back(std::string(relativePath));

    void* fileData = nullptr;
    size_t size = 0;
    std::string usedPath;

    for (const auto& p : paths) {
        fileData = SDL_LoadFile(p.c_str(), &size);
        if (fileData) { usedPath = p; break; }
    }

    if (!fileData) {
        SDL_Log("CRITICAL ERROR: Could not find asset at any path: %s", relativePath);
        return nullptr;
    }

    int w, h, ch;
    unsigned char* imgData = stbi_load_from_memory((unsigned char*)fileData, (int)size, &w, &h, &ch, 4);
    SDL_free(fileData);

    if (!imgData) {
        SDL_Log("ERROR: STB decoding failed for: %s", usedPath.c_str());
        return nullptr;
    }

    SDL_Surface* surf = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, imgData, w * 4);
    if (!surf) { stbi_image_free(imgData); return nullptr; }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_DestroySurface(surf);
    stbi_image_free(imgData);
    
    if (tex) {
        SDL_Log("SUCCESS: Texture Ready -> %s", usedPath.c_str());
        g_loadedTextures++;
    }
    return tex;
}

// ── Audio Engine ─────────────────────────────────────────────────────────────
void loadSound(const char* relativePath, SoundEffect& s) {
    const char* basePath = SDL_GetBasePath();
    std::string fullPath = std::string(basePath ? basePath : "") + relativePath;
    if (!SDL_LoadWAV(fullPath.c_str(), &s.spec, &s.buffer, &s.length)) {
        SDL_Log("ERROR: Failed to load sound: %s", fullPath.c_str());
    } else {
        SDL_Log("SUCCESS: Loaded sound: %s", fullPath.c_str());
    }
}

static SDL_AudioDeviceID g_audioDevice = 0;

void playSoundPositional(const SoundEffect& s, const Position& source, const Position& player) {
    if (!s.buffer || g_audioDevice == 0) return;
    
    // Simple linear distance-based volume scaling
    float dx = (float)(source.x - player.x);
    float dy = (float)(source.y - player.y);
    float dist = sqrtf(dx*dx + dy*dy);
    float maxDist = 25.0f;
    float volume = 1.0f - (dist / maxDist);
    if (volume < 0.1f) return; // Too far
    
    SDL_AudioStream* stream = SDL_CreateAudioStream(&s.spec, nullptr);
    if (stream) {
        SDL_SetAudioStreamGain(stream, volume);
        SDL_BindAudioStream(g_audioDevice, stream);
        SDL_PutAudioStreamData(stream, s.buffer, s.length);
        SDL_FlushAudioStream(stream);
    }
}

void initAssets(SDL_Renderer* r, GameAssets& assets) {
    assets.backgrounds.push_back({"Magic Forest", "assets/bg_magicforest.png", nullptr});
    assets.backgrounds.push_back({"Deep Forest", "assets/bg_forest.png", nullptr});
    assets.backgrounds.push_back({"Desert Sands", "assets/bg_desert.png", nullptr});
    assets.backgrounds.push_back({"Grasslands", "assets/grass_bg.png", nullptr});
    for (auto& bg : assets.backgrounds) bg.tex = loadTexture(r, bg.path.c_str());
    
    assets.t_townhall = loadTexture(r, "assets/sprite_townhall.png");
    assets.t_townhall80 = loadTexture(r, "assets/sprite_townhall80.png");
    assets.t_townhall40 = loadTexture(r, "assets/sprite_townhall40.png");
    assets.t_townhall00 = loadTexture(r, "assets/sprite_townhall00.png");
    assets.t_goldmine = loadTexture(r, "assets/gold_mine.png");
    assets.t_goldmine1 = loadTexture(r, "assets/gold_mine1.png");
    assets.t_elixir = loadTexture(r, "assets/elixir_collector.png");
    assets.t_elixir1 = loadTexture(r, "assets/elixir_collector1.png");
    assets.t_barracks = loadTexture(r, "assets/bracket.png");
    assets.t_barracks_on = loadTexture(r, "assets/bracket_on.png");
    assets.t_wall = loadTexture(r, "assets/wall.png");
    assets.t_landing = loadTexture(r, "assets/landing.png");
    assets.t_archer = loadTexture(r, "assets/archer.png");
    assets.t_barbarian = loadTexture(r, "assets/barbarian.png");
    assets.t_raider = loadTexture(r, "assets/raider.png");
    assets.t_bomber = loadTexture(r, "assets/bomber.png");

    loadSound("audio/build.wav", assets.s_build);
    loadSound("audio/collect_gold.wav", assets.s_collect_gold);
    loadSound("audio/collect_elixir.wav", assets.s_collect_elixir);
    loadSound("audio/hit.wav", assets.s_hit);
}

// ── Rendu ────────────────────────────────────────────────────────────────────

static void renderBackground(SDL_Renderer* r, const GameAssets& assets) {
    if (assets.backgrounds.empty()) return;
    SDL_Texture* tex = assets.backgrounds[assets.selectedBg].tex;
    if (tex) {
        SDL_RenderTexture(r, tex, nullptr, nullptr);
        fR(r, 0, 0, WIN_W, WIN_H, {0, 0, 0, 80});
    } else {
        fR(r, 0, 0, WIN_W, WIN_H, C_BOARD_BG);
    }
}

static void drawSprite(SDL_Renderer* r, SDL_Texture* tex, float gx, float gy, int szX, int szY, RGBA fallback, float bob = 0.0f) {
    float ox = 0, oy = 0;
    if (g_shake >= 1.0f) {
        ox = (float)(rand() % (int)g_shake - (int)g_shake / 2);
        oy = (float)(rand() % (int)g_shake - (int)g_shake / 2);
    }
    float x = cX(0) + gx * CELL - (szX/2.0f)*CELL + ox;
    float y = cY(0) + gy * CELL - (szY/2.0f)*CELL + bob + oy;
    float w = szX * CELL;
    float h = szY * CELL;
    if (tex) {
        SDL_FRect dst = {x, y, w, h};
        SDL_RenderTexture(r, tex, nullptr, &dst);
    } else {
        fR(r, x+1, y+1, w-2, h-2, fallback);
        dR(r, x+1, y+1, w-2, h-2, {255,255,255,100});
    }
}

static void renderBoard(SDL_Renderer* ren, const Board& board, const GameAssets& assets) {
    float time = (float)SDL_GetTicks() / 1000.0f;
    float ox = 0, oy = 0;
    if (g_shake >= 1.0f) {
        ox = (float)(rand() % (int)g_shake - (int)g_shake / 2);
        oy = (float)(rand() % (int)g_shake - (int)g_shake / 2);
    }

    // Grid
    setC(ren, {C_GRID.r, C_GRID.g, C_GRID.b, (Uint8)(C_GRID.a * (1.0f + 0.2f*sinf(time)))});
    for (int x = 0; x <= BOARD_W; x++) SDL_RenderLine(ren, cX(x)+ox, MARGIN+oy, cX(x)+ox, MARGIN+BOARD_H*CELL+oy);
    for (int y = 0; y <= BOARD_H; y++) SDL_RenderLine(ren, MARGIN+ox, cY(y)+oy, MARGIN+BOARD_W*CELL+ox, cY(y)+oy);

    // TownHall
    auto& th = board.townHall;
    if (th) {
        float pct = (float)th->health / (float)th->maxHealth;
        SDL_Texture* thTex = assets.t_townhall;
        if (pct <= 0.0f) thTex = assets.t_townhall00;
        else if (pct <= 0.4f) thTex = assets.t_townhall40;
        else if (pct <= 0.8f) thTex = assets.t_townhall80;

        fGlow(ren, cX(0) + th->visualX*CELL + ox, cY(0) + th->visualY*CELL + oy, 35, {255, 200, 0, (Uint8)(50 + 30*sinf(time*3))});
        
        // Level Tinting for TH
        if (th->level > 1) {
            SDL_SetTextureColorMod(thTex, 150, 200, 255);
        }
        drawSprite(ren, thTex, th->visualX, th->visualY, th->sizeX, th->sizeY, {255, 210, 50, 180});
        if (th->level > 1) SDL_SetTextureColorMod(thTex, 255, 255, 255);
        
        if (th->health < th->maxHealth) {
            hpBar(ren, cX(0) + (th->visualX - 2.5f)*CELL + ox, cY(0) + (th->visualY - 2.5f)*CELL - 15 + oy, 5 * CELL, 10, th->health, th->maxHealth);
        }
        
        // Draw Level Indicator
        char lvlBuf[16]; sprintf(lvlBuf, "LVL %d", th->level);
        txt(ren, cX(0) + th->visualX*CELL - 15 + ox, cY(0) + th->visualY*CELL + 30 + oy, lvlBuf, {255, 200, 50, 255}, 1.2f);
    }

    // Buildings
    bool animFrame = (SDL_GetTicks() % 1000) > 500;
    
    for (auto& g : board.goldMines) {
        SDL_Texture* tex = (animFrame && assets.t_goldmine1) ? assets.t_goldmine1 : assets.t_goldmine;
        drawSprite(ren, tex, g->visualX, g->visualY, g->sizeX, g->sizeY, {230, 180, 30, 180});
        if (g->health < g->maxHealth) {
            hpBar(ren, cX(0) + (g->visualX - g->sizeX/2.0f)*CELL + ox, cY(0) + (g->visualY - g->sizeY/2.0f)*CELL - 10 + oy, g->sizeX * CELL, 6, g->health, g->maxHealth);
        }
    }
    
    for (auto& e : board.elixirCollectors) {
        SDL_Texture* tex = (animFrame && assets.t_elixir1) ? assets.t_elixir1 : assets.t_elixir;
        drawSprite(ren, tex, e->visualX, e->visualY, e->sizeX, e->sizeY, {170, 50, 210, 180});
        if (e->health < e->maxHealth) {
            hpBar(ren, cX(0) + (e->visualX - e->sizeX/2.0f)*CELL + ox, cY(0) + (e->visualY - e->sizeY/2.0f)*CELL - 10 + oy, e->sizeX * CELL, 6, e->health, e->maxHealth);
        }
    }
    for (auto& b : board.barracks) {
        SDL_Texture* bTex = (!b->trainQueue.empty()) ? assets.t_barracks_on : assets.t_barracks;
        if (b->level > 1) SDL_SetTextureColorMod(bTex, 255, 150, 255);
        drawSprite(ren, bTex, b->visualX, b->visualY, b->sizeX, b->sizeY, {30, 180, 110, 180});
        if (b->level > 1) SDL_SetTextureColorMod(bTex, 255, 255, 255);
        
        if (b->health < b->maxHealth) {
            hpBar(ren, cX(0) + (b->visualX - b->sizeX/2.0f)*CELL + ox, cY(0) + (b->visualY - b->sizeY/2.0f)*CELL - 10 + oy, b->sizeX * CELL, 6, b->health, b->maxHealth);
        }
        char lvlBuf[16]; sprintf(lvlBuf, "LVL %d", b->level);
        txt(ren, cX(0) + b->visualX*CELL - 15 + ox, cY(0) + b->visualY*CELL + 15 + oy, lvlBuf, {255, 150, 255, 255}, 0.8f);
        if (!b->trainQueue.empty()) {
            float bx = cX(0) + (b->visualX - b->sizeX/2.0f)*CELL + ox;
            float by = cY(0) + (b->visualY - b->sizeY/2.0f)*CELL - 24 + oy;
            hpBar(ren, bx, by, b->sizeX * CELL, 8, b->trainTimer, 60, true);
            char trainBuf[32];
            int ticksLeft = 60 - b->trainTimer; // TRAIN_INTERVAL is 60
            int secondsLeft = (ticksLeft + 9) / 10; // 10 ticks per real-world second
            sprintf(trainBuf, "TRAINING... [%ds]", secondsLeft);
            txt(ren, bx, by - 15, trainBuf, {100, 200, 255, 255});
        }
    }
    for (auto& w : board.walls) {
        drawSprite(ren, assets.t_wall, w->visualX, w->visualY, 1, 1, {160, 160, 170, 255});
        if (w->health < w->maxHealth) {
            hpBar(ren, cX(0) + (w->visualX - 0.5f)*CELL + ox, cY(0) + (w->visualY - 0.5f)*CELL - 6 + oy, CELL, 4, w->health, w->maxHealth);
        }
    }

    // Entities
    auto drawEnt = [&](float gx, float gy, RGBA c, bool isHero = false, const char* name = nullptr, SDL_Texture* sprite = nullptr, int hp = 0, int mhp = 0, RGBA barColor = {100, 255, 100, 255}, RGBA tint = {255,255,255,255}) {
        float ex = cX(0) + gx * CELL + ox;
        float ey = cY(0) + gy * CELL + oy;
        
        // Shadow
        fCircle(ren, (int)(ex + CELL/2), (int)(ey + CELL - 4), 12, {0, 0, 0, 80});

        if (sprite) {
            SDL_SetTextureColorMod(sprite, tint.r, tint.g, tint.b);
            SDL_FRect dst = {ex, ey, CELL, CELL};
            SDL_RenderTexture(ren, sprite, nullptr, &dst);
            SDL_SetTextureColorMod(sprite, 255, 255, 255); // Reset
        } else {
            fRoundedRect(ren, ex + 2, ey + 2, CELL - 4, CELL - 4, 4, c);
        }
        
        // Floating Name Tag & Mini HP Bar
        if (name || isHero || (hp > 0 && hp < mhp)) {
            const char* label = isHero ? "COMMANDER" : name;
            RGBA labelCol = isHero ? C_PLAYER : (name ? C_WHITE : c);
            
            float tw = 40.0f; // Standard width for mini bars
            if (label) tw = (float)strlen(label) * 7.0f + 10.0f;
            tw = std::max(tw, 40.0f);
            
            float tx = ex + CELL/2 - tw/2;
            float ty = ey - 22;
            
            // Glass background for tag + bar
            fRoundedRect(ren, tx, ty, tw, 18, 4, {20, 25, 35, 180});
            
            if (label) {
                txt(ren, tx + 5, ty + 2, label, labelCol, 0.6f);
            }
            
            // Mini HP Bar
            if (mhp > 0) {
                float hW = tw - 10;
                float fill = (float)hp / (float)mhp;
                RGBA hpC = barColor;
                
                fR(ren, tx + 5, ty + 12, hW, 3, {50, 50, 60, 255});
                fR(ren, tx + 5, ty + 12, hW * fill, 3, hpC);
            }
        }

        if (isHero) {
            dR(ren, ex - 2, ey - 2, CELL + 4, CELL + 4, C_PLAYER);
        }
    };

    for (auto& u : board.defenders) {
        if(!u->isAlive()) continue;
        SDL_Texture* tex = (u->type == UnitType::ARCHER || u->type == UnitType::MAGE) ? assets.t_archer : assets.t_barbarian;
        RGBA tint = (u->type == UnitType::MAGE) ? RGBA{200, 100, 255, 255} : C_WHITE;
        RGBA tagColor = (u->type == UnitType::ARCHER) ? C_ARCHER : ((u->type == UnitType::MAGE) ? RGBA{200, 100, 255, 255} : C_BARB);
        drawEnt(u->visualX, u->visualY, tagColor, false, u->name.c_str(), tex, u->health, u->maxHealth, C_HP_G, tint);
    }
    for (auto& r : board.raiders) if(r->isAlive()) drawEnt(r->visualX, r->visualY, C_RAIDER, false, r->name.c_str(), assets.t_raider, r->health, r->maxHealth, C_HP_R);
    for (auto& bm : board.bombermen) if(bm->isAlive()) drawEnt(bm->visualX, bm->visualY, C_BOMBER, false, bm->name.c_str(), assets.t_bomber, bm->health, bm->maxHealth, C_HP_R);

    // Player (Now with a forced Name, Sprite, and UI tag)
    fGlow(ren, cX(0) + board.player->visualX*CELL + ox, cY(0) + board.player->visualY*CELL + oy, 20, {0, 100, 255, (Uint8)(40 + 20*sinf(time*4))});
    drawEnt(board.player->visualX, board.player->visualY, C_PLAYER, true, "THE COMMANDER", assets.t_barbarian, 0, 0, C_HP_G);

    // VFX
    for(auto& p : g_parts) {
        float alpha = std::max(0.0f, std::min(1.0f, p.life));
        RGBA pc = p.c; pc.a = (Uint8)(255 * alpha);
        fR(ren, p.x+ox, p.y+oy, p.size, p.size, pc);
    }
    for(auto& ft : g_fTexts) {
        float alpha = std::max(0.0f, std::min(1.0f, ft.life));
        RGBA fc = ft.c; fc.a = (Uint8)(255 * alpha);
        txt(ren, ft.x+ox, ft.y+oy, ft.text.c_str(), fc);
    }
    for(auto& w : g_wParts) {
        fR(ren, w.x, w.y, w.sz, w.sz, w.c);
    }
}

static void renderPanel(SDL_Renderer* ren, const Board& board) {
    float pw = 320, ph = 620;
    float px = WIN_W - pw - 20, py = 10;
    
    // COMMAND HUD
    fGlassPanel(ren, px, py, pw, ph, C_PLAYER, false);
    float x = px + 20, y = py + 20;
    txt(ren, x, y, "COMMAND HUD", {100, 200, 255, 255}, 1.4f); y += 35;

    auto stat = [&](const char* lbl, int val, int mx, RGBA c) {
        txt(ren, x, y, lbl, {160, 165, 175, 255}); 
        char buf[32]; sprintf(buf, "%d / %d", val, mx);
        txt(ren, x + pw - 120, y, buf, C_WHITE); y += 22;
        hpBar(ren, x, y, pw - 40, 12, val, mx, true, c); y += 30;
    };
    
    stat("GOLD", board.player->resources.gold, 1000, C_GOLD);
    stat("ELIXIR", board.player->resources.elixir, 1000, C_ELIX);
    stat("TOWN HALL", board.townHall->health, board.townHall->maxHealth, {0,0,0,0}); // Dynamic

    y += 10;
    fR(ren, x, y, pw - 40, 1, {255, 255, 255, 30}); y += 15;
    
    char timeBuf[64];
    int totalSecs = board.tick / 10;
    sprintf(timeBuf, "SURVIVAL: %02d:%02d", totalSecs / 60, totalSecs % 60);
    txt(ren, x, y, timeBuf, C_WHITE, 1.1f);
    
    char waveBuf[64];
    if (board.waveManager.isWaveActive) {
        sprintf(waveBuf, "WAVE %d ACTIVE!", board.waveManager.waveCount);
        txt(ren, x + pw - 150, y, waveBuf, C_HP_R, 1.1f);
    } else {
        sprintf(waveBuf, "NEXT WAVE: %ds", board.waveManager.ticksUntilNextWave / 10);
        txt(ren, x + pw - 150, y, waveBuf, {150, 200, 255, 255}, 1.1f);
    }
    y += 30;

    // TRAINING QUEUE
    txt(ren, x, y, "TRAINING QUEUE", {255, 150, 255, 150}); y += 20;
    bool trainingAny = false;
    for (auto& b : board.barracks) {
        if (!b->trainQueue.empty()) {
            trainingAny = true;
            UnitType t = b->trainQueue.front();
            const char* tName = (t == UnitType::ARCHER) ? "ARCHER" : ((t == UnitType::MAGE) ? "MAGE" : "BARB");
            txt(ren, x, y, tName, C_WHITE, 0.8f);
            
            float hW = pw - 120;
            hpBar(ren, x + 80, y + 2, hW, 10, b->trainTimer, 60, true, {100, 200, 255, 255});
            y += 20;
        }
    }
    if (!trainingAny) {
        txt(ren, x, y, "IDLE", C_DIM, 0.8f);
        y += 20;
    }
    y += 10;

    txt(ren, x, y, "SQUAD STATUS", {100, 200, 255, 150}); y += 25;
    
    int displayed = 0;
    // Show only first 5 defenders to fit the panel
    for (auto& u : board.defenders) {
        if (!u->isAlive()) continue;
        if (displayed >= 5) break;
        
        RGBA uCol = (u->type == UnitType::ARCHER) ? C_ARCHER : C_WHITE;
        txt(ren, x, y, u->name.c_str(), uCol, 0.8f);
        
        float hW = pw - 120;
        hpBar(ren, x + 80, y + 2, hW, 10, u->health, u->maxHealth, true, C_HP_G);
        
        y += 20;
        displayed++;
    }
    if (board.defenders.size() > 5) {
        char moreBuf[32]; sprintf(moreBuf, "+ %zu more...", board.defenders.size() - 5);
        txt(ren, x, y, moreBuf, C_DIM, 0.7f);
        y += 20;
    } else if (board.defenders.empty()) {
        txt(ren, x, y, "NO ACTIVE SQUAD", C_DIM, 0.8f);
        y += 20;
    }

    y += 10;
    char b2[64];
    sprintf(b2, "Threats detected: %zu", board.raiders.size() + board.bombermen.size());
    txt(ren, x, y, b2, C_HP_R, 0.9f);

    // Notifications Overlay
    float nx = 20, ny = WIN_H - 40;
    for (int i = (int)g_notes.size() - 1; i >= 0; i--) {
        float alpha = std::min(1.0f, g_notes[i].life);
        RGBA nc = g_notes[i].c; nc.a = (Uint8)(255 * alpha);
        fGlassPanel(ren, nx, ny - 45, 350, 40, nc, false);
        txt(ren, nx + 20, ny - 32, g_notes[i].text.c_str(), nc);
        ny -= 50;
    }
}

static void renderInstructions(SDL_Renderer* r, const GameAssets& assets) {
    if (assets.t_landing) SDL_RenderTexture(r, assets.t_landing, nullptr, nullptr);
    else fR(r, 0, 0, WIN_W, WIN_H, {10, 12, 15, 255});
    fR(r, 0, 0, WIN_W, WIN_H, {0,0,0,170}); 
    fVignette(r);
    
    float pulse = (sinf(SDL_GetTicks()*0.005f)+1.0f)*0.5f;
    txt(r, WIN_W/2 - 200, 80, "COMMANDER'S TACTICAL GUIDE", {100, 200, 255, 255}, 1.8f);
    
    float bx = WIN_W/2 - 350, bw = 700, bh = 480;
    fGlassPanel(r, bx, 150, bw, bh, {100, 200, 255, 80}, false);

    float y = 190, x = bx + 40;
    auto line = [&](const char* k, const char* d) {
        txt(r, x, y, k, C_PLAYER, 1.2f); 
        txt(r, x + 160, y, d, {200, 200, 210, 255}, 1.0f); y += 50;
    };
    line("[Arrows]", "March your Hero through the realm");
    line("[C]", "Harvest Gold & Elixir from Mines");
    line("[G / E]", "Erect Gold Mines & Elixir Storage");
    line("[W / B]", "Construct Defenses & Military Barracks");
    line("[A / H]", "Recruit Archers & Barbarian Hordes");
    line("[S / L]", "Save / Load Village State");
    
    y += 10;
    fR(r, x, y, bw - 80, 1, {255, 255, 255, 30}); y += 30;
    txt(r, x + 20, y, "MISSION: DEFEND THE TOWN HALL AT ALL COSTS", C_HP_R, 1.1f);
    
    txt(r, WIN_W/2-160, WIN_H-80, "PRESS [ENTER] TO COMMENCE", {(Uint8)(180+75*pulse),(Uint8)(180+75*pulse),(Uint8)(180+75*pulse),255}, 1.2f);
}

static void renderMainMenu(SDL_Renderer* r, const GameAssets& assets, float bestTime) {
    if (assets.t_landing) SDL_RenderTexture(r, assets.t_landing, nullptr, nullptr);
    fR(r, 0, 0, WIN_W, WIN_H, {0,0,0,100}); 
    fVignette(r);

    float time = (float)SDL_GetTicks() * 0.001f;
    float titleX = WIN_W/2 - 300;
    txt(r, titleX + 4, 154, "VILLAGE UNDER ATTACK", {20, 20, 30, 200}, 3.5f);
    txt(r, titleX, 150, "VILLAGE UNDER ATTACK", C_WHITE, 3.5f);
    
    // Buttons
    float bx = WIN_W/2 - 150, bw = 300, bh = 60;
    fGlassPanel(r, bx, 350, bw, bh, C_PLAYER, false);
    txt(r, bx + 100, 370, "[P] PLAY", C_WHITE, 1.4f);

    fGlassPanel(r, bx, 440, bw, bh, {200, 200, 210, 100}, false);
    txt(r, bx + 80, 460, "[K] CREDITS", C_WHITE, 1.4f);

    // Sprite Showcase (Full Battlefield Test)
    float sy = 400;
    if (assets.t_archer) {
        SDL_FRect d = {bx - 180, sy, 60, 60}; SDL_RenderTexture(r, assets.t_archer, nullptr, &d);
        txt(r, bx - 180, sy + 65, "ARCHER", C_WHITE, 0.7f);
    }
    if (assets.t_barbarian) {
        SDL_FRect d = {bx - 100, sy, 60, 60}; SDL_RenderTexture(r, assets.t_barbarian, nullptr, &d);
        txt(r, bx - 100, sy + 65, "BARB", C_WHITE, 0.7f);
    }
    if (assets.t_raider) {
        SDL_FRect d = {bx + bw + 40, sy, 60, 60}; SDL_RenderTexture(r, assets.t_raider, nullptr, &d);
        txt(r, bx + bw + 40, sy + 65, "RAIDER", C_RAIDER, 0.7f);
    }
    if (assets.t_bomber) {
        SDL_FRect d = {bx + bw + 120, sy, 60, 60}; SDL_RenderTexture(r, assets.t_bomber, nullptr, &d);
        txt(r, bx + bw + 120, sy + 65, "BOMBER", C_BOMBER, 0.7f);
    }

    if (bestTime > 0) {
        char bBuf[64]; int bs = (int)bestTime;
        sprintf(bBuf, "BEST SURVIVAL: %02d:%02d", bs/60, bs%60);
        txt(r, WIN_W/2 - 120, 550, bBuf, {255, 210, 0, 255}, 1.2f);
    }

    float pulse = (sinf(time * 6.0f) + 1.0f) * 0.5f;
    txt(r, WIN_W/2 - 120, WIN_H - 100, "V.2026 PREMIUM EDITION", {100, 200, 255, (Uint8)(100 + 155 * pulse)});
}

static void renderCredits(SDL_Renderer* r, const GameAssets& assets) {
    if (assets.t_landing) SDL_RenderTexture(r, assets.t_landing, nullptr, nullptr);
    fR(r, 0, 0, WIN_W, WIN_H, {0,0,0,200}); 
    fVignette(r);

    txt(r, WIN_W/2 - 100, 100, "HALL OF FAME", C_PLAYER, 2.0f);
    
    float bx = WIN_W/2 - 300, bw = 600, bh = 400;
    fGlassPanel(r, bx, 180, bw, bh, {255, 210, 0, 100}, false);

    float y = 230, x = bx + 50;
    txt(r, x, y, "DEVELOPER:", C_DIM); txt(r, x + 200, y, "escgot", C_WHITE); y += 60;
    txt(r, x, y, "GRAPHICS:", C_DIM); txt(r, x + 200, y, "MODERN ASSETS", C_WHITE); y += 60;
    txt(r, x, y, "ENGINE:", C_DIM); txt(r, x + 200, y, "SDL3 PREMIUM", C_WHITE); y += 100;

    txt(r, x, y, "SPECIAL THANKS TO ALL DEFENDERS!", {255, 200, 0, 255}, 1.2f);
    
    txt(r, WIN_W/2 - 120, WIN_H - 100, "PRESS [ESC] TO RETURN", C_DIM);
}

static void renderBackgroundSelect(SDL_Renderer* r, const GameAssets& assets) {
    if (assets.t_landing) SDL_RenderTexture(r, assets.t_landing, nullptr, nullptr);
    else fR(r, 0, 0, WIN_W, WIN_H, {10, 12, 15, 255});
    fR(r, 0, 0, WIN_W, WIN_H, {0,0,0,130});
    fVignette(r);

    float time = (float)SDL_GetTicks() * 0.001f;
    float titleX = WIN_W/2 - 300;
    // 3D Shadow Effect
    txt(r, titleX + 4, 104, "VILLAGE UNDER ATTACK", {20, 20, 30, 200}, 3.5f);
    txt(r, titleX, 100, "VILLAGE UNDER ATTACK", C_WHITE, 3.5f);
    
    txt(r, WIN_W/2 - 140, 170, "CHOOSE YOUR DESTINY", {100, 200, 255, 180}, 1.3f);

    for (int i=0; i<(int)assets.backgrounds.size(); i++) {
        float w = 480, h = 80;
        float x = WIN_W/2 - w/2, y = 250 + i*100;
        bool sel = (i == assets.selectedBg);
        if (sel) x += 15; // Slide effect
        
        fGlassPanel(r, x, y, w, h, C_PLAYER, sel);
        txt(r, x + 30, y + 28, assets.backgrounds[i].name.c_str(), sel ? C_WHITE : C_DIM, 1.4f);
        
        if (sel) {
            float pulse = (sinf(time * 5.0f) + 1.0f) * 0.5f;
            txt(r, x + w - 120, y + 30, "READY", {(Uint8)(0),(Uint8)(200),(Uint8)(255),(Uint8)(150+105*pulse)}, 1.0f);
        }
    }

    float pulse = (sinf(SDL_GetTicks()*0.006f)+1.0f)*0.5f;
    txt(r, WIN_W/2-150, WIN_H-80, "PRESS [ENTER] TO DEPLOY", {(Uint8)(100+155*pulse),(Uint8)(150+105*pulse),255,255}, 1.2f);
}

class GameEngine {
public:
    State state = State::MAIN_MENU;
    GameAssets assets;
    float bestTime = 0.0f;
    SDL_AudioDeviceID audioDevice = 0;
    Board* board = nullptr;
    SDL_Window* win = nullptr;
    SDL_Renderer* ren = nullptr;
    bool running = true;

    GameEngine() {}
    ~GameEngine() {
        if (board) delete board;
        if (ren) SDL_DestroyRenderer(ren);
        if (win) SDL_DestroyWindow(win);
        SDL_Quit();
    }

    void run() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return;
        
        audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
        if (audioDevice != 0) {
            SDL_ResumeAudioDevice(audioDevice);
            g_audioDevice = audioDevice; // Crucial: sync global handle
        }

        win = SDL_CreateWindow(kWindowTitlePlaying, WIN_W, WIN_H, SDL_WINDOW_FULLSCREEN);
        ren = SDL_CreateRenderer(win, nullptr);
        SDL_SetRenderLogicalPresentation(ren, WIN_W, WIN_H, SDL_LOGICAL_PRESENTATION_LETTERBOX);
        SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
        
        initAssets(ren, assets);
        board = new Board(BOARD_W, BOARD_H);
        
        Uint64 lastT = SDL_GetTicks(); 
        Uint64 logicT = SDL_GetTicks();
        SDL_Event ev;

        while (running) {
            handleEvents(ev);

            Uint64 now = SDL_GetTicks();
            float dt = (now - lastT) / 1000.0f;
            lastT = now;

            updateVFX(dt, assets.selectedBg);
            if (board) board->updateVisuals(dt);
            
            updateLogic(now, logicT);
            render();
        }
    }

private:
    void handleEvents(SDL_Event& ev) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                auto sc = ev.key.scancode;
                if (sc == SDL_SCANCODE_ESCAPE) {
                    if (state == State::MAIN_MENU) running = false;
                    else {
                        state = State::MAIN_MENU;
                        if (win) SDL_SetWindowTitle(win, kWindowTitlePlaying);
                    }
                    continue;
                }
                
                if (state == State::MAIN_MENU) {
                    if (sc == SDL_SCANCODE_P) state = State::BACKGROUND_SELECT;
                    if (sc == SDL_SCANCODE_K) state = State::CREDITS;
                } else if (state == State::CREDITS) {
                } else if (state == State::GAME_OVER && sc == SDL_SCANCODE_R) {
                    delete board;
                    board = new Board(BOARD_W, BOARD_H);
                    g_wParts.clear();
                    state = State::MAIN_MENU;
                    if (win) SDL_SetWindowTitle(win, kWindowTitlePlaying);
                    continue;
                }
                
                if (state == State::BACKGROUND_SELECT) {
                    if (sc == SDL_SCANCODE_RETURN) state = State::INSTRUCTIONS;
                    int numBg = (int)assets.backgrounds.size();
                    if (sc == SDL_SCANCODE_UP) assets.selectedBg = (assets.selectedBg - 1 + numBg) % numBg;
                    if (sc == SDL_SCANCODE_DOWN) assets.selectedBg = (assets.selectedBg + 1) % numBg;
                } else if (state == State::INSTRUCTIONS) {
                    if (sc == SDL_SCANCODE_RETURN) state = State::PLAYING;
                } else if (state == State::PLAYING) {
                    int oldG = board->player->resources.gold, oldE = board->player->resources.elixir;
                    if (sc == SDL_SCANCODE_UP) board->player->move(0,-1,BOARD_W,BOARD_H);
                    if (sc == SDL_SCANCODE_DOWN) board->player->move(0,1,BOARD_W,BOARD_H);
                    if (sc == SDL_SCANCODE_LEFT) board->player->move(-1,0,BOARD_W,BOARD_H);
                    if (sc == SDL_SCANCODE_RIGHT) board->player->move(1,0,BOARD_W,BOARD_H);
                    if (sc == SDL_SCANCODE_C) {
                        board->collectResources();
                        if (board->player->resources.gold > oldG) {
                            addFloatText(cX(0) + board->player->visualX*CELL, cY(0) + board->player->visualY*CELL, "+Gold", C_GOLD);
                            playSoundPositional(assets.s_collect_gold, board->player->position, board->player->position);
                        }
                        if (board->player->resources.elixir > oldE) {
                            addFloatText(cX(0) + board->player->visualX*CELL, cY(0) + board->player->visualY*CELL, "+Elixir", C_ELIX);
                            playSoundPositional(assets.s_collect_elixir, board->player->position, board->player->position);
                        }
                    }
                    if (sc == SDL_SCANCODE_G) { if(board->buildGoldMine(board->player->position)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_E) { if(board->buildElixirCollector(board->player->position)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_W) { if(board->buildWall(board->player->position)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_B) { if(board->buildBarracks(board->player->position)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_A) { if(board->trainInBarracks(UnitType::ARCHER)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_H) { if(board->trainInBarracks(UnitType::BARBARIAN)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_M) { if(board->trainInBarracks(UnitType::MAGE)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_U) { if(board->upgradeBuilding(board->player->position)) playSoundPositional(assets.s_build, board->player->position, board->player->position); }
                    if (sc == SDL_SCANCODE_S) { board->saveGame("savegame.txt"); }
                    if (sc == SDL_SCANCODE_L) { board->loadGame("savegame.txt"); }
                }
            }
        }
    }

    void updateLogic(Uint64 now, Uint64& logicT) {
        if (now - logicT >= 100) {
            if (state == State::PLAYING && board->townHall) {
                bool oldWave = board->waveManager.isWaveActive;
                int oldDef = (int)board->defenders.size();
                int oldTH = board->townHall->health;
                
                board->update();
                
                if (board->waveManager.isWaveActive && !oldWave) {
                    char buf[64]; sprintf(buf, "WARNING: WAVE %d STARTING!", board->waveManager.waveCount);
                    addNote(buf, C_HP_R);
                    g_shake = 15.0f;
                }
                if (!board->waveManager.isWaveActive && oldWave) {
                    addNote("WAVE DEFEATED! RESTORE DEFENSES.", C_HP_G);
                }
                if ((int)board->defenders.size() > oldDef) {
                    addNote("TRAINING COMPLETE: UNIT DEPLOYED.", C_ARCHER);
                }
                if (board->player->resources.gold < 50 && board->tick % 500 == 0) {
                    addNote("LOW GOLD! CONSTRUCT MINES.", C_GOLD);
                }
                
                if (board->townHall) {
                    if (board->townHall->health < oldTH) { 
                        g_shake = 10.0f; 
                        spawnParts(cX(0) + board->townHall->visualX*CELL, cY(0) + board->townHall->visualY*CELL, C_HP_R); 
                        playSoundPositional(assets.s_hit, board->townHall->position, board->player->position);
                    }
                }
                if (board->gameOver) {
                    float currentTime = board->tick / 10.0f;
                    if (currentTime > bestTime) bestTime = currentTime;
                    state = State::GAME_OVER;
                    if (win) SDL_SetWindowTitle(win, kWindowTitleGameOver);
                }
            }
            logicT = now;
        }
    }

    void render() {
        SDL_SetRenderDrawColor(ren, 10, 10, 15, 255);
        SDL_RenderClear(ren);

        if (state == State::MAIN_MENU) {
            renderMainMenu(ren, assets, bestTime);
        } else if (state == State::CREDITS) {
            renderCredits(ren, assets);
        } else if (state == State::BACKGROUND_SELECT) {
            renderBackgroundSelect(ren, assets);
        } else if (state == State::INSTRUCTIONS) {
            renderInstructions(ren, assets);
        } else if (state == State::PLAYING) {
            renderBackground(ren, assets);
            renderBoard(ren, *board, assets);
            renderPanel(ren, *board);
        } else if (state == State::GAME_OVER) {
            renderBackground(ren, assets);
            renderBoard(ren, *board, assets);
            // Strong defeat tint + darken (cannot be mistaken for legacy "VILLAGE HAS FALLEN" layout)
            fR(ren, 0, 0, WIN_W, WIN_H, {40, 0, 0, 160});
            fR(ren, 0, 0, WIN_W, WIN_H, {0, 0, 0, 140});
            // Impossible-to-miss banner: if you do not see this bar, you are not running this build.
            {
                float bandY = WIN_H * 0.12f;
                fR(ren, 0, bandY, (float)WIN_W, 52, {180, 20, 20, 200});
                txt(ren, WIN_W/2 - 130, bandY + 12, "YOUR LEGACY ENDS HERE", {255, 255, 255, 255}, 1.2f);
            }

            float bw = 580, bh = 260;
            float bx = WIN_W / 2.0f - bw / 2.0f;
            float by = WIN_H / 2.0f - bh / 2.0f;
            fGlassPanel(ren, bx, by, bw, bh, C_HP_R, false);

            txt(ren, bx + 105, by + 28, "*** GAME  OVER ***", C_HP_R, 2.8f);
            txt(ren, bx + 72, by + 100, "Le TownHall est detruit !", C_WHITE, 1.35f);
            txt(ren, bx + 148, by + 140, "(Town Hall destroyed)", {220, 200, 200, 255}, 1.05f);
            txt(ren, bx + 55, by + 200, "[ R ] APPUYEZ POUR RECONSTRUIRE VOTRE VILLAGE", C_PLAYER, 1.05f);
        }

        SDL_RenderPresent(ren);
    }
};

int main(int argc, char* argv[]) {
    GameEngine engine;
    engine.run();
    return 0;
}

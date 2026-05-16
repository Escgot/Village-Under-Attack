#ifndef SDL_RENDER_H
#define SDL_RENDER_H

#include <SDL3/SDL.h>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

const int WIN_W = 1200;
const int WIN_H = 800;
const int CELL = 40;
const int MARGIN = 40;
const int BOARD_W = 20;
const int BOARD_H = 18;
const int PANEL_W = 360;

struct RGBA { Uint8 r, g, b, a; };
const RGBA C_WHITE = {255, 255, 255, 255};
const RGBA C_BLACK = {0, 0, 0, 255};
const RGBA C_DIM = {160, 165, 175, 255};
const RGBA C_GRID = {255, 255, 255, 15};
const RGBA C_PLAYER = {0, 200, 255, 255};
const RGBA C_RAIDER = {255, 80, 80, 255};
const RGBA C_BOMBER = {255, 200, 0, 255};
const RGBA C_ARCHER = {80, 255, 120, 255};
const RGBA C_BARB = {255, 150, 50, 255};
const RGBA C_GOLD = {255, 220, 0, 255};
const RGBA C_ELIX = {200, 50, 255, 255};
const RGBA C_HP_G = {40, 220, 80, 255};
const RGBA C_HP_Y = {255, 200, 0, 255};
const RGBA C_HP_R = {255, 50, 50, 255};
const RGBA C_PANEL_BG = {15, 18, 24, 230};
const RGBA C_PANEL_BD = {60, 65, 80, 255};
const RGBA C_BOARD_BG = {10, 12, 16, 255};

inline void setC(SDL_Renderer* r, RGBA c) { SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a); }
inline void fR(SDL_Renderer* r, float x, float y, float w, float h, RGBA c) {
    setC(r, c); SDL_FRect rect = {x, y, w, h}; SDL_RenderFillRect(r, &rect);
}
inline void dR(SDL_Renderer* r, float x, float y, float w, float h, RGBA c) {
    setC(r, c); SDL_FRect rect = {x, y, w, h}; SDL_RenderRect(r, &rect);
}
inline void fCircle(SDL_Renderer* r, int cx, int cy, int radius, RGBA c) {
    setC(r, c);
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius*radius)) SDL_RenderPoint(r, (float)(cx + dx), (float)(cy + dy));
        }
    }
}

inline float cX(int gx) { return (float)(MARGIN + gx * CELL); }
inline float cY(int gy) { return (float)(MARGIN + gy * CELL); }

struct Particle { float x, y, vx, vy, life, maxLife; RGBA c; float size; };
static std::vector<Particle> g_parts;

struct FloatText { float x, y, vy, life; std::string text; RGBA c; };
static std::vector<FloatText> g_fTexts;

struct WeatherPart { float x, y, vx, vy, life; RGBA c; float sz; };
static std::vector<WeatherPart> g_wParts;

struct Notification { std::string text; RGBA c; float life; };
static std::vector<Notification> g_notes;

static float g_shake = 0.0f;

inline void spawnParts(float x, float y, RGBA c, int n = 8) {
    for (int i = 0; i < n; i++) {
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float speed = (float)(rand() % 100) / 40.0f + 0.5f;
        g_parts.push_back({x, y, cosf(angle)*speed, sinf(angle)*speed, 1.0f, 1.0f, c, (float)(rand()%3 + 2)});
    }
}
inline void addFloatText(float x, float y, const char* t, RGBA c) {
    g_fTexts.push_back({x, y, -0.8f, 1.2f, t, c});
}
inline void addNote(const std::string& t, RGBA c) {
    g_notes.push_back({t, c, 4.0f});
}

inline void updateVFX(float dt, int bgType) {
    for (int i = (int)g_parts.size() - 1; i >= 0; i--) {
        g_parts[i].x += g_parts[i].vx; g_parts[i].y += g_parts[i].vy;
        g_parts[i].life -= dt * 1.5f; if (g_parts[i].life <= 0) g_parts.erase(g_parts.begin() + i);
    }
    if (g_shake > 0) g_shake -= dt * 15.0f; else g_shake = 0;
    for (int i = (int)g_fTexts.size() - 1; i >= 0; i--) {
        g_fTexts[i].y += g_fTexts[i].vy; g_fTexts[i].life -= dt;
        if (g_fTexts[i].life <= 0) g_fTexts.erase(g_fTexts.begin() + i);
    }
    if (g_wParts.size() < 60) {
        float wx = (float)(rand() % WIN_W), wy = -20; RGBA wc = {255,255,255,100}; float wvx = 0, wvy = 0, wsz = 2;
        if (bgType == 0) { // Magic
            wc = {100, 255, 200, 180}; wvx = (rand()%20-10)/10.0f; wvy = (rand()%10)/10.0f + 0.2f; wsz = 3; wy = (float)(rand()%WIN_H);
        } else if (bgType == 1) { // Forest
            wc = {80, 150, 40, 200}; wvx = 0.5f; wvy = 1.0f; wsz = 4;
        } else { // Desert
            wc = {230, 200, 150, 120}; wvx = -2.0f; wvy = 0.2f; wsz = 2; wx = (float)WIN_W + 20; wy = (float)(rand()%WIN_H);
        }
        g_wParts.push_back({wx, wy, wvx, wvy, 5.0f, wc, wsz});
    }
    for (int i = (int)g_wParts.size() - 1; i >= 0; i--) {
        g_wParts[i].x += g_wParts[i].vx; g_wParts[i].y += g_wParts[i].vy; g_wParts[i].life -= dt;
        if (g_wParts[i].x < -50 || g_wParts[i].x > WIN_W+50 || g_wParts[i].y > WIN_H+50 || g_wParts[i].life <= 0)
            g_wParts.erase(g_wParts.begin() + i);
    }
    for (int i = (int)g_notes.size() - 1; i >= 0; i--) {
        g_notes[i].life -= dt;
        if (g_notes[i].life <= 0) g_notes.erase(g_notes.begin() + i);
    }
}

inline void fGlow(SDL_Renderer* r, float x, float y, float radius, RGBA c) {
    for (int i = 0; i < 6; i++) {
        RGBA gc = c; gc.a = (Uint8)(c.a / (i + 2)); fCircle(r, (int)x, (int)y, (int)(radius + i * 5), gc);
    }
}
inline void fVignette(SDL_Renderer* r) {
    for (int i = 0; i < 15; i++) {
        RGBA c = {0, 0, 0, (Uint8)(120 - i*8)}; dR(r, (float)i, (float)i, (float)(WIN_W - i*2), (float)(WIN_H - i*2), c);
    }
}
inline void fRoundedRect(SDL_Renderer* r, float x, float y, float w, float h, float rad, RGBA c) {
    if (rad < 1) { fR(r, x, y, w, h, c); return; }
    // Center crosses
    fR(r, x + rad, y, w - rad * 2, h, c);
    fR(r, x, y + rad, w, h - rad * 2, c);
    // Corners
    fCircle(r, (int)(x + rad), (int)(y + rad), (int)rad, c);
    fCircle(r, (int)(x + w - rad), (int)(y + rad), (int)rad, c);
    fCircle(r, (int)(x + rad), (int)(y + h - rad), (int)rad, c);
    fCircle(r, (int)(x + w - rad), (int)(y + h - rad), (int)rad, c);
}

inline void hpBar(SDL_Renderer* r, float x, float y, float w, float h, int cur, int mx, bool liquid = false, RGBA overrideColor = {0,0,0,0}) {
    if (mx <= 0) return; 
    float rad = h / 2.0f;
    fRoundedRect(r, x, y, w, h, rad, {30, 32, 40, 220});
    float pct = std::max(0.0f, std::min(1.0f, (float)cur / (float)mx));
    if (pct > 0) {
        RGBA c = (overrideColor.a > 0) ? overrideColor : ((pct > 0.5f) ? C_HP_G : (pct > 0.25f) ? C_HP_Y : C_HP_R); 
        fRoundedRect(r, x + 1, y + 1, (w - 2) * pct, h - 2, rad - 1, c);
        if (liquid) {
            float pulse = (sinf(SDL_GetTicks() * 0.005f) + 1.0f) * 0.5f;
            fRoundedRect(r, x + 1, y + 1, (w - 2) * pct, (h - 2) / 2.0f, rad - 1, {255, 255, 255, (Uint8)(20 + 40 * pulse)});
        }
    }
    setC(r, {255, 255, 255, 30});
    SDL_FRect rect = {x, y, w, h};
    SDL_RenderRect(r, &rect);
}

inline void txt(SDL_Renderer* r, float x, float y, const char* s, RGBA c, float scl = 1.0f) {
    setC(r, {0, 0, 0, 180}); SDL_RenderDebugText(r, x + 2, y + 2, s);
    setC(r, c); SDL_RenderDebugText(r, x, y, s);
}

inline void dRoundedRect(SDL_Renderer* r, float x, float y, float w, float h, float rad, RGBA c) {
    if (rad < 1) { dR(r, x, y, w, h, c); return; }
    setC(r, c);
    // 4 Lines
    SDL_RenderLine(r, x + rad, y, x + w - rad, y);
    SDL_RenderLine(r, x + rad, y + h, x + w - rad, y + h);
    SDL_RenderLine(r, x, y + rad, x, y + h - rad);
    SDL_RenderLine(r, x + w, y + rad, x + w, y + h - rad);
    // 4 Corner Arcs (Simple points for now to ensure thickness match)
    for (int i=0; i<360; i+=2) {
        float a = (float)i * 3.14159f / 180.0f;
        float s = sinf(a), c_ = cosf(a);
        float qx = (i<90)?x+w-rad : (i<180)?x+rad : (i<270)?x+rad : x+w-rad;
        float qy = (i<90)?y+rad : (i<180)?y+rad : (i<270)?y+h-rad : y+h-rad;
        SDL_RenderPoint(r, qx + c_*rad, qy - s*rad);
    }
}

inline void fGlassPanel(SDL_Renderer* r, float x, float y, float w, float h, RGBA border, bool selected) {
    RGBA bg = {20, 25, 35, 180};
    float rad = 15.0f;
    if (selected) {
        bg = {40, 55, 80, 200};
        float pulse = (sinf(SDL_GetTicks() * 0.005f) + 1.0f) * 0.5f;
        border.a = (Uint8)(150 + 105 * pulse);
        for (int i = 1; i < 4; i++) {
            RGBA gc = border; gc.a = (Uint8)(50 / i);
            fRoundedRect(r, x - i, y - i, w + i * 2, h + i * 2, rad + i, gc);
        }
    }
    fRoundedRect(r, x, y, w, h, rad, bg);
    dRoundedRect(r, x, y, w, h, rad, border);
}

struct BgOption { std::string name, path; SDL_Texture* tex; };
struct SoundEffect { Uint8* buffer = nullptr; Uint32 length = 0; SDL_AudioSpec spec; };
struct GameAssets {
    std::vector<BgOption> backgrounds; int selectedBg = 0;
    SDL_Texture* t_townhall = nullptr; 
    SDL_Texture* t_townhall80 = nullptr; SDL_Texture* t_townhall40 = nullptr; SDL_Texture* t_townhall00 = nullptr;
    SDL_Texture* t_goldmine = nullptr; SDL_Texture* t_goldmine1 = nullptr;
    SDL_Texture* t_elixir = nullptr; SDL_Texture* t_elixir1 = nullptr;
    SDL_Texture* t_barracks = nullptr; SDL_Texture* t_barracks_on = nullptr;
    SDL_Texture* t_wall = nullptr; SDL_Texture* t_landing = nullptr;
    SDL_Texture* t_archer = nullptr;
    SDL_Texture* t_barbarian = nullptr;
    SDL_Texture* t_raider = nullptr;
    SDL_Texture* t_bomber = nullptr;
    SoundEffect s_build, s_collect_gold, s_collect_elixir, s_hit;
};

#endif

#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include <algorithm>

namespace UIHelpers {
    void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color);
    void DrawCircleRing(SDL_Renderer* renderer, int cx, int cy, int radius, int thickness, SDL_Color color);
    void FillRoundedRect(SDL_Renderer* renderer,
                                  int x, int y, int w, int h,
                                  int radius, SDL_Color color);
}
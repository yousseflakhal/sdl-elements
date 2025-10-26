#pragma once
#include <SDL2/SDL.h>
#include <cmath>
#include <memory>
#include <algorithm>

namespace UIHelpers {
    void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color);
    void DrawCircleRing(SDL_Renderer* renderer, int cx, int cy, int radius, int thickness, SDL_Color color);
    void FillRoundedRect(SDL_Renderer* renderer,
                                  int x, int y, int w, int h,
                                  int radius, SDL_Color color);
    inline SDL_Color AdjustBrightness(SDL_Color c, int delta) {
        auto clamp = [](int v){ return std::max(0, std::min(255, v)); };
        return SDL_Color{ Uint8(clamp(c.r + delta)), Uint8(clamp(c.g + delta)),
                          Uint8(clamp(c.b + delta)), c.a };
    }
    inline SDL_Color WithAlpha(SDL_Color c, Uint8 a) { c.a = a; return c; }
    void DrawShadowRoundedRect(SDL_Renderer* r, const SDL_Rect& rect, int radius, int offset, Uint8 alpha);
    inline float RelativeLuma(SDL_Color c) {
        auto lin = [](float u){ u/=255.0f; return (u<=0.04045f)? u/12.92f : powf((u+0.055f)/1.055f, 2.4f); };
        float R = lin(c.r), G = lin(c.g), B = lin(c.b);
        return 0.2126f*R + 0.7152f*G + 0.0722f*B;
    }
    inline SDL_Color Lighten(SDL_Color c, int delta) { return AdjustBrightness(c, +std::abs(delta)); }
    inline SDL_Color Darken (SDL_Color c, int delta) { return AdjustBrightness(c, -std::abs(delta)); }

    inline SDL_Color PickHoverColor(SDL_Color bg) {
        float L = RelativeLuma(bg);
        if (L < 0.10f) {
            return Lighten(bg, 12);
        } else {
            return Darken(bg, 12);
        }
    }

    inline SDL_Color PickFocusRing(SDL_Color bg) {
        float L = RelativeLuma(bg);
        return (L > 0.85f) ? Darken(bg, 40) : Lighten(bg, 40);
    }

    void StrokeRoundedRectOutside(SDL_Renderer* r,
                                  const SDL_Rect& innerRect,
                                  int radius,
                                  int thickness,
                                  SDL_Color ringColor,
                                  SDL_Color innerBg);
    void DrawRoundStrokeLine(SDL_Renderer* r, float x1, float y1, float x2, float y2, float thickness, SDL_Color color);
    void DrawCheckmark(SDL_Renderer* r, const SDL_Rect& box, float thickness, SDL_Color color, float pad);
    inline SDL_Color RGBA(int r, int g, int b, int a = 255) {
        auto clip = [](int v){ return static_cast<Uint8>(std::clamp(v, 0, 255)); };
        return SDL_Color{ clip(r), clip(g), clip(b), clip(a) };
    }

    void DrawChevronDown(SDL_Renderer* r, int cx, int cy, int width, int height, float thickness, SDL_Color color);
    struct SurfaceDeleter { 
        void operator()(SDL_Surface* s) const noexcept { 
            if (s) SDL_FreeSurface(s); 
        } 
    };
    
    struct TextureDeleter { 
        void operator()(SDL_Texture* t) const noexcept { 
            if (t) SDL_DestroyTexture(t); 
        } 
    };
    
    using UniqueSurface = std::unique_ptr<SDL_Surface, SurfaceDeleter>;
    using UniqueTexture = std::unique_ptr<SDL_Texture, TextureDeleter>;
    
    inline UniqueSurface MakeSurface(SDL_Surface* raw) {
        return UniqueSurface(raw);
    }
    
    inline UniqueTexture MakeTexture(SDL_Texture* raw) {
        return UniqueTexture(raw);
    }

}
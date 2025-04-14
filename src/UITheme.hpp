#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "UIConfig.hpp"

struct UIConfig;

struct UITheme {
    SDL_Color backgroundColor     = { 100, 100, 100, 255 };
    SDL_Color hoverColor          = { 130, 130, 130, 255 };
    SDL_Color borderColor         = { 200, 200, 200, 255 };
    SDL_Color borderHoverColor    = { 255, 255, 255, 255 };
    SDL_Color textColor           = { 255, 255, 255, 255 };
    SDL_Color placeholderColor    = { 160, 160, 160, 255 };
    SDL_Color cursorColor         = { 255, 255, 255, 255 };
    SDL_Color sliderTrackColor    = { 80, 80, 80, 255 };
    SDL_Color sliderThumbColor    = { 180, 180, 255, 255 };
    SDL_Color checkboxTickColor   = { 255, 255, 255, 255 };

    TTF_Font* font = nullptr;
};

TTF_Font* getThemeFont(const UITheme& theme); 
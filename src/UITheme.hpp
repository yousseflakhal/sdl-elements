#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "UIConfig.hpp"

struct UIConfig;

struct UITheme {
    SDL_Color backgroundColor   = {100,100,100,255};
    SDL_Color hoverColor        = {130,130,130,255};
    SDL_Color borderColor       = {200,200,200,255};
    SDL_Color borderHoverColor  = {255,255,255,255};
    SDL_Color textColor         = {255,255,255,255};
    SDL_Color placeholderColor  = {160,160,160,255};
    SDL_Color cursorColor       = {255,255,255,255};
    SDL_Color sliderTrackColor  = {80,80,80,255};
    SDL_Color sliderThumbColor  = {180,180,255,255};
    SDL_Color checkboxTickColor = {255,255,255,255};

    SDL_Color focusRing         = {13,110,253,178};
    SDL_Color selectionBg       = {0,120,215,120};

    int radiusSm = 6;
    int radiusMd = 10;
    int radiusLg = 16;
    int borderThin = 1;
    int borderThick = 2;
    int padSm = 6, padMd = 10, padLg = 16;

    TTF_Font* font = nullptr;

    UITheme();
    enum class ThemePreset { Neutral, Bootstrap, Solarized, Dracula, Fluent, Material };
    enum class ThemeMode   { Light, Dark, HighContrast };
    enum class Accent      { Blue, Red, Green, Orange, Purple, Teal, Pink, None };
    enum class Shape       { Square, Rounded, Pill };
    enum class Density     { Compact, Default, Comfortable };

    struct ThemeOptions {
        ThemePreset preset = ThemePreset::Neutral;
        ThemeMode   mode   = ThemeMode::Light;
        Accent      accent = Accent::Blue;
        Shape       shape  = Shape::Rounded;
        Density     density= Density::Default;
    };
};

UITheme MakeBootstrapLight();
UITheme MakeBootstrapDark();

TTF_Font* getThemeFont(const UITheme& theme); 
#pragma once
#include <SDL2/SDL_ttf.h>
#include "UITheme.hpp"

struct UITheme;
class UIConfig {
public:
    // NOTE: SDLFormUI does NOT take ownership. Caller must manage the lifetime (TTF_CloseFont).
    static void setDefaultFont(TTF_Font* font);
    static TTF_Font* getDefaultFont();

    static void setTheme(const UITheme& theme);
    static const UITheme& getTheme();
    static TTF_Font** getDefaultFontPtr();

private:
    static TTF_Font* defaultFont;
    static UITheme defaultTheme;
};
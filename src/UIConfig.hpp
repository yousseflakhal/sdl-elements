#pragma once
#include <SDL2/SDL_ttf.h>

class UIConfig {
public:
    static void setDefaultFont(TTF_Font* font);
    static TTF_Font* getDefaultFont();

private:
    static TTF_Font* defaultFont;
};
#pragma once
#include "UITheme.hpp"

struct UITextFieldStyle {
    int radius      = 10;
    int borderPx    = 1;

    SDL_Color bg{};
    SDL_Color fg{};
    SDL_Color placeholder{};

    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};

    SDL_Color selectionBg{};
    SDL_Color caret{};
};

struct UITextAreaStyle {
    int radius   = 10;
    int borderPx = 1;

    SDL_Color bg{};
    SDL_Color fg{};
    SDL_Color placeholder{};

    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};

    SDL_Color selectionBg{};
    SDL_Color caret{};
};

UITextFieldStyle MakeTextFieldStyle(const UITheme& t);
UITextAreaStyle MakeTextAreaStyle(const UITheme& t);

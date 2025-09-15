#pragma once
#include "UITheme.hpp"
#include <algorithm>

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

struct UIButtonStyle {
    int radius   = 10;
    int borderPx = 1;

    SDL_Color text{};
    SDL_Color border{};
    SDL_Color borderFocus{};
};

struct UICheckboxStyle {
    int boxSize   = 18;
    int radius    = 4;
    int borderPx  = 1;
    int spacingPx = 8;

    SDL_Color text{};
    SDL_Color boxBg{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color tick{};
};

UITextFieldStyle MakeTextFieldStyle(const UITheme& t);
UITextAreaStyle MakeTextAreaStyle(const UITheme& t);
UIButtonStyle MakeButtonStyle(const UITheme& t);
UICheckboxStyle MakeCheckboxStyle(const UITheme& t);
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

struct UIGroupBoxStyle {
    int  radius    = 10;
    int  borderPx  = 1;
    int  titlePadX = 8;
    int  titlePadY = 2;

    SDL_Color title;
    SDL_Color border;
    SDL_Color bg;
};

struct UIRadioStyle {
    int outerRadius     = 9;
    int ringThickness   = 5;
    int borderThickness = 2;
    int spacingPx       = 12;
    int gapTextPx       = 18;

    SDL_Color text{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color selected{};
};

UITextFieldStyle MakeTextFieldStyle(const UITheme& t);
UITextAreaStyle MakeTextAreaStyle(const UITheme& t);
UIButtonStyle MakeButtonStyle(const UITheme& t);
UICheckboxStyle MakeCheckboxStyle(const UITheme& t);
UIGroupBoxStyle MakeGroupBoxStyle(const UITheme& t);
UIRadioStyle MakeRadioStyle(const UITheme& t);
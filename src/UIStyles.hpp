#pragma once
#include "UITheme.hpp"
#include "UIStyle.hpp"
#include "UIHelpers.hpp"
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

struct UIComboBoxStyle {
    int radius   = 10;
    int borderPx = 1;
    int padX     = 10;
    SDL_Color fieldBg{};
    SDL_Color fieldFg{};
    SDL_Color placeholder{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color caret;
    SDL_Color menuBg{};
    SDL_Color menuBorder{};
    SDL_Color itemFg{};
    SDL_Color itemHoverBg{};
    SDL_Color itemSelectedBg{};
    SDL_Color itemSelectedFg{};
};

struct UISpinnerStyle {
    int radius   = 10;
    int borderPx = 1;
    int padX     = 10;
    SDL_Color fieldBg{};
    SDL_Color fieldBorder{};
    SDL_Color text{};
    SDL_Color btnBg{};
    SDL_Color btnBgHover{};
    SDL_Color btnBorder{};
    SDL_Color btnGlyph{};
    SDL_Color focusRing{};
};

struct UISliderStyle {
    int trackH = 6;
    SDL_Color track{};
    SDL_Color thumb{};
    SDL_Color thumbDrag{};
    SDL_Color focusRing{};
};

struct UILabelStyle {
    SDL_Color fg;
};

struct PopupStyle {
    SDL_Color bg;
    SDL_Color border;
    SDL_Color borderFocus;
    int       radius;
    int       borderPx;
    int       pad;
};

UITextFieldStyle MakeTextFieldStyle(const UITheme& t, const UIStyle& s);
UITextAreaStyle  MakeTextAreaStyle (const UITheme& t, const UIStyle& s);
UIButtonStyle    MakeButtonStyle   (const UITheme& t, const UIStyle& s);
UICheckboxStyle  MakeCheckboxStyle (const UITheme& t, const UIStyle& s);
UIGroupBoxStyle  MakeGroupBoxStyle (const UITheme& t, const UIStyle& s);
UIRadioStyle     MakeRadioStyle    (const UITheme& t, const UIStyle& s);
UIComboBoxStyle  MakeComboBoxStyle (const UITheme& t, const UIStyle& s);
UISpinnerStyle   MakeSpinnerStyle  (const UITheme& t, const UIStyle& s);
UISliderStyle    MakeSliderStyle   (const UITheme& t, const UIStyle&);
UISliderStyle    MakeSliderStyle   (const UITheme& t);
UILabelStyle     MakeLabelStyle    (const UITheme& th, const UIStyle&);
UILabelStyle     MakeLabelStyle    (const UITheme& th);
PopupStyle       MakePopupStyle    (const UITheme& th, const UIStyle& s);

UITextFieldStyle MakeTextFieldStyle(const UITheme& t);
UITextAreaStyle  MakeTextAreaStyle (const UITheme& t);
UIButtonStyle    MakeButtonStyle   (const UITheme& t);
UICheckboxStyle  MakeCheckboxStyle (const UITheme& t);
UIGroupBoxStyle  MakeGroupBoxStyle (const UITheme& t);
UIRadioStyle     MakeRadioStyle    (const UITheme& t);
UIComboBoxStyle  MakeComboBoxStyle (const UITheme& t);
UISpinnerStyle   MakeSpinnerStyle  (const UITheme& t);
PopupStyle       MakePopupStyle    (const UITheme& th);

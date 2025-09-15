#include "UIStyles.hpp"

UITextFieldStyle MakeTextFieldStyle(const UITheme& t) {
    UITextFieldStyle s;
    s.radius       = t.radiusMd;
    s.borderPx     = t.borderThin;

    s.bg           = t.backgroundColor;
    s.fg           = t.textColor;
    s.placeholder  = t.placeholderColor;

    s.border       = t.borderColor;
    s.borderHover  = t.borderHoverColor;
    s.borderFocus  = t.focusRing;

    s.selectionBg  = t.selectionBg;
    s.caret        = t.cursorColor;
    return s;
}

UITextAreaStyle MakeTextAreaStyle(const UITheme& t) {
    UITextAreaStyle s;
    s.radius       = t.radiusMd;
    s.borderPx     = t.borderThin;

    s.bg           = t.backgroundColor;
    s.fg           = t.textColor;
    s.placeholder  = t.placeholderColor;

    s.border       = t.borderColor;
    s.borderHover  = t.borderHoverColor;
    s.borderFocus  = t.focusRing;

    s.selectionBg  = t.selectionBg;
    s.caret        = t.cursorColor;
    return s;
}

UIButtonStyle MakeButtonStyle(const UITheme& t) {
    UIButtonStyle s;
    s.radius      = t.radiusMd;
    s.borderPx    = t.borderThin;
    s.text        = t.textColor;
    s.border      = t.borderColor;
    s.borderFocus = t.focusRing;
    return s;
}
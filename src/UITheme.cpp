#include "UITheme.hpp"

UITheme::UITheme() : font(nullptr) {}

TTF_Font* getThemeFont(const UITheme& theme) {
    return theme.font ? theme.font : UIConfig::getDefaultFont();
}

UITheme MakeLightTheme() {
    UITheme t;
    t.backgroundColor   = {255,255,255,255};
    t.textColor         = {33,37,41,255};
    t.placeholderColor  = {160,160,160,255};
    t.borderColor       = {206,212,218,255};
    t.borderHoverColor  = {173,181,189,255};
    t.focusRing         = {13,110,253,178};
    t.selectionBg       = {0,120,215,120};
    t.sliderTrackColor  = {222,226,230,255};
    t.sliderThumbColor  = {13,110,253,255};
    t.checkboxTickColor = {13,110,253,255};
    t.cursorColor       = {33,37,41,255};
    t.font = nullptr;
    return t;
}

UITheme MakeDarkTheme() {
    UITheme th;
    th.backgroundColor   = {18,18,20,255};
    th.hoverColor        = {28,28,32,255};
    th.borderColor       = {60,60,66,255};
    th.borderHoverColor  = {108,117,125,255};
    th.textColor         = {222,226,230,255};
    th.placeholderColor  = {134,142,150,255};
    th.cursorColor       = {255,255,255,255};
    th.selectionBg       = {13,110,253,120};
    th.focusRing         = {13,110,253,178};
    th.sliderTrackColor  = {44,44,50,255};
    th.sliderThumbColor  = {108,117,125,255};
    th.checkboxTickColor = {255,255,255,255};
    return th;
}

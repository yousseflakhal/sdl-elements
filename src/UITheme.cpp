#include "UITheme.hpp"
#include "UIConfig.hpp"

UITheme::UITheme() : font(nullptr) {}

TTF_Font* getThemeFont(const UITheme& theme) {
    return theme.font ? theme.font : UIConfig::getDefaultFont();
}

UITheme MakeBootstrapLight() {
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

    t.radiusSm = 6;  t.radiusMd = 10;  t.radiusLg = 16;
    t.borderThin = 1; t.borderThick = 2;
    t.padSm = 6; t.padMd = 10; t.padLg = 16;

    t.font = nullptr;
    return t;
}
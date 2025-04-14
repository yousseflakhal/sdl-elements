#include "UIConfig.hpp"

TTF_Font* UIConfig::defaultFont = nullptr;
UITheme UIConfig::defaultTheme;

void UIConfig::setDefaultFont(TTF_Font* font) {
    defaultFont = font;
}

TTF_Font* UIConfig::getDefaultFont() {
    return defaultFont;
}

void UIConfig::setTheme(const UITheme& theme) {
    defaultTheme = theme;
}

const UITheme& UIConfig::getTheme() {
    return defaultTheme;
}

TTF_Font** UIConfig::getDefaultFontPtr() {
    return &defaultFont;
}

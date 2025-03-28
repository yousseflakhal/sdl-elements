#include "UIConfig.hpp"

TTF_Font* UIConfig::defaultFont = nullptr;

void UIConfig::setDefaultFont(TTF_Font* font) {
    UIConfig::defaultFont = font;
}

TTF_Font* UIConfig::getDefaultFont() {
    return UIConfig::defaultFont;
}
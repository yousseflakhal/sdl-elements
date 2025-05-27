#include "UITheme.hpp"
#include "UIConfig.hpp"

UITheme::UITheme() : font(nullptr) {}

TTF_Font* getThemeFont(const UITheme& theme) {
    return theme.font ? theme.font : UIConfig::getDefaultFont();
}
#include "UITheme.hpp"
#include "UIConfig.hpp"

TTF_Font* getThemeFont(const UITheme& theme) {
    return theme.font ? theme.font : UIConfig::getDefaultFont();
}
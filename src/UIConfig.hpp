#pragma once
#include <SDL2/SDL_ttf.h>
#include <string_view>
#include "UITheme.hpp"
#include "LookIds.hpp"

struct UIStyle;
struct UITheme;

class UIConfig {
public:
    // NOTE: SDLFormUI does NOT take ownership. Caller must manage the lifetime (TTF_CloseFont).
    static void setDefaultFont(TTF_Font* font);
    static TTF_Font* getDefaultFont();

    static void setTheme(const UITheme& theme);
    static const UITheme& getTheme();

    static void setStyle(const UIStyle& style);
    static const UIStyle& getStyle();

    static void setStyle(StyleId id);
    static void setTheme(ThemeId id);
    static void setLook(StyleId style, ThemeId theme);

    static void setStyle(std::string_view name);
    static void setTheme(std::string_view name);
    static void setLook(std::string_view styleName,
                        std::string_view themeName);

    using StyleFactory = UIStyle(*)();
    using ThemeFactory = UITheme(*)();
    static void registerStyle(std::string_view name, StyleFactory f);
    static void registerTheme(std::string_view name, ThemeFactory f);

    static TTF_Font** getDefaultFontPtr();

private:
    static TTF_Font* defaultFont;
    static UITheme   defaultTheme;

    static UIStyle   defaultStyle;
};

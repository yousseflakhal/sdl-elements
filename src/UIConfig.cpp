#include "UIConfig.hpp"
#include "UITheme.hpp"
#include "UIStyle.hpp"
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <string>

TTF_Font* UIConfig::defaultFont = nullptr;
UITheme   UIConfig::defaultTheme;
UIStyle   UIConfig::defaultStyle = MakeClassicStyle();

static UIStyle styleFromEnum(StyleId id) {
    switch (id) {
        case StyleId::Classic: return MakeClassicStyle();
        case StyleId::Minimal: return MakeMinimalStyle();
    }
    return MakeClassicStyle();
}

static UITheme themeFromEnum(ThemeId id) {
    switch (id) {
        case ThemeId::Light:        return MakeLightTheme();
        case ThemeId::Dark:         return MakeDarkTheme();
        case ThemeId::HighContrast: return MakeHighContrastTheme();
    }
    return MakeLightTheme();
}

struct NoThrowHash {
    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string>{}(s);
    }
};
struct NoThrowEq {
    bool operator()(const std::string& a, const std::string& b) const noexcept {
        return a == b;
    }
};

static std::unordered_map<std::string, UIConfig::StyleFactory, NoThrowHash, NoThrowEq> gStyleReg = {
    {"classic", &MakeClassicStyle},
    {"minimal", &MakeMinimalStyle},
};


static std::unordered_map<std::string, UIConfig::ThemeFactory, NoThrowHash, NoThrowEq> gThemeReg = {
    {"light",  &MakeLightTheme},
    {"dark",   &MakeDarkTheme},
    {"high-contrast", &MakeHighContrastTheme},
    {"classic-light",  &MakeLightTheme},
    {"classic-dark",   &MakeDarkTheme},
    {"bootstrap-light",&MakeLightTheme},
    {"bootstrap-dark", &MakeDarkTheme},
};

static std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}

void UIConfig::setDefaultFont(TTF_Font* font) { defaultFont = font; }
TTF_Font* UIConfig::getDefaultFont() { return defaultFont; }
TTF_Font** UIConfig::getDefaultFontPtr() { return &defaultFont; }

void UIConfig::setTheme(const UITheme& theme) { defaultTheme = theme; }
const UITheme& UIConfig::getTheme() { return defaultTheme; }

void UIConfig::setStyle(const UIStyle& style) { defaultStyle = style; }
const UIStyle& UIConfig::getStyle() { return defaultStyle; }

void UIConfig::setStyle(StyleId id) { setStyle(styleFromEnum(id)); }
void UIConfig::setTheme(ThemeId id) { setTheme(themeFromEnum(id)); }
void UIConfig::setLook(StyleId s, ThemeId t) {
    setStyle(styleFromEnum(s));
    setTheme(themeFromEnum(t));
}

void UIConfig::setStyle(std::string_view name) {
    auto key = lower(name);
    if (auto it = gStyleReg.find(key); it != gStyleReg.end())
        setStyle(it->second());
}

void UIConfig::setTheme(std::string_view name) {
    auto key = lower(name);
    if (auto it = gThemeReg.find(key); it != gThemeReg.end())
        setTheme(it->second());
}

void UIConfig::setLook(std::string_view styleName, std::string_view themeName) {
    setStyle(styleName);
    setTheme(themeName);
}

void UIConfig::registerStyle(std::string_view name, StyleFactory f) {
    gStyleReg[lower(name)] = f;
}

void UIConfig::registerTheme(std::string_view name, ThemeFactory f) {
    gThemeReg[lower(name)] = f;
}

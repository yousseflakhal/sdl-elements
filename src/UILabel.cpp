#include "UILabel.hpp"
#include "UIConfig.hpp"
#include "UITheme.hpp"
#include "UIStyles.hpp"
#include <cstring>

UILabel::UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font)
    : text(text), font(font) {
    bounds = { x, y, w, h };
}

void UILabel::invalidateCache() const {
    cachedTexture.reset();
    cachedText.clear();
    cachedColor = {0, 0, 0, 0};
    cachedFont = nullptr;
    cachedWidth = 0;
    cachedHeight = 0;
}

void UILabel::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeLabelStyle(th, ds);
    TTF_Font* activeFont = font ? font : getThemeFont(th);
    if (!activeFont) return;

    SDL_Color txtCol = (color.a != 0) ? color : st.fg;

    bool needsRebuild = !cachedTexture || 
                        cachedText != text || 
                        cachedFont != activeFont ||
                        std::memcmp(&cachedColor, &txtCol, sizeof(SDL_Color)) != 0;
    
    if (needsRebuild) {
        invalidateCache();
        
        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(activeFont, text.c_str(), txtCol)
        );
        
        if (!surface) return;
        
        cachedTexture = UIHelpers::MakeTexture(
            SDL_CreateTextureFromSurface(renderer, surface.get())
        );
        if (!cachedTexture) return;
        
        cachedText = text;
        cachedColor = txtCol;
        cachedFont = activeFont;
        cachedWidth = surface->w;
        cachedHeight = surface->h;
    }
    
    SDL_Rect dstRect = {
        bounds.x,
        bounds.y + (bounds.h - cachedHeight) / 2,
        cachedWidth,
        cachedHeight
    };
    
    SDL_RenderCopy(renderer, cachedTexture.get(), nullptr, &dstRect);
}

UILabel* UILabel::setColor(SDL_Color c) {
    if (std::memcmp(&color, &c, sizeof(SDL_Color)) != 0) {
        color = c;
        invalidateCache();
    }
    return this;
}

SDL_Color UILabel::getColor() const {
    return color;
}

void UILabel::setText(const std::string& newText) {
    if (text != newText) {
        text = newText;
        invalidateCache();
    }
}
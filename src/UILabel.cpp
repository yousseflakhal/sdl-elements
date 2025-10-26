#include "UILabel.hpp"
#include "UIConfig.hpp"
#include "UITheme.hpp"
#include "UIStyles.hpp"

UILabel::UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font)
    : text(text), font(font) {
    bounds = { x, y, w, h };
}

void UILabel::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeLabelStyle(th, ds);
    TTF_Font* activeFont = font ? font : getThemeFont(th);
    if (!activeFont) return;

    SDL_Color txtCol = (color.a != 0) ? color : st.fg;

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, text.c_str(), txtCol)
    );
    if (!surface) return;
    
    auto texture = UIHelpers::MakeTexture(
        SDL_CreateTextureFromSurface(renderer, surface.get())
    );
    
    SDL_Rect dstRect = {
        bounds.x,
        bounds.y + (bounds.h - surface->h) / 2,
        surface->w,
        surface->h
    };
    
    SDL_RenderCopy(renderer, texture.get(), nullptr, &dstRect);
}

UILabel* UILabel::setColor(SDL_Color c) {
    color = c;
    return this;
}

SDL_Color UILabel::getColor() const {
    return color;
}

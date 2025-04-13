#include "UILabel.hpp"
#include "UIConfig.hpp"

UILabel::UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font)
    : text(text), font(font)
{
    bounds = { x, y, w, h };
}

void UILabel::render(SDL_Renderer* renderer) {
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) {
        SDL_Log("UILabel: No valid font to render text.");
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, text.c_str(), color);
    if (!textSurface) {
        SDL_Log("UILabel: Failed to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!texture) {
        SDL_Log("UILabel: Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect dstRect = {
        bounds.x,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };

    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);
}


UILabel* UILabel::setColor(SDL_Color newColor) {
    color = newColor;
    return this;
}

SDL_Color UILabel::getColor() const {
    return color;
}
#include "UIButton.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

UIButton::UIButton(const std::string& text, int x, int y, int w, int h, TTF_Font* f)
    : label(text), font(f)
{
    bounds = { x, y, w, h };
}

void UIButton::setOnClick(std::function<void()> callback) {
    onClick = callback;
}

void UIButton::setText(const std::string& newText) {
    label = newText;
}

const std::string& UIButton::getText() const {
    return label;
}

void UIButton::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            pressed = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (onClick) onClick();
        }
        pressed = false;
    }
}

void UIButton::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = { mx, my };
    hovered = SDL_PointInRect(&mousePoint, &bounds);
}


void UIButton::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();

    SDL_Color bg     = hovered ? theme.hoverColor       : theme.backgroundColor;
    SDL_Color border = hovered ? theme.borderHoverColor : theme.borderColor;
    SDL_Color txt    = theme.textColor;

    if (customBgColor)     bg = *customBgColor;
    if (customBorderColor) border = *customBorderColor;
    if (customTextColor)   txt = *customTextColor;

    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, border.a);
    SDL_RenderDrawRect(renderer, &bounds);

    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), txt);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textRect = {
        bounds.x + (bounds.w - textSurface->w) / 2,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}


void UIButton::setFont(TTF_Font* f) {
    font = f;
}

bool UIButton::isHovered() const {
    return hovered;
}

UIButton* UIButton::setTextColor(SDL_Color c) {
    customTextColor = c;
    return this;
}

UIButton* UIButton::setBackgroundColor(SDL_Color c) {
    customBgColor = c;
    return this;
}

UIButton* UIButton::setBorderColor(SDL_Color c) {
    customBorderColor = c;
    return this;
}
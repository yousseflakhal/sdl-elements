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

    SDL_SetRenderDrawColor(renderer,
                           hovered ? theme.hoverColor.r : theme.backgroundColor.r,
                           hovered ? theme.hoverColor.g : theme.backgroundColor.g,
                           hovered ? theme.hoverColor.b : theme.backgroundColor.b,
                           hovered ? theme.hoverColor.a : theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer,
                           hovered ? theme.borderHoverColor.r : theme.borderColor.r,
                           hovered ? theme.borderHoverColor.g : theme.borderColor.g,
                           hovered ? theme.borderHoverColor.b : theme.borderColor.b,
                           hovered ? theme.borderHoverColor.a : theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) {
        SDL_Log("UIButton: No valid font to render label.");
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) {
        SDL_Log("UIButton: Failed to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("UIButton: Failed to create texture from surface: %s", SDL_GetError());
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
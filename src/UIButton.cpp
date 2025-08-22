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

    SDL_Color bg   = hovered ? theme.hoverColor       : theme.backgroundColor;
    SDL_Color txt  = theme.textColor;
    if (customBgColor)   bg  = *customBgColor;
    if (customTextColor) txt = *customTextColor;

    // exact AA fill
    UIHelpers::FillRoundedRect(renderer,
        bounds.x, bounds.y, bounds.w, bounds.h,
        cornerRadius, bg);

    // text
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    SDL_Surface* s = TTF_RenderText_Blended(activeFont, label.c_str(), txt);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t) { SDL_FreeSurface(s); return; }

    SDL_Rect r = { bounds.x + (bounds.w - s->w)/2,
                   bounds.y + (bounds.h - s->h)/2,
                   s->w, s->h };
    SDL_RenderCopy(renderer, t, nullptr, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
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
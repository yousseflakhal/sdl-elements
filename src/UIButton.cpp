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
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            pressed = true;
            if (focusable) focused = true;
        } else if (focusable) {
            focused = false;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
        int mx = e.button.x, my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (onClick) onClick();
        }
        pressed = false;
    }

    if (focusable && focused && e.type == SDL_KEYDOWN && enabled) {
        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
            pressed = true;
        }
    } else if (focusable && focused && e.type == SDL_KEYUP) {
        if ((e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) && pressed) {
            if (onClick) onClick();
            pressed = false;
        }
    }
}

void UIButton::update(float) {
    if (!enabled) { hovered = false; pressed = false; return; }
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = { mx, my };
    hovered = SDL_PointInRect(&mousePoint, &bounds);
}


void UIButton::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    SDL_Color baseBg     = customBgColor     ? *customBgColor     : theme.backgroundColor;
    SDL_Color baseText   = customTextColor   ? *customTextColor   : theme.textColor;
    SDL_Color baseBorder = customBorderColor ? *customBorderColor : theme.borderColor;

    bool drawShadow = true;
    Uint8 globalAlpha = 255;
    if (!enabled) { globalAlpha = 128; drawShadow = false; }

    SDL_Color bg = baseBg;
    if (enabled) {
        if (pressed)      bg = UIHelpers::Darken(baseBg, 10);
        else if (hovered) bg = UIHelpers::PickHoverColor(baseBg);
    }
    bg.a = globalAlpha;

    if (drawShadow && !pressed) {
        UIHelpers::DrawShadowRoundedRect(renderer, bounds, cornerRadius, 2, 64);
    }

    if (focusable && focused) {
        SDL_Color ring = UIHelpers::PickFocusRing(baseBg);
        ring.a = (Uint8)std::min<int>(178, globalAlpha);
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, cornerRadius, 2, ring, bg);
    }

    SDL_Rect dst = bounds;
    if (pressed) {
        dst.y += pressOffset;
        dst.x += 1;
        dst.w -= 2; dst.h -= 2;
    }

    UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, cornerRadius, bg);

    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    SDL_Color txt = baseText;
    txt.a = globalAlpha;

    SDL_Surface* s = TTF_RenderText_Blended(activeFont, label.c_str(), txt);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t) { SDL_FreeSurface(s); return; }
    SDL_Rect r = { dst.x + (dst.w - s->w)/2, dst.y + (dst.h - s->h)/2, s->w, s->h };
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
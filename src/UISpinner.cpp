#include "UISpinner.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <sstream>
#include <iomanip>

UISpinner::UISpinner(int x, int y, int w, int h, int& bind, int min, int max, int step)
    : value(bind), minValue(min), maxValue(max), step(step)
{
    bounds = { x, y, w, h };
}

void UISpinner::setFont(TTF_Font* f) {
    font = f;
}

void UISpinner::setOnChange(std::function<void(int)> callback) {
    onChange = callback;
}

bool UISpinner::isHovered() const {
    return hoveredMinus || hoveredPlus;
}

void UISpinner::handleEvent(const SDL_Event& e) {
    int mx = e.button.x;
    int my = e.button.y;
    SDL_Point point = { mx, my };

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (SDL_PointInRect(&point, &minusRect)) {
            if (value.get() > minValue) {
                value.get() -= step;
                if (onChange) onChange(value.get());
            }
            heldButton = HeldButton::DECREMENT;
            pressStartTime = SDL_GetTicks();
            lastStepTime = pressStartTime;
        } else if (SDL_PointInRect(&point, &plusRect)) {
            if (value.get() < maxValue) {
                value.get() += step;
                if (onChange) onChange(value.get());
            }
            heldButton = HeldButton::INCREMENT;
            pressStartTime = SDL_GetTicks();
            lastStepTime = pressStartTime;
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        heldButton = HeldButton::NONE;
    }
}

void UISpinner::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };

    hoveredMinus = SDL_PointInRect(&point, &minusRect);
    hoveredPlus = SDL_PointInRect(&point, &plusRect);

    Uint32 now = SDL_GetTicks();
    if (heldButton != HeldButton::NONE && now - pressStartTime > 400 && now - lastStepTime > 100) {
        if (heldButton == HeldButton::INCREMENT && value.get() < maxValue) {
            value.get() += step;
            if (onChange) onChange(value.get());
        } else if (heldButton == HeldButton::DECREMENT && value.get() > minValue) {
            value.get() -= step;
            if (onChange) onChange(value.get());
        }
        lastStepTime = now;
    }
}

void UISpinner::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) return;

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect  = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };
    SDL_Rect centerRect = {
        bounds.x + bounds.h,
        bounds.y,
        bounds.w - 2 * bounds.h,
        bounds.h
    };

    SDL_Color minusColor = hoveredMinus ? theme.hoverColor : theme.borderColor;
    SDL_Color plusColor  = hoveredPlus  ? theme.hoverColor : theme.borderColor;
    SDL_Color centerColor = theme.borderColor;

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, centerColor.r, centerColor.g, centerColor.b, centerColor.a);
    SDL_RenderDrawRect(renderer, &centerRect);

    SDL_SetRenderDrawColor(renderer, minusColor.r, minusColor.g, minusColor.b, 255);
    SDL_RenderDrawLine(renderer, minusRect.x, minusRect.y, minusRect.x + minusRect.w, minusRect.y);
    SDL_RenderDrawLine(renderer, minusRect.x, minusRect.y, minusRect.x, minusRect.y + minusRect.h);
    SDL_RenderDrawLine(renderer, minusRect.x, minusRect.y + minusRect.h - 1, minusRect.x + minusRect.w, minusRect.y + minusRect.h - 1);

    SDL_SetRenderDrawColor(renderer, plusColor.r, plusColor.g, plusColor.b, 255);
    SDL_RenderDrawLine(renderer, plusRect.x, plusRect.y, plusRect.x + plusRect.w, plusRect.y);
    SDL_RenderDrawLine(renderer, plusRect.x + plusRect.w - 1, plusRect.y, plusRect.x + plusRect.w - 1, plusRect.y + plusRect.h);
    SDL_RenderDrawLine(renderer, plusRect.x, plusRect.y + plusRect.h - 1, plusRect.x + plusRect.w, plusRect.y + plusRect.h - 1);

    SDL_SetRenderDrawColor(renderer, minusColor.r, minusColor.g, minusColor.b, 255);
    SDL_RenderDrawLine(renderer,
        minusRect.x + minusRect.w / 4,
        minusRect.y + minusRect.h / 2,
        minusRect.x + 3 * minusRect.w / 4,
        minusRect.y + minusRect.h / 2);

    SDL_SetRenderDrawColor(renderer, plusColor.r, plusColor.g, plusColor.b, 255);
    SDL_RenderDrawLine(renderer,
        plusRect.x + plusRect.w / 2,
        plusRect.y + plusRect.h / 4,
        plusRect.x + plusRect.w / 2,
        plusRect.y + 3 * plusRect.h / 4);
    SDL_RenderDrawLine(renderer,
        plusRect.x + plusRect.w / 4,
        plusRect.y + plusRect.h / 2,
        plusRect.x + 3 * plusRect.w / 4,
        plusRect.y + plusRect.h / 2);

    std::ostringstream oss;
    oss << value.get();
    SDL_Surface* surface = TTF_RenderText_Blended(activeFont, oss.str().c_str(), theme.textColor);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {
            centerRect.x + (centerRect.w - surface->w) / 2,
            centerRect.y + (centerRect.h - surface->h) / 2,
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

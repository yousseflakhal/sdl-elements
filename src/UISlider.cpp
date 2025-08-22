#include "UISlider.hpp"
#include "UIConfig.hpp"
#include <algorithm>
#include <sstream>

UISlider::UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max)
    : label(label), linkedValue(bind), minValue(min), maxValue(max)
{
    bounds = { x, y, w, h };
}

void UISlider::handleEvent(const SDL_Event& e) {
    int mx = 0, my = 0;
    if (e.type == SDL_MOUSEMOTION) {
        mx = e.motion.x; my = e.motion.y;
    } else {
        mx = e.button.x; my = e.button.y;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            dragging = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        dragging = false;
    } else if (e.type == SDL_MOUSEMOTION && dragging && linkedValue) {
        float relX = mx - bounds.x;
        float t = std::clamp(relX / float(bounds.w), 0.0f, 1.0f);
        linkedValue.get() = minValue + t * (maxValue - minValue);
    }
}

bool UISlider::isHovered() const {
    return hovered;
}

void UISlider::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };
    hovered = SDL_PointInRect(&point, &bounds);
}

void UISlider::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* font = getThemeFont(getTheme());
    if (!font || !linkedValue) return;

    SDL_Surface* labelSurface = TTF_RenderText_Blended(font, label.c_str(), theme.textColor);
    if (labelSurface) {
        SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
        SDL_Rect labelRect = { bounds.x, bounds.y - labelSurface->h - 4, labelSurface->w, labelSurface->h };
        SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
        SDL_FreeSurface(labelSurface);
        SDL_DestroyTexture(labelTexture);
    }

    SDL_Rect track = {
        bounds.x,
        bounds.y + bounds.h / 2 - 4,
        bounds.w,
        8
    };
    SDL_SetRenderDrawColor(renderer, theme.sliderTrackColor.r, theme.sliderTrackColor.g, theme.sliderTrackColor.b, theme.sliderTrackColor.a);
    SDL_RenderFillRect(renderer, &track);

    float t = (linkedValue.get() - minValue) / (maxValue - minValue);
    int thumbX = bounds.x + static_cast<int>(t * bounds.w);
    SDL_Rect thumb = { thumbX - 6, bounds.y + bounds.h / 2 - 10, 12, 20 };
    SDL_SetRenderDrawColor(renderer, theme.sliderThumbColor.r, theme.sliderThumbColor.g, theme.sliderThumbColor.b, theme.sliderThumbColor.a);
    SDL_RenderFillRect(renderer, &thumb);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << linkedValue.get();
    SDL_Surface* valueSurface = TTF_RenderText_Blended(font, oss.str().c_str(), theme.textColor);
    if (valueSurface) {
        SDL_Texture* valueTexture = SDL_CreateTextureFromSurface(renderer, valueSurface);
        SDL_Rect valueRect = {
            bounds.x + bounds.w + 10,
            bounds.y + (bounds.h - valueSurface->h) / 2,
            valueSurface->w,
            valueSurface->h
        };
        SDL_RenderCopy(renderer, valueTexture, nullptr, &valueRect);
        SDL_FreeSurface(valueSurface);
        SDL_DestroyTexture(valueTexture);
    }
}
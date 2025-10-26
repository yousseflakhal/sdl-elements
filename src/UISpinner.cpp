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
            int newValue = value.get() + step;
            if (newValue <= maxValue) {
                value.get() = newValue;
                if (onChange) onChange(value.get());
            }
        } else if (heldButton == HeldButton::DECREMENT && value.get() > minValue) {
            int newValue = value.get() - step;
            if (newValue >= minValue) {
                value.get() = newValue;
                if (onChange) onChange(value.get());
            }
        }
        lastStepTime = now;
    }
}

void UISpinner::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeSpinnerStyle(th, ds);
    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const int effRadius   = st.radius;
    const int effBorderPx = st.borderPx;

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect  = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };
    SDL_Rect centerRect{ bounds.x + bounds.h, bounds.y, bounds.w - 2*bounds.h, bounds.h };

    UIHelpers::FillRoundedRect(renderer, bounds.x, bounds.y, bounds.w, bounds.h, effRadius, st.fieldBorder);
    SDL_Rect inner = { bounds.x + effBorderPx, bounds.y + effBorderPx, bounds.w - 2*effBorderPx, bounds.h - 2*effBorderPx };
    UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), st.fieldBg);

    SDL_SetRenderDrawColor(renderer, st.fieldBorder.r, st.fieldBorder.g, st.fieldBorder.b, st.fieldBorder.a);
    SDL_RenderDrawLine(renderer, centerRect.x, bounds.y, centerRect.x, bounds.y + bounds.h - 1);
    SDL_RenderDrawLine(renderer, centerRect.x + centerRect.w, bounds.y, centerRect.x + centerRect.w, bounds.y + bounds.h - 1);

    if (hoveredMinus) {
        SDL_Color hb = st.btnBgHover;
        SDL_SetRenderDrawColor(renderer, hb.r, hb.g, hb.b, hb.a);
        SDL_RenderFillRect(renderer, &minusRect);
    }
    if (hoveredPlus) {
        SDL_Color hb = st.btnBgHover;
        SDL_SetRenderDrawColor(renderer, hb.r, hb.g, hb.b, hb.a);
        SDL_RenderFillRect(renderer, &plusRect);
    }

    SDL_Color glyph = st.btnGlyph;
    SDL_SetRenderDrawColor(renderer, glyph.r, glyph.g, glyph.b, glyph.a);
    SDL_RenderDrawLine(renderer, minusRect.x + minusRect.w/4, minusRect.y + minusRect.h/2, minusRect.x + 3*minusRect.w/4, minusRect.y + minusRect.h/2);
    SDL_RenderDrawLine(renderer, plusRect.x + plusRect.w/4, plusRect.y + plusRect.h/2, plusRect.x + 3*plusRect.w/4, plusRect.y + plusRect.h/2);
    SDL_RenderDrawLine(renderer, plusRect.x + plusRect.w/2, plusRect.y + plusRect.h/4, plusRect.x + plusRect.w/2, plusRect.y + 3*plusRect.h/4);

    std::ostringstream oss; 
    oss << value.get();
    SDL_Color txtCol = st.text;
    
    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, oss.str().c_str(), txtCol)
    );
    
    if (surface) {
        auto texture = UIHelpers::MakeTexture(
            SDL_CreateTextureFromSurface(renderer, surface.get())
        );
        
        SDL_Rect textRect = { 
            centerRect.x + (centerRect.w - surface->w)/2, 
            centerRect.y + (centerRect.h - surface->h)/2, 
            surface->w, 
            surface->h 
        };
        
        SDL_RenderCopy(renderer, texture.get(), nullptr, &textRect);
    }
}

#include "UICheckbox.hpp"
#include "UIConfig.hpp"

UICheckbox::UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f)
    : label(label), linkedValue(bind), font(f)
{
    bounds = { x, y, w, h };
}

void UICheckbox::setFont(TTF_Font* f) {
    font = f;
}

void UICheckbox::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            linkedValue.get() = !linkedValue.get();
        }
    }
}

bool UICheckbox::isHovered() const {
    return hovered;
}

void UICheckbox::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = { mx, my };
    hovered = SDL_PointInRect(&mousePoint, &bounds);
}

void UICheckbox::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) {
        SDL_Log("UICheckbox: No valid font for rendering.");
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) {
        SDL_Log("UICheckbox: Failed to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("UICheckbox: Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textW = textSurface->w;
    int textH = textSurface->h;
    int margin = 10;
    int boxSize = 20;

    int totalWidth = textW + margin + boxSize;
    int totalHeight = std::max(textH, boxSize);
    bounds.w = totalWidth;
    bounds.h = totalHeight;

    SDL_Rect textRect = {
        bounds.x,
        bounds.y + (bounds.h - textH) / 2,
        textW,
        textH
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_Rect box = {
        bounds.x + textW + margin,
        bounds.y + (bounds.h - boxSize) / 2,
        boxSize,
        boxSize
    };

    SDL_SetRenderDrawColor(renderer,
        hovered ? theme.borderHoverColor.r : theme.borderColor.r,
        hovered ? theme.borderHoverColor.g : theme.borderColor.g,
        hovered ? theme.borderHoverColor.b : theme.borderColor.b,
        hovered ? theme.borderHoverColor.a : theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &box);

    if (linkedValue.get()) {
        SDL_SetRenderDrawColor(renderer,
            theme.checkboxTickColor.r,
            theme.checkboxTickColor.g,
            theme.checkboxTickColor.b,
            theme.checkboxTickColor.a);
        SDL_Rect inner = {
            box.x + 4,
            box.y + 4,
            boxSize - 8,
            boxSize - 8
        };
        SDL_RenderFillRect(renderer, &inner);
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
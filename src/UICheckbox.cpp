#include "UICheckbox.hpp"
#include "UIConfig.hpp"

UICheckbox::UICheckbox(const std::string& label, int x, int y, int w, int h, bool* bind)
    : label(label), linkedValue(bind)
{
    bounds = { x, y, w, h };
}

void UICheckbox::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (linkedValue) *linkedValue = !(*linkedValue);
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
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font) return;

    SDL_Color textColor = { 255, 255, 255, 255 };

    SDL_Surface* textSurface = TTF_RenderText_Blended(font, label.c_str(), textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
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
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_Rect box = {
        bounds.x + textW + margin,
        bounds.y + (bounds.h - boxSize) / 2,
        boxSize,
        boxSize
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &box);

    if (linkedValue && *linkedValue) {
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

#include "UIRadioButton.hpp"

UIRadioButton::UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font)
    : label(label), id(id), group(group), font(font)
{
    bounds = { x, y, w, h };
}

void UIRadioButton::setFont(TTF_Font* f) {
    font = f;
}

int UIRadioButton::getID() const {
    return id;
}

bool UIRadioButton::isHovered() const {
    return hovered;
}

void UIRadioButton::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (group) group->select(id);
        }
    }
}

void UIRadioButton::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point p = { mx, my };
    hovered = SDL_PointInRect(&p, &bounds);
}

void UIRadioButton::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    int radius = 8;
    int cx = bounds.x + 10;
    int cy = bounds.y + bounds.h / 2;

    SDL_Rect outer = { cx - radius, cy - radius, radius * 2, radius * 2 };
    SDL_SetRenderDrawColor(renderer,
        hovered ? theme.borderHoverColor.r : theme.borderColor.r,
        hovered ? theme.borderHoverColor.g : theme.borderColor.g,
        hovered ? theme.borderHoverColor.b : theme.borderColor.b,
        hovered ? theme.borderHoverColor.a : theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &outer);

    if (group && group->getSelectedID() == id) {
        SDL_Rect inner = { cx - radius / 2, cy - radius / 2, radius, radius };
        SDL_SetRenderDrawColor(renderer,
            theme.checkboxTickColor.r,
            theme.checkboxTickColor.g,
            theme.checkboxTickColor.b,
            theme.checkboxTickColor.a);
        SDL_RenderFillRect(renderer, &inner);
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
        cx + radius + 10,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

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

    int cx = bounds.x + 12;
    int cy = bounds.y + bounds.h / 2;
    int outerRadius = 10;
    int thickness = 2;

    SDL_Color ringColor = hovered ? theme.borderHoverColor : theme.borderColor;

    UIHelpers::DrawCircleRing(renderer, cx, cy, outerRadius, thickness, ringColor);

    if (group && group->getSelectedID() == id) {
        int innerRadius = outerRadius - thickness - 2;
        UIHelpers::DrawFilledCircle(renderer, cx, cy, innerRadius, ringColor);
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
        bounds.x + 30,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
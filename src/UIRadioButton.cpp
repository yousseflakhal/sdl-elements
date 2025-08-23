#include "UIRadioButton.hpp"

static inline SDL_Color PrimaryBlue() { return SDL_Color{ 0x0D, 0x6E, 0xFD, 255 }; }
static inline SDL_Color Darken(SDL_Color c, int d){ return UIHelpers::Darken(c, d); }

UIRadioButton::UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font)
    : label(label), id(id), group(group), font(font)
{
    bounds = { x, y, w, h };
}

void UIRadioButton::setFont(TTF_Font* f) { font = f; }
int  UIRadioButton::getID() const { return id; }
bool UIRadioButton::isHovered() const { return hovered; }

void UIRadioButton::handleEvent(const SDL_Event& e) {
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        if (SDL_PointInRect(&p, &bounds)) {
            pressed = true;
            if (focusable) focused = true;
        } else if (focusable) {
            focused = false;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        if (pressed && SDL_PointInRect(&p, &bounds)) {
            if (group) group->select(id);
        }
        pressed = false;
    }

    if (focusable && focused && e.type == SDL_KEYDOWN && enabled) {
        if (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) {
            if (group) group->select(id);
        } else if (e.key.keysym.sym == SDLK_ESCAPE) {
            focused = false;
        }
    }
}

void UIRadioButton::update(float) {
    if (!enabled) { hovered = false; pressed = false; return; }
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point p{ mx, my };
    hovered = SDL_PointInRect(&p, &bounds);
}

void UIRadioButton::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    const bool selected = (group && group->getSelectedID() == id);

    const int cx = bounds.x + 12;
    const int cy = bounds.y + bounds.h / 2;
    const int outerRadius     = 9;
    const int borderThickness = 2;
    const int donutThickness  = 5;

    if (focusable && focused) {
        SDL_Color halo = {0x0D, 0x6E, 0xFD, 100};
        UIHelpers::DrawCircleRing(renderer, cx, cy, outerRadius + 3, 3, halo);
    }

    if (selected) {
        SDL_Color blue = {0x0D, 0x6E, 0xFD, 255};
        if (pressed) blue = UIHelpers::AdjustBrightness(blue, -18);
        UIHelpers::DrawCircleRing(renderer, cx, cy, outerRadius, donutThickness, blue);
    } else {
        SDL_Color gray = {160,160,160,255};
        if (pressed) gray = UIHelpers::AdjustBrightness(gray, -18);
        UIHelpers::DrawCircleRing(renderer, cx, cy, outerRadius, borderThickness, gray);
    }

    SDL_Color textCol = {0, 0, 0,255};
    SDL_Surface* s = TTF_RenderText_Blended(activeFont, label.c_str(), textCol);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect textRect = { bounds.x + 30, bounds.y + (bounds.h - (s->h))/2, s->w, s->h };
    SDL_RenderCopy(renderer, t, nullptr, &textRect);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
}

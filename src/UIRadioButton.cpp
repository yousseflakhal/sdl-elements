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
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) {
            focused = true;
            return;
        }
        if (e.user.code == 0xF002) {
            focused = false;
            return;
        }
    }

    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        if (SDL_PointInRect(&p, &bounds)) {
            pressed = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        if (pressed && SDL_PointInRect(&p, &bounds)) {
            if (group) group->select(id);
        }
        pressed = false;
    }

    if (focused && e.type == SDL_KEYDOWN && enabled) {
        if (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) {
            if (group) group->select(id);
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
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeRadioStyle(th, ds);

    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const bool selected = (group && group->getSelectedID() == id);
    const int cx = bounds.x + st.spacingPx;
    const int cy = bounds.y + bounds.h / 2;

    SDL_Color ringCol   = hovered ? st.borderHover : st.border;
    SDL_Color selectCol = st.selected;
    SDL_Color textCol   = st.text;

    Uint8 globalAlpha = enabled ? 255 : 160;
    ringCol.a   = globalAlpha;
    selectCol.a = globalAlpha;
    textCol.a   = globalAlpha;

    if (focusable && focused) {
        SDL_Color halo = st.borderFocus; 
        halo.a = std::min<int>(halo.a, globalAlpha);
        UIHelpers::DrawCircleRing(renderer, cx, cy, st.outerRadius + 3, 3, halo);
    }

    if (selected) {
        SDL_Color c = pressed ? UIHelpers::AdjustBrightness(selectCol, -18) : selectCol;
        UIHelpers::DrawCircleRing(renderer, cx, cy, st.outerRadius, st.ringThickness, c);
    } else {
        SDL_Color c = pressed ? UIHelpers::AdjustBrightness(ringCol, -18) : ringCol;
        UIHelpers::DrawCircleRing(renderer, cx, cy, st.outerRadius, st.borderThickness, c);
    }

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, label.c_str(), textCol)
    );
    
    if (!surface) return;
    
    auto texture = UIHelpers::MakeTexture(
        SDL_CreateTextureFromSurface(renderer, surface.get())
    );
    
    SDL_Rect textRect = { 
        bounds.x + st.spacingPx + st.outerRadius + st.gapTextPx - st.outerRadius,
        bounds.y + (bounds.h - surface->h)/2, 
        surface->w, 
        surface->h 
    };
    
    SDL_RenderCopy(renderer, texture.get(), nullptr, &textRect);
}
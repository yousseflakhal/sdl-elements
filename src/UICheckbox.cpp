#include "UICheckbox.hpp"

UICheckbox::UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f)
    : label(label), linkedValue(bind), font(f)
{
    bounds = { x, y, w, h };
}

void UICheckbox::setFont(TTF_Font* f) {
    font = f;
}

void UICheckbox::handleEvent(const SDL_Event& e) {
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            linkedValue.get() = !linkedValue.get();
            if (focusable) focused = true;
        } else if (focusable) {
            focused = false;
        }
    }

    if (!focused || !enabled) return;

    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_SPACE:
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                linkedValue.get() = !linkedValue.get();
                break;
            case SDLK_ESCAPE:
                focused = false;
                break;
            default: break;
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
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) return;

    const bool isChecked = linkedValue.get();
    const int  boxSize   = 18;
    const int  radius    = 4;

    SDL_Color textCol   = hasCustomTextColor ? customTextColor
                                             : UIHelpers::RGBA(33,37,41, enabled ? 255 : 160);
    SDL_Color borderCol = hasCustomBorderColor ? customBorderColor
                                               : (hovered ? UIHelpers::RGBA(120,120,120)
                                                          : UIHelpers::RGBA(160,160,160));
    SDL_Color fillCol   = hasCustomBoxBgColor ? customBoxBgColor
                                              : UIHelpers::RGBA(255,255,255);
    SDL_Color tickCol   = hasCustomCheckedColor ? customCheckedColor
                                                : UIHelpers::RGBA(13,110,253);

    const UITheme& theme = getTheme();
    SDL_Color parentBg   = theme.backgroundColor;

    SDL_Rect box = { bounds.x, bounds.y + (bounds.h - boxSize)/2, boxSize, boxSize };

    int stroke = std::max(0, borderPx);
    int innerRadius = std::max(0, radius - stroke);
    SDL_Rect inner = { box.x + stroke, box.y + stroke, box.w - 2*stroke, box.h - 2*stroke };

    if (stroke > 0) {
        if (fillCol.a == 0) {
            UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke, borderCol, parentBg);
        } else {
            UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, innerRadius, fillCol);
            UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke, borderCol, fillCol);
        }
    } else {
        if (fillCol.a > 0)
            UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, innerRadius, fillCol);
    }

    if (isChecked) {
        float pad   = 3.5f;
        float thick = std::clamp(box.w * 0.16f, 1.5f, 3.0f);
        SDL_Rect markBox = box; markBox.y -= 1;
        UIHelpers::DrawCheckmark(renderer, markBox, thick, tickCol, pad);
    }

    const int textLeft = box.x + box.w + 8;
    SDL_Surface* s = TTF_RenderText_Blended(activeFont, label.c_str(), textCol);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect tr = { textLeft, bounds.y + (bounds.h - s->h)/2, s->w, s->h };
    SDL_RenderCopy(renderer, t, nullptr, &tr);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(s);
}

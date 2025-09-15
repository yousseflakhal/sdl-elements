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
    const UITheme& th = getTheme();
    const auto st = MakeCheckboxStyle(th);

    TTF_Font* activeFont = font ? font
                                : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const bool isChecked = linkedValue.get();

    SDL_Color textCol   = hasCustomTextColor    ? customTextColor    : st.text;
    SDL_Color borderCol = hasCustomBorderColor  ? customBorderColor  : (hovered ? st.borderHover : st.border);
    SDL_Color fillCol   = hasCustomBoxBgColor   ? customBoxBgColor   : st.boxBg;
    SDL_Color tickCol   = hasCustomCheckedColor ? customCheckedColor : st.tick;

    Uint8 globalAlpha = enabled ? 255 : 160;
    textCol.a   = globalAlpha;
    borderCol.a = globalAlpha;
    fillCol.a   = globalAlpha;
    tickCol.a   = globalAlpha;

    SDL_Rect box = { bounds.x, bounds.y + (bounds.h - st.boxSize)/2, st.boxSize, st.boxSize };

    int stroke = std::max<int>(0, (borderPx > 0 ? borderPx : st.borderPx));
    int radius = st.radius;
    int innerRadius = std::max(0, radius - stroke);
    SDL_Rect inner = { box.x + stroke, box.y + stroke, box.w - 2*stroke, box.h - 2*stroke };

    if (focusable && focused) {
        SDL_Color ring = st.borderFocus; ring.a = std::min<int>(ring.a, globalAlpha);
        UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke + 1, ring, fillCol);
    }

    if (stroke > 0) {
        if (fillCol.a == 0) {
            UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke, borderCol, th.backgroundColor);
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

    const int textLeft = box.x + box.w + st.spacingPx;
    SDL_Surface* s = TTF_RenderUTF8_Blended(activeFont, label.c_str(), textCol);
    if (!s) return;
    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_Rect tr = { textLeft, bounds.y + (bounds.h - s->h)/2, s->w, s->h };
    SDL_RenderCopy(renderer, t, nullptr, &tr);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(s);
}
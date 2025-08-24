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
    const int  borderPx  = 1;

    SDL_Color textCol   = UIHelpers::RGBA(33, 37, 41, enabled ? 255 : 160);
    SDL_Color borderCol = hovered ? UIHelpers::RGBA(120,120,120) : UIHelpers::RGBA(160,160,160);
    SDL_Color fillCol   = UIHelpers::RGBA(255,255,255);
    SDL_Color primary   = UIHelpers::RGBA(0,123,255);

    SDL_Rect box = { bounds.x, bounds.y + (bounds.h - boxSize)/2, boxSize, boxSize };

    if (focusable && focused) {
        SDL_Color ring = UIHelpers::RGBA(13,110,253,178);
        UIHelpers::StrokeRoundedRectOutside(renderer, box, radius, 2, ring, UIHelpers::RGBA(0,0,0,0));
    }

    if (!isChecked) {
        UIHelpers::FillRoundedRect(renderer, box.x, box.y, box.w, box.h, radius, borderCol);
        SDL_Rect inner = { box.x + borderPx, box.y + borderPx, box.w - 2*borderPx, box.h - 2*borderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, radius - borderPx), fillCol);
    } else {
        UIHelpers::FillRoundedRect(renderer, box.x, box.y, box.w, box.h, radius, primary);
        float pad   = 3.5f;
        float thick = std::clamp(box.w * 0.16f, 1.5f, 3.0f);
        SDL_Color white = UIHelpers::RGBA(255,255,255);
        SDL_Rect markBox = box; markBox.y -= 1;
        UIHelpers::DrawCheckmark(renderer, markBox, thick, white, pad);
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

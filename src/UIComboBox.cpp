#include "UIComboBox.hpp"


UIComboBox::UIComboBox(int x, int y, int w, int h, const std::vector<std::string>& options, int& selectedIndex)
    : options(options), selectedIndex(selectedIndex)
{
    bounds = { x, y, w, h };
}

void UIComboBox::setFont(TTF_Font* f) {
    font = f;
}

void UIComboBox::setOnSelect(std::function<void(int)> callback) {
    onSelect = callback;
}

bool UIComboBox::isHovered() const { 
    return hovered;
}

bool UIComboBox::isExpanded() const { 
    return expanded;

}

int UIComboBox::getItemCount() const { 
    return static_cast<int>(options.size());
}

int UIComboBox::getItemHeight() const {
    return bounds.h;
}

const SDL_Rect& UIComboBox::getBounds() const {
    return bounds;
}

void UIComboBox::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { focused = true; return; }
        if (e.user.code == 0xF002) { focused = false; expanded = false; return; }
    }
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };

        if (SDL_PointInRect(&p, &bounds)) {
            expanded = !expanded;
            if (expanded) {
                const int hi = (int)options.size() - 1;
                if (hi >= 0) hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
            }
            return;
        }

        if (expanded) {
            const int ih = bounds.h;
            for (int i = 0; i < (int)options.size(); ++i) {
                SDL_Rect itemRect{ bounds.x, bounds.y + (i + 1) * ih, bounds.w, ih };
                if (SDL_PointInRect(&p, &itemRect)) {
                    selectedIndex = i;
                    if (onSelect) onSelect(i);
                    expanded = false;
                    return;
                }
            }
        }
    }

    if (!focused) return;

    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                if (expanded) { expanded = false; return; }
                break;

            case SDLK_SPACE:
            case SDLK_RETURN:
                if (expanded) {
                    if (!options.empty() && hoveredIndex >= 0 && hoveredIndex < (int)options.size()) {
                        selectedIndex = hoveredIndex;
                        if (onSelect) onSelect(hoveredIndex);
                    }
                    expanded = false;
                } else {
                    expanded = true;
                    const int hi = (int)options.size() - 1;
                    if (hi >= 0) hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
                }
                return;

            case SDLK_UP:
                if (!expanded) {
                    if (!options.empty()) {
                        expanded = true;
                        const int hi = (int)options.size() - 1;
                        hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
                    }
                    return;
                }
                if (!options.empty()) {
                    hoveredIndex = std::max(0, hoveredIndex - 1);
                }
                return;

            case SDLK_DOWN:
                if (!expanded) {
                    if (!options.empty()) {
                        expanded = true;
                        const int hi = (int)options.size() - 1;
                        hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
                    }
                    return;
                }
                if (!options.empty()) {
                    hoveredIndex = std::min((int)options.size() - 1, hoveredIndex + 1);
                }
                return;
        }
    }
}

void UIComboBox::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point p{ mx, my };
    hovered = SDL_PointInRect(&p, &bounds);

    if (!expanded || options.empty()) return;

    const int ih = bounds.h;
    SDL_Rect listRect{ bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };

    if (SDL_PointInRect(&p, &listRect)) {
        int relY = my - listRect.y;
        int idx = relY / ih;
        idx = std::clamp(idx, 0, (int)options.size() - 1);
        hoveredIndex = idx;
    }
}

void UIComboBox::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(theme);
    if (!activeFont) return;

    SDL_Color textCol    = UIHelpers::RGBA(33, 37, 41);
    SDL_Color bgCol      = UIHelpers::RGBA(255,255,255);
    SDL_Color borderCol  = UIHelpers::RGBA(180,180,180);
    SDL_Color arrowCol   = UIHelpers::RGBA(108,117,125);
    SDL_Color focusRing  = UIHelpers::RGBA(13,110,253,178);
    const int r = cornerRadius;

    if (focusable && focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, r, 2, focusRing, bgCol);
    }

    if (true) {
        UIHelpers::FillRoundedRect(renderer, bounds.x, bounds.y, bounds.w, bounds.h, r, borderCol);
        SDL_Rect inner{ bounds.x + 1, bounds.y + 1, bounds.w - 2, bounds.h - 2 };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, r - 1), bgCol);

        const std::string selectedText = options.empty() ? "" : options[selectedIndex.get()];
        if (!selectedText.empty()) {
            SDL_Surface* s = TTF_RenderUTF8_Blended(activeFont, selectedText.c_str(), textCol);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect tr{ inner.x + 10, inner.y + (inner.h - s->h)/2, s->w, s->h };
                SDL_RenderCopy(renderer, t, nullptr, &tr);
                SDL_DestroyTexture(t);
                SDL_FreeSurface(s);
            }
        }

        float cx = float(inner.x + inner.w - 16);
        float cy = float(inner.y + inner.h/2);
        float half = 5.0f;
        float thick = 2.2f;
        UIHelpers::DrawRoundStrokeLine(renderer, cx - half, cy - 2.5f, cx, cy + 2.5f, thick, arrowCol);
        UIHelpers::DrawRoundStrokeLine(renderer, cx, cy + 2.5f, cx + half, cy - 2.5f, thick, arrowCol);
    }

    if (expanded && !options.empty()) {
        const int ih = bounds.h;
        SDL_Rect listRect{ bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };

        SDL_Color listBorder = UIHelpers::RGBA(0,0,0);
        SDL_Color listBg     = UIHelpers::RGBA(255,255,255);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, listBg.r, listBg.g, listBg.b, listBg.a);
        SDL_RenderFillRect(renderer, &listRect);
        SDL_SetRenderDrawColor(renderer, listBorder.r, listBorder.g, listBorder.b, listBorder.a);
        SDL_RenderDrawRect(renderer, &listRect);

        for (int i = 0; i < (int)options.size(); ++i) {
            SDL_Rect itemRect{ listRect.x, listRect.y + i*ih, listRect.w, ih };
            bool active = (i == hoveredIndex);
            SDL_Color bg = active ? UIHelpers::RGBA(13,110,253) : listBg;
            SDL_Color fg = active ? UIHelpers::RGBA(255,255,255) : textCol;

            if (active) {
                SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
                SDL_RenderFillRect(renderer, &itemRect);
            }
            SDL_Surface* s = TTF_RenderUTF8_Blended(activeFont, options[i].c_str(), fg);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect tr{ itemRect.x + 10, itemRect.y + (itemRect.h - s->h)/2, s->w, s->h };
                SDL_RenderCopy(renderer, t, nullptr, &tr);
                SDL_DestroyTexture(t);
                SDL_FreeSurface(s);
            }
        }
    }
}

UIComboBox* UIComboBox::setTextColor(SDL_Color c) {
    customTextColor = c;
    return this;
}
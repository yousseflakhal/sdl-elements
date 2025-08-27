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
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };

        // Click on field → toggle, focus
        if (SDL_PointInRect(&p, &bounds)) {
            expanded = !expanded;
            if (focusable) focused = true;
            if (expanded) hoveredIndex = 0;          // first item active by default
            return;
        }

        // Click on list items
        if (expanded) {
            const int ih = bounds.h;
            for (int i = 0; i < (int)options.size(); ++i) {
                SDL_Rect itemRect{ bounds.x, bounds.y + (i + 1) * ih, bounds.w, ih };
                if (SDL_PointInRect(&p, &itemRect)) {
                    selectedIndex.get() = i;
                    if (onSelect) onSelect(i);
                    expanded = false;
                    return;
                }
            }
        }

        // Click outside → close and blur
        expanded = false;
        if (focusable) focused = false;
    }

    if (!focused || !enabled) return;

    if (e.type == SDL_KEYDOWN) {
        if (!expanded && (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_DOWN)) {
            expanded = true;
            hoveredIndex = 0;
        } else if (expanded) {
            if (e.key.keysym.sym == SDLK_DOWN) {
                hoveredIndex = std::min((int)options.size()-1, (hoveredIndex < 0 ? 0 : hoveredIndex + 1));
            } else if (e.key.keysym.sym == SDLK_UP) {
                hoveredIndex = std::max(0, hoveredIndex - 1);
            } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
                if (hoveredIndex >= 0) {
                    selectedIndex.get() = hoveredIndex;
                    if (onSelect) onSelect(hoveredIndex);
                }
                expanded = false;
            } else if (e.key.keysym.sym == SDLK_ESCAPE) {
                expanded = false;
            }
        } else if (e.key.keysym.sym == SDLK_ESCAPE) {
            focused = false;
        }
    }
}


void UIComboBox::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point p{ mx, my };
    hovered = SDL_PointInRect(&p, &bounds);

    if (!expanded) { return; }

    // Track hover in list; if mouse not over any item, keep current or default to first.
    hoveredIndex = -1;
    const int ih = bounds.h;
    for (int i = 0; i < (int)options.size(); ++i) {
        SDL_Rect itemRect{ bounds.x, bounds.y + (i + 1) * ih, bounds.w, ih };
        if (SDL_PointInRect(&p, &itemRect)) {
            hoveredIndex = i;
            break;
        }
    }
    if (hoveredIndex == -1) hoveredIndex = 0; // first stays active by default
}



void UIComboBox::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(theme);
    if (!activeFont) return;

    // Field colors (defaults)
    SDL_Color textCol    = UIHelpers::RGBA(33, 37, 41);   // #212529
    SDL_Color bgCol      = UIHelpers::RGBA(255,255,255);  // white
    SDL_Color borderCol  = UIHelpers::RGBA(180,180,180);  // default border
    SDL_Color arrowCol   = UIHelpers::RGBA(108,117,125);  // #6c757d
    SDL_Color focusRing  = UIHelpers::RGBA(13,110,253,178); // #0D6EFD @ ~70%
    const int r = cornerRadius;

    // Focus ring ONLY on the field
    if (focusable && focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, r, 2, focusRing, bgCol);
    }

    // Field: rounded, white, gray border
    if (true) { // draw border then inset fill to get a crisp 1px border
        UIHelpers::FillRoundedRect(renderer, bounds.x, bounds.y, bounds.w, bounds.h, r, borderCol);
        SDL_Rect inner{ bounds.x + 1, bounds.y + 1, bounds.w - 2, bounds.h - 2 };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, r - 1), bgCol);

        // Text
        const std::string selectedText = options.empty() ? "" : options[selectedIndex.get()];
        if (!selectedText.empty()) {
            SDL_Surface* s = TTF_RenderText_Blended(activeFont, selectedText.c_str(), textCol);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect tr{ inner.x + 10, inner.y + (inner.h - s->h)/2, s->w, s->h };
                SDL_RenderCopy(renderer, t, nullptr, &tr);
                SDL_DestroyTexture(t);
                SDL_FreeSurface(s);
            }
        }

        // Chevron (smooth V) at right using rounded strokes
        float cx = float(inner.x + inner.w - 16);
        float cy = float(inner.y + inner.h/2);
        float half = 5.0f;
        float thick = 2.2f;
        UIHelpers::DrawRoundStrokeLine(renderer, cx - half, cy - 2.5f, cx, cy + 2.5f, thick, arrowCol);
        UIHelpers::DrawRoundStrokeLine(renderer, cx, cy + 2.5f, cx + half, cy - 2.5f, thick, arrowCol);
    }

    // Dropdown list
    if (expanded && !options.empty()) {
        const int ih = bounds.h;
        SDL_Rect listRect{ bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };

        // Outer black border, no rounded corners, white fill, no internal borders
        SDL_Color listBorder = UIHelpers::RGBA(0,0,0);
        SDL_Color listBg     = UIHelpers::RGBA(255,255,255);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, listBg.r, listBg.g, listBg.b, listBg.a);
        SDL_RenderFillRect(renderer, &listRect);
        SDL_SetRenderDrawColor(renderer, listBorder.r, listBorder.g, listBorder.b, listBorder.a);
        SDL_RenderDrawRect(renderer, &listRect);

        // Items
        for (int i = 0; i < (int)options.size(); ++i) {
            SDL_Rect itemRect{ listRect.x, listRect.y + i*ih, listRect.w, ih };
            bool active = (i == hoveredIndex);
            SDL_Color bg = active ? UIHelpers::RGBA(13,110,253) : listBg; // #0D6EFD
            SDL_Color fg = active ? UIHelpers::RGBA(255,255,255) : textCol;

            if (active) {
                SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
                SDL_RenderFillRect(renderer, &itemRect);
            }
            SDL_Surface* s = TTF_RenderText_Blended(activeFont, options[i].c_str(), fg);
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
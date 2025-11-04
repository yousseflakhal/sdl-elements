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

void UIComboBox::updateDropdownRect(SDL_Renderer* renderer) const {
    const int ih = bounds.h;
    SDL_Rect menu = { bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };
    
    if (renderer) {
        int windowH = 0;
        SDL_GetRendererOutputSize(renderer, nullptr, &windowH);
        
        bool fitsBelow = (menu.y + menu.h) <= windowH;
        if (!fitsBelow) {
            menu.y = bounds.y - menu.h;
        }
    }
    
    cachedDropdownRect = menu;
    dropdownPositionValid = true;
}

SDL_Rect UIComboBox::getDropdownRect() const {
    return cachedDropdownRect;
}

void UIComboBox::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { focused = true; return; }
        if (e.user.code == 0xF002) { 
            focused = false; 
            expanded = false; 
            dropdownPositionValid = false;
            return; 
        }
    }
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };

        if (SDL_PointInRect(&p, &bounds)) {
            expanded = !expanded;
            if (expanded) {
                const int hi = (int)options.size() - 1;
                if (hi >= 0) hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
            } else {
                dropdownPositionValid = false;
            }
            return;
        }

        if (expanded && dropdownPositionValid) {
            SDL_Rect dropdownRect = getDropdownRect();
            const int ih = bounds.h;
            
            for (int i = 0; i < (int)options.size(); ++i) {
                SDL_Rect itemRect{ dropdownRect.x, dropdownRect.y + i * ih, dropdownRect.w, ih };
                if (SDL_PointInRect(&p, &itemRect)) {
                    selectedIndex.get() = i;
                    if (onSelect) onSelect(i);
                    expanded = false;
                    dropdownPositionValid = false;
                    return;
                }
            }
            expanded = false;
            dropdownPositionValid = false;
        }
    }

    if (!focused) return;

    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                if (expanded) { 
                    expanded = false; 
                    dropdownPositionValid = false;
                    return; 
                }
                break;

            case SDLK_SPACE:
            case SDLK_RETURN:
                if (expanded) {
                    if (!options.empty() && hoveredIndex >= 0 && hoveredIndex < (int)options.size()) {
                        selectedIndex.get() = hoveredIndex;
                        if (onSelect) onSelect(hoveredIndex);
                    }
                    expanded = false;
                    dropdownPositionValid = false;
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

    if (dropdownPositionValid) {
        SDL_Rect listRect = getDropdownRect();
        const int ih = bounds.h;

        if (SDL_PointInRect(&p, &listRect)) {
            int relY = my - listRect.y;
            int idx = relY / ih;
            idx = std::clamp(idx, 0, (int)options.size() - 1);
            hoveredIndex = idx;
        }
    }
}


bool UIComboBox::isInside(int x, int y) const {
    SDL_Point p{ x, y };
    if (SDL_PointInRect(&p, &bounds)) return true;
    if (expanded && dropdownPositionValid) {
        SDL_Rect listRect = getDropdownRect();
        if (SDL_PointInRect(&p, &listRect)) return true;
    }
    return false;
}

UIComboBox* UIComboBox::setTextColor(SDL_Color c) {
    customTextColor = c;
    return this;
}

void UIComboBox::renderField(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeComboBoxStyle(th, ds);
    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const int effRadius   = (cornerRadius > 0 ? cornerRadius : st.radius);
    const int effBorderPx = st.borderPx;

    SDL_Rect field = bounds;
    if (focusable && focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, field, effRadius, effBorderPx + 1, st.borderFocus, st.fieldBg);
    }

    SDL_Color borderNow = focused ? st.borderFocus : (hovered ? st.borderHover : st.border);
    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, field.x, field.y, field.w, field.h, effRadius, borderNow);
        SDL_Rect inner = { field.x + effBorderPx, field.y + effBorderPx, field.w - 2*effBorderPx, field.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), st.fieldBg);
        field = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, field.x, field.y, field.w, field.h, effRadius, st.fieldBg);
    }

    int sel = selectedIndex.get();
    std::string display = (sel >= 0 && sel < (int)options.size()) ? options[sel] : placeholder;
    SDL_Color textCol   = (sel >= 0) ? st.fieldFg : st.placeholder;
    if (customTextColor) textCol = *customTextColor;

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, display.c_str(), textCol)
    );
    
    if (surface) {
        auto texture = UIHelpers::MakeTexture(
            SDL_CreateTextureFromSurface(renderer, surface.get())
        );
        
        SDL_Rect tr = { field.x + st.padX, field.y + (field.h - surface->h)/2, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture.get(), nullptr, &tr);
    }

    const int caretW = 12;
    const int caretH = 7;
    int cx = field.x + field.w - st.padX;
    int cy = field.y + field.h/2;
    SDL_Color caretCol = st.caret;
    if (!enabled) caretCol.a = 160;
    float thick = std::max(1.5f, float(st.borderPx) + 0.5f);
    UIHelpers::DrawChevronDown(renderer, cx, cy, caretW, caretH, thick, caretCol);
}

void UIComboBox::renderDropdown(SDL_Renderer* renderer) {
    if (!expanded || options.empty()) return;
    
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeComboBoxStyle(th, ds);
    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const int effRadius   = (cornerRadius > 0 ? cornerRadius : st.radius);
    const int effBorderPx = st.borderPx;
    const int ih = bounds.h;
    
    updateDropdownRect(renderer);
    SDL_Rect menu = getDropdownRect();
    
    SDL_Color mb = st.menuBg;
    SDL_Color mborder = st.menuBorder;
    int sel = selectedIndex.get();

    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, menu.x, menu.y, menu.w, menu.h, effRadius, mborder);
        
        SDL_Rect innerMenu = { 
            menu.x + effBorderPx, 
            menu.y + effBorderPx, 
            menu.w - 2*effBorderPx, 
            menu.h - 2*effBorderPx 
        };
        UIHelpers::FillRoundedRect(renderer, innerMenu.x, innerMenu.y, innerMenu.w, innerMenu.h, 
                                   std::max(0, effRadius - effBorderPx), mb);
        
        int y = innerMenu.y;
        for (int i = 0; i < (int)options.size(); ++i) {
            SDL_Rect row{ innerMenu.x + 4, y, innerMenu.w - 8, ih };
            bool isSel = (i == sel);
            bool isHot = (i == hoveredIndex);

            if (isSel) {
                UIHelpers::FillRoundedRect(renderer, row.x, row.y + 2, row.w, row.h - 4, 6, st.itemSelectedBg);
            } else if (isHot) {
                UIHelpers::FillRoundedRect(renderer, row.x, row.y + 2, row.w, row.h - 4, 6, st.itemHoverBg);
            }

            SDL_Color ic = isSel ? st.itemSelectedFg : st.itemFg;
            
            auto itemSurface = UIHelpers::MakeSurface(
                TTF_RenderUTF8_Blended(activeFont, options[i].c_str(), ic)
            );
            
            if (itemSurface) {
                auto itemTexture = UIHelpers::MakeTexture(
                    SDL_CreateTextureFromSurface(renderer, itemSurface.get())
                );
                
                SDL_Rect ir = { row.x + 8, row.y + (row.h - itemSurface->h)/2, itemSurface->w, itemSurface->h };
                SDL_RenderCopy(renderer, itemTexture.get(), nullptr, &ir);
            }
            
            y += ih;
        }
    } else {
        UIHelpers::FillRoundedRect(renderer, menu.x, menu.y, menu.w, menu.h, effRadius, mb);
        
        int y = menu.y;
        for (int i = 0; i < (int)options.size(); ++i) {
            SDL_Rect row{ menu.x + 4, y, menu.w - 8, ih };
            bool isSel = (i == sel);
            bool isHot = (i == hoveredIndex);

            if (isSel) {
                UIHelpers::FillRoundedRect(renderer, row.x, row.y + 2, row.w, row.h - 4, 6, st.itemSelectedBg);
            } else if (isHot) {
                UIHelpers::FillRoundedRect(renderer, row.x, row.y + 2, row.w, row.h - 4, 6, st.itemHoverBg);
            }

            SDL_Color ic = isSel ? st.itemSelectedFg : st.itemFg;
            
            auto itemSurface = UIHelpers::MakeSurface(
                TTF_RenderUTF8_Blended(activeFont, options[i].c_str(), ic)
            );
            
            if (itemSurface) {
                auto itemTexture = UIHelpers::MakeTexture(
                    SDL_CreateTextureFromSurface(renderer, itemSurface.get())
                );
                
                SDL_Rect ir = { row.x + 8, row.y + (row.h - itemSurface->h)/2, itemSurface->w, itemSurface->h };
                SDL_RenderCopy(renderer, itemTexture.get(), nullptr, &ir);
            }
            
            y += ih;
        }
    }
}

void UIComboBox::render(SDL_Renderer* renderer) {
    renderField(renderer);
    if (expanded) {
        renderDropdown(renderer);
    }
}

bool UIComboBox::isHoveringDropdown(int mx, int my) const {
    if (!expanded || !dropdownPositionValid) return false;
    
    SDL_Point p{ mx, my };
    SDL_Rect dropdownRect = getDropdownRect();
    return SDL_PointInRect(&p, &dropdownRect);
}
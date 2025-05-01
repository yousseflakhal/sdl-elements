#include "UIComboBox.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

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

void UIComboBox::handleEvent(const SDL_Event& e) {
    int mx = e.button.x;
    int my = e.button.y;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point point = { mx, my };

        if (SDL_PointInRect(&point, &bounds)) {
            expanded = !expanded;
        } else if (expanded) {
            int itemHeight = bounds.h;
            for (size_t i = 0; i < options.size(); ++i) {
                SDL_Rect itemRect = { bounds.x, bounds.y + static_cast<int>((i + 1) * itemHeight), bounds.w, itemHeight };
                if (SDL_PointInRect(&point, &itemRect)) {
                    selectedIndex.get() = static_cast<int>(i);
                    if (onSelect) onSelect(static_cast<int>(i));
                    expanded = false;
                    break;
                }
            }
        } else {
            expanded = false;
        }
    }
}

void UIComboBox::update(float) {
    if (!expanded) {
        hoveredIndex = -1;
        return;
    }

    int mx, my;
    SDL_GetMouseState(&mx, &my);

    int itemHeight = bounds.h;
    hoveredIndex = -1;

    for (size_t i = 0; i < options.size(); ++i) {
        SDL_Rect itemRect = { bounds.x, bounds.y + static_cast<int>((i + 1) * itemHeight), bounds.w, itemHeight };
        if (mx >= itemRect.x && mx <= itemRect.x + itemRect.w &&
            my >= itemRect.y && my <= itemRect.y + itemRect.h) {
            hoveredIndex = static_cast<int>(i);
            break;
        }
    }
}

void UIComboBox::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(theme);
    if (!activeFont) return;

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    std::string selectedText = options[selectedIndex.get()];
    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, selectedText.c_str(), theme.textColor);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {
            bounds.x + 5,
            bounds.y + (bounds.h - textSurface->h) / 2,
            textSurface->w,
            textSurface->h
        };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    float cx = static_cast<float>(bounds.x + bounds.w - 15);
    float cy = static_cast<float>(bounds.y + bounds.h / 2);
    SDL_Vertex vertices[3] = {
        { { cx - 5.0f, cy - 3.0f }, { theme.textColor.r, theme.textColor.g, theme.textColor.b, theme.textColor.a }, { 0, 0 } },
        { { cx, cy + 3.0f },        { theme.textColor.r, theme.textColor.g, theme.textColor.b, theme.textColor.a }, { 0, 0 } },
        { { cx + 5.0f, cy - 3.0f }, { theme.textColor.r, theme.textColor.g, theme.textColor.b, theme.textColor.a }, { 0, 0 } }
    };
    SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);

    if (expanded) {
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            SDL_Rect itemRect = { bounds.x, bounds.y + static_cast<int>((i + 1) * bounds.h), bounds.w, bounds.h };
            SDL_Color bgColor = (i == hoveredIndex) ? theme.hoverColor : theme.backgroundColor;
            SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer, &itemRect);

            SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
            SDL_RenderDrawRect(renderer, &itemRect);

            SDL_Surface* itemSurface = TTF_RenderText_Blended(activeFont, options[i].c_str(), theme.textColor);
            if (itemSurface) {
                SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(renderer, itemSurface);
                SDL_Rect itemTextRect = {
                    itemRect.x + 5,
                    itemRect.y + (itemRect.h - itemSurface->h) / 2,
                    itemSurface->w,
                    itemSurface->h
                };
                SDL_RenderCopy(renderer, itemTexture, nullptr, &itemTextRect);
                SDL_FreeSurface(itemSurface);
                SDL_DestroyTexture(itemTexture);
            }
        }
    }
}

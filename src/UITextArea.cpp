#include "UITextArea.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

UITextArea::UITextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
}

void UITextArea::setFont(TTF_Font* f) {
    font = f;
}

void UITextArea::setPlaceholder(const std::string& text) {
    placeholder = text;
}

void UITextArea::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point point = { e.button.x, e.button.y };
        bool wasFocused = focused;
        focused = SDL_PointInRect(&point, &bounds);
        if (!wasFocused && focused) SDL_StartTextInput();
        else if (wasFocused && !focused) SDL_StopTextInput();
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        if (linkedText.get().length() < static_cast<size_t>(maxLength)) {
            linkedText.get().append(e.text.text);
        }
    }

    if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && !linkedText.get().empty()) {
            linkedText.get().pop_back();
        } else if (e.key.keysym.sym == SDLK_RETURN) {
            linkedText.get().append("\n");
        }
    }
}

void UITextArea::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };
    hovered = SDL_PointInRect(&point, &bounds);
}

bool UITextArea::isHovered() const {
    return hovered;
}

void UITextArea::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) return;

    if (!label.empty()) {
        SDL_Surface* labelSurf = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
        if (labelSurf) {
            SDL_Texture* labelTex = SDL_CreateTextureFromSurface(renderer, labelSurf);
            SDL_Rect labelRect = { bounds.x, bounds.y - labelSurf->h - 4, labelSurf->w, labelSurf->h };
            SDL_RenderCopy(renderer, labelTex, nullptr, &labelRect);
            SDL_FreeSurface(labelSurf);
            SDL_DestroyTexture(labelTex);
        }
    }

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    const std::string& content = linkedText.get().empty() ? placeholder : linkedText.get();
    SDL_Color color = linkedText.get().empty() ? theme.placeholderColor : theme.textColor;
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(activeFont, content.c_str(), color, bounds.w - 10);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = { bounds.x + 5, bounds.y + 5, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}

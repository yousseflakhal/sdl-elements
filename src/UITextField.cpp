#include "UITextField.hpp"
#include "UIConfig.hpp"

UITextField::UITextField(const std::string& label, int x, int y, int w, int h, std::string* bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
}

void UITextField::handleEvent(const SDL_Event& e) {
    if (!linkedText) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        SDL_Point point = { mx, my };
        focused = SDL_PointInRect(&point, &bounds);
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        if (linkedText->length() < static_cast<size_t>(maxLength)) {
            linkedText->append(e.text.text);
        }
    }

    if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && !linkedText->empty()) {
            linkedText->pop_back();
        } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
            focused = false;
        }
    }
}

void UITextField::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };
    hovered = SDL_PointInRect(&point, &bounds);
}

void UITextField::render(SDL_Renderer* renderer) {
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font || !linkedText) return;

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Surface* labelSurface = TTF_RenderText_Blended(font, label.c_str(), textColor);
    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    SDL_Rect labelRect = {
        bounds.x,
        bounds.y - labelSurface->h - 4,
        labelSurface->w,
        labelSurface->h
    };
    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    SDL_SetRenderDrawColor(renderer, focused ? 80 : 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &bounds);

    SDL_Surface* textSurface = TTF_RenderText_Blended(font, linkedText->c_str(), textColor);
    if (!textSurface) return;
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

bool UITextField::isHovered() const {
    return hovered;
}

#include "UITextField.hpp"
#include "UIConfig.hpp"

UITextField::UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
}

UITextField* UITextField::setPlaceholder(const std::string& text) {
    placeholder = text;
    return this;
}

UITextField* UITextField::setFont(TTF_Font* f) {
    font = f;
    return this;
}

UITextField* UITextField::setInputType(InputType type) {
    inputType = type;
    return this;
}

bool UITextField::isHovered() const {
    return hovered;
}

void UITextField::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        SDL_Point point = { mx, my };
    
        bool wasFocused = focused;
        focused = SDL_PointInRect(&point, &bounds);
    
        if (!wasFocused && focused) {
            SDL_StartTextInput();
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
        }
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        // SDL_Log("Text input: %s", e.text.text);
        if (linkedText.get().length() < static_cast<size_t>(maxLength)) {
            std::string input = e.text.text;
        
            bool valid = true;
            switch (inputType) {
                case InputType::NUMERIC:
                    valid = std::all_of(input.begin(), input.end(), ::isdigit);
                    break;
                case InputType::EMAIL:
                    valid = std::all_of(input.begin(), input.end(), [](char c) {
                        return std::isalnum(c) || c == '@' || c == '.' || c == '-' || c == '_';
                    });
                    break;
                case InputType::TEXT:
                default:
                    valid = true;
                    break;
            }
        
            if (valid) {
                linkedText.get().append(input);
            }
        }
    }

    if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && !linkedText.get().empty()) {
            linkedText.get().pop_back();
        } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
            focused = false;
            SDL_StopTextInput();
        }
    }
}

void UITextField::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };
    hovered = SDL_PointInRect(&point, &bounds);

    if (focused) {
        Uint32 now = SDL_GetTicks();
        if (now - lastBlinkTime >= 500) {
            cursorVisible = !cursorVisible;
            lastBlinkTime = now;
        }
    } else {
        cursorVisible = false;
        lastBlinkTime = SDL_GetTicks();
    }
}


void UITextField::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    SDL_Surface* labelSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!labelSurface) return;

    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    SDL_Rect labelRect = { bounds.x, bounds.y - labelSurface->h - 4, labelSurface->w, labelSurface->h };
    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_Color borderCol = hovered ? theme.borderHoverColor : theme.borderColor;
    SDL_SetRenderDrawColor(renderer, borderCol.r, borderCol.g, borderCol.b, borderCol.a);
    SDL_RenderDrawRect(renderer, &bounds);

    std::string toRender = linkedText.get();
    SDL_Color textCol = theme.textColor;
    if (toRender.empty() && !focused && !placeholder.empty()) {
        toRender = placeholder;
        textCol = theme.placeholderColor;
    }

    if (!toRender.empty()) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, toRender.c_str(), textCol);
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

            if (focused && cursorVisible) {
                SDL_SetRenderDrawColor(renderer, theme.cursorColor.r, theme.cursorColor.g, theme.cursorColor.b, theme.cursorColor.a);
                SDL_Rect cursorRect = { textRect.x + textRect.w + 2, textRect.y, 2, textRect.h };
                SDL_RenderFillRect(renderer, &cursorRect);
            }
        }
    }
}


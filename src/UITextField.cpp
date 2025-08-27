#include "UITextField.hpp"

UITextField::UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
    lastBlinkTicks = SDL_GetTicks();
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
    if (e.type == SDL_USEREVENT) {
    if (e.user.code == 0xF001) {
        if (!focused) { focused = true; SDL_StartTextInput(); }
        return;
    }
    if (e.user.code == 0xF002) {
        if (focused) { focused = false; SDL_StopTextInput(); cursorVisible = false; }
        return;
    }
}
    if (!enabled) return;

    auto inside = [&](int x, int y) {
        return (x >= bounds.x && x < bounds.x + bounds.w &&
                y >= bounds.y && y < bounds.y + bounds.h);
    };

    auto markTyping = [&](){
        lastInputTicks = SDL_GetTicks();
        cursorVisible  = true;
        lastBlinkTicks = lastInputTicks;
    };

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        const bool wasFocused = focused;
        focused = inside(e.button.x, e.button.y);
        if (focused) {
            if (!wasFocused) SDL_StartTextInput();
            markTyping();
        } else {
            if (wasFocused) SDL_StopTextInput();
            cursorVisible = false;
        }
        return;
    }

    if (!focused) return;

    if (e.type == SDL_TEXTINPUT) {
        std::string& s = linkedText.get();
        for (const char* p = e.text.text; *p; ++p) {
            if ((int)s.size() >= maxLength) break;
            unsigned char c = static_cast<unsigned char>(*p);
            if (c >= 32) s.push_back(char(c));
        }
        markTyping();
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        std::string& s = linkedText.get();
        switch (e.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (!s.empty()) s.pop_back();
                markTyping();
                break;

            case SDLK_DELETE:
                if (!s.empty()) s.pop_back();
                markTyping();
                break;

            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (onSubmit) onSubmit(s);
                markTyping();
                break;

            case SDLK_ESCAPE:
                focused = false;
                SDL_StopTextInput();
                cursorVisible = false;
                break;

            case SDLK_LEFT:
            case SDLK_RIGHT:
            case SDLK_HOME:
            case SDLK_END:
                markTyping();
                break;
        }
    }
}


void UITextField::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    hovered = (mx >= bounds.x && mx < bounds.x + bounds.w &&
               my >= bounds.y && my < bounds.y + bounds.h);

    if (!enabled) { cursorVisible = false; return; }
    if (!focused) { cursorVisible = false; return; }

    const Uint32 now          = SDL_GetTicks();
    const Uint32 typingHoldMs = 300;
    const Uint32 blinkMs      = 530;

    if (now - lastInputTicks < typingHoldMs) {
        cursorVisible  = true;
        lastBlinkTicks = now;
    } else {
        if (now - lastBlinkTicks >= blinkMs) {
            cursorVisible  = !cursorVisible;
            lastBlinkTicks = now;
        }
    }
}


void UITextField::render(SDL_Renderer* renderer) {
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) return;

    SDL_Color baseBg      = {255,255,255,255};
    SDL_Color baseBorder  = {180,180,180,255};
    SDL_Color baseText    = {73, 80, 87,255};
    SDL_Color baseCursor  = {73, 80, 87,255};
    SDL_Color placeholderCol = {160,160,160,255};
    SDL_Color focusBlue   = {13,110,253,178};

    if (focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, cornerRadius, 2, focusBlue, baseBg);
    }

    SDL_Rect dst = bounds;

    if (borderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, cornerRadius, baseBorder);
        SDL_Rect inner = { dst.x + borderPx, dst.y + borderPx, dst.w - 2*borderPx, dst.h - 2*borderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, cornerRadius - borderPx), baseBg);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, cornerRadius, baseBg);
    }

    std::string toRender = (inputType == InputType::PASSWORD)
        ? std::string(linkedText.get().size(), '*')
        : linkedText.get();

    SDL_Color drawCol = baseText;
    if (toRender.empty() && !focused && !placeholder.empty()) {
        toRender = placeholder;
        drawCol = placeholderCol;
    }

    int cursorX = dst.x + 8;
    int cursorH = TTF_FontHeight(activeFont);
    int cursorY = dst.y + (dst.h - cursorH) / 2;

    if (!toRender.empty()) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, toRender.c_str(), drawCol);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {
                dst.x + 8,
                dst.y + (dst.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);

            cursorX = textRect.x + textRect.w;
            cursorH = textSurface->h;
            cursorY = textRect.y;

            SDL_FreeSurface(textSurface);
        }
    }

    if (focused && cursorVisible) {
        SDL_SetRenderDrawColor(renderer, baseCursor.r, baseCursor.g, baseCursor.b, baseCursor.a);
        SDL_Rect cursorRect = { cursorX, cursorY, 1, cursorH };
        SDL_RenderFillRect(renderer, &cursorRect);
    }
}



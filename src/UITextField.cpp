#include "UITextField.hpp"

static int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static int textWidth(TTF_Font* font, const std::string& s) {
    int w = 0, h = 0;
    if (font && !s.empty()) TTF_SizeUTF8(font, s.c_str(), &w, &h);
    return w;
}

static SDL_Rect innerRect(const SDL_Rect& r, int borderPx) {
    if (borderPx <= 0) return r;
    SDL_Rect out{ r.x + borderPx, r.y + borderPx,
                  r.w - 2*borderPx, r.h - 2*borderPx };
    if (out.w < 0) out.w = 0;
    if (out.h < 0) out.h = 0;
    return out;
}

static void ensureCaretVisible(TTF_Font* font, const std::string& full, bool passwordMode,
                               int caret, const SDL_Rect& inner, int paddingLeft,
                               int& scrollX) {
    std::string prefix = passwordMode ? std::string(caret, '*')
                                      : full.substr(0, clampi(caret, 0, (int)full.size()));
    int caretPx = 0, dummy = 0;
    if (font && !prefix.empty()) TTF_SizeUTF8(font, prefix.c_str(), &caretPx, &dummy);

    const int viewLeft  = scrollX;
    const int viewRight = scrollX + (inner.w - paddingLeft * 2);

    if (caretPx < viewLeft) {
        scrollX = caretPx;
    } else if (caretPx > viewRight) {
        scrollX = caretPx - (inner.w - paddingLeft * 2);
    }
    if (scrollX < 0) scrollX = 0;
}

static bool isWordChar(unsigned char c) {
    return std::isalnum(c) || c == '_';
}

static int prevWordIndex(const std::string& s, int from) {
    from = clampi(from, 0, (int)s.size());
    if (from == 0) return 0;
    int i = from - 1;
    while (i > 0 && !isWordChar((unsigned char)s[i])) --i;
    while (i > 0 && isWordChar((unsigned char)s[i-1])) --i;
    return i;
}

static int nextWordIndex(const std::string& s, int from) {
    from = clampi(from, 0, (int)s.size());
    int n = (int)s.size();
    if (from >= n) return n;
    int i = from;
    while (i < n && !isWordChar((unsigned char)s[i])) ++i;
    while (i < n && isWordChar((unsigned char)s[i])) ++i;
    return i;
}

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
        caret = (int)linkedText.get().size();
        clearSelection();
        TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
        SDL_Rect inner = innerRect(bounds, borderPx);
        ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                        caret, inner, 8, scrollX);
        return;
    }
    if (e.user.code == 0xF002) {
        if (focused) { focused = false; SDL_StopTextInput(); cursorVisible = false; }
        return;
    }
}
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        auto inside = [&](int x, int y) {
            return (x >= bounds.x && x < bounds.x + bounds.w &&
                    y >= bounds.y && y < bounds.y + bounds.h);
        };

        const bool wasFocused = focused;
        focused = inside(e.button.x, e.button.y);

        if (focused) {
            if (!wasFocused) SDL_StartTextInput();

            TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
            int innerX = bounds.x + (borderPx > 0 ? borderPx : 0);

            int clickX = (e.button.x - (innerX + 8)) + scrollX;
            if (clickX <= 0) {
                caret = 0;
            } else {
                const std::string& s = linkedText.get();
                int best = 0, bestDiff = 1e9;
                for (int i = 0; i <= (int)s.size(); ++i) {
                    std::string prefix;
                    if (inputType == InputType::PASSWORD) prefix.assign(i, '*');
                    else prefix = s.substr(0, i);
                    int w = textWidth(activeFont, prefix);
                    int diff = std::abs(w - clickX);
                    if (diff < bestDiff) { bestDiff = diff; best = i; }
                }
                int oldCaret = caret;
                caret = best;
                Uint32 now = SDL_GetTicks();
                bool isDouble = (now - lastClickTicks) <= 350 && std::abs(e.button.x - lastClickX) <= 4;
                lastClickTicks = now;
                lastClickX     = e.button.x;

                if (isDouble) {
                    const std::string& sdd = linkedText.get();
                    if (!sdd.empty()) {
                        int start = caret > 0 ? caret - 1 : 0;
                        while (start > 0 && isWordChar((unsigned char)sdd[start-1])) --start;
                        int end = caret;
                        while (end < (int)sdd.size() && isWordChar((unsigned char)sdd[end])) ++end;

                        selAnchor = start;
                        caret     = end;
                        SDL_Rect inner = innerRect(bounds, borderPx);
                        TTF_Font* activeFont2 = font ? font : UIConfig::getDefaultFont();
                        ensureCaretVisible(activeFont2, linkedText.get(), inputType == InputType::PASSWORD,
                                        caret, inner, 8, scrollX);
                        selectingDrag = false;
                        return;
                    }
                }
                TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                SDL_Rect inner = innerRect(bounds, borderPx);
                ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                caret, inner, 8, scrollX);
                const bool shiftHeld = (SDL_GetModState() & KMOD_SHIFT) != 0;

                if (shiftHeld) {
                    if (selAnchor < 0) selAnchor = oldCaret;
                } else {
                    selAnchor = caret;
                }

                selectingDrag = true;
            }

            lastInputTicks = SDL_GetTicks();
            cursorVisible  = true;
            lastBlinkTicks = lastInputTicks;
        } else {
            if (wasFocused) SDL_StopTextInput();
            cursorVisible = false;
        }
        return;
    }

    if (e.type == SDL_MOUSEMOTION && selectingDrag && focused) {
        TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
        int innerX = bounds.x + (borderPx > 0 ? borderPx : 0);
        int clickX = (e.motion.x - (innerX + 8)) + scrollX;

        const std::string& s = linkedText.get();
        int best = 0, bestDiff = 1e9;
        for (int i = 0; i <= (int)s.size(); ++i) {
            std::string prefix = (inputType == InputType::PASSWORD) ? std::string(i, '*') : s.substr(0, i);
            int w = textWidth(activeFont, prefix);
            int diff = std::abs(w - clickX);
            if (diff < bestDiff) { bestDiff = diff; best = i; }
        }
        caret = best;
        SDL_Rect inner = innerRect(bounds, borderPx);
        ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                        caret, inner, 8, scrollX);
        lastInputTicks = SDL_GetTicks();
        cursorVisible  = true;
        lastBlinkTicks = lastInputTicks;
        return;
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        if (selectingDrag && selAnchor == caret) {
            clearSelection();
        }
        selectingDrag = false;
        return;
    }

    if (!focused) return;

    if (e.type == SDL_TEXTINPUT) {
        std::string& s = linkedText.get();
        if (selAnchor >= 0 && selAnchor == caret) {
            clearSelection();
        }

        if (hasSelection()) {
            auto [a,b] = selRange();
            s.erase(a, b - a);
            caret = a;
            TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
            SDL_Rect inner = innerRect(bounds, borderPx);
            ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                            caret, inner, 8, scrollX);
            clearSelection();
        }

        int capacity = maxLength - (int)s.size();
        if (capacity <= 0) return;

        std::string incoming = e.text.text;
        if ((int)incoming.size() > capacity) incoming.resize(capacity);

        s.insert(s.begin() + clampi(caret, 0, (int)s.size()), incoming.begin(), incoming.end());
        caret = clampi(caret + (int)incoming.size(), 0, (int)s.size());
        TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
        SDL_Rect inner = innerRect(bounds, borderPx);
        ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                        caret, inner, 8, scrollX);

        lastInputTicks = SDL_GetTicks();
        cursorVisible  = true;
        lastBlinkTicks = lastInputTicks;
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        std::string& s = linkedText.get();
        const bool shiftHeld = (SDL_GetModState() & KMOD_SHIFT) != 0;
        const bool ctrlHeld  = (SDL_GetModState() & KMOD_CTRL)  != 0;

        switch (e.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    s.erase(a, b - a);
                    caret = a;
                    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                    SDL_Rect inner = innerRect(bounds, borderPx);
                    ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                    caret, inner, 8, scrollX);
                    clearSelection();
                } else if (caret > 0 && !s.empty()) {
                    s.erase(s.begin() + (caret - 1));
                    caret--;
                    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                    SDL_Rect inner = innerRect(bounds, borderPx);
                    ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                    caret, inner, 8, scrollX);
                }
                break;

            case SDLK_DELETE:
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    s.erase(a, b - a);
                    caret = a;
                    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                    SDL_Rect inner = innerRect(bounds, borderPx);
                    ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                    caret, inner, 8, scrollX);
                    clearSelection();
                } else if (caret < (int)s.size() && !s.empty()) {
                    s.erase(s.begin() + caret);
                    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                    SDL_Rect inner = innerRect(bounds, borderPx);
                    ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                    caret, inner, 8, scrollX);
                }
                break;

            case SDLK_LEFT: {
                int before = caret;
                if (ctrlHeld) {
                    caret = prevWordIndex(linkedText.get(), caret);
                } else {
                    caret = clampi(caret - 1, 0, (int)linkedText.get().size());
                }

                if (shiftHeld) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); }

                TTF_Font* activeFont2 = font ? font : UIConfig::getDefaultFont();
                SDL_Rect inner = innerRect(bounds, borderPx);
                ensureCaretVisible(activeFont2, linkedText.get(), inputType == InputType::PASSWORD,
                                caret, inner, 8, scrollX);
            } break;

            case SDLK_RIGHT: {
                int before = caret;
                if (ctrlHeld) {
                    caret = nextWordIndex(linkedText.get(), caret);
                } else {
                    caret = clampi(caret + 1, 0, (int)linkedText.get().size());
                }

                if (shiftHeld) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); }

                TTF_Font* activeFont2 = font ? font : UIConfig::getDefaultFont();
                SDL_Rect inner = innerRect(bounds, borderPx);
                ensureCaretVisible(activeFont2, linkedText.get(), inputType == InputType::PASSWORD,
                                caret, inner, 8, scrollX);
            } break;

            case SDLK_HOME: {
                int before = caret;
                caret = 0;
                TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                SDL_Rect inner = innerRect(bounds, borderPx);
                ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                caret, inner, 8, scrollX);
                if (shiftHeld) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); }
            } break;

            case SDLK_END: {
                int before = caret;
                caret = (int)s.size();
                TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                SDL_Rect inner = innerRect(bounds, borderPx);
                ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                caret, inner, 8, scrollX);
                if (shiftHeld) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); }
            } break;

            case SDLK_a:
                if (ctrlHeld) {
                    selAnchor = 0;
                    caret    = (int)s.size();
                    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
                    SDL_Rect inner = innerRect(bounds, borderPx);
                    ensureCaretVisible(activeFont, linkedText.get(), inputType == InputType::PASSWORD,
                                    caret, inner, 8, scrollX);
                    break;
                }
                [[fallthrough]];
            case SDLK_c:
                if (ctrlHeld && hasSelection()) {
                    auto [a,b] = selRange();
                    const std::string& ssrc = linkedText.get();
                    std::string clip = ssrc.substr(a, b - a);
                    SDL_SetClipboardText(clip.c_str());
                    break;
                }
                break;

            case SDLK_x:
                if (ctrlHeld && hasSelection()) {
                    auto [a,b] = selRange();
                    std::string& sdst = linkedText.get();
                    std::string clip = sdst.substr(a, b - a);
                    SDL_SetClipboardText(clip.c_str());
                    sdst.erase(a, b - a);
                    caret = a;
                    clearSelection();
                    {
                        TTF_Font* activeFont2 = font ? font : UIConfig::getDefaultFont();
                        SDL_Rect inner = innerRect(bounds, borderPx);
                        ensureCaretVisible(activeFont2, linkedText.get(), inputType == InputType::PASSWORD,
                                        caret, inner, 8, scrollX);
                    }
                    break;
                }
                break;

            case SDLK_v:
                if (ctrlHeld) {
                    char* txt = SDL_GetClipboardText();
                    if (txt) {
                        std::string paste = txt;
                        SDL_free(txt);

                        std::string& dst = linkedText.get();
                        if (hasSelection()) {
                            auto [a,b] = selRange();
                            dst.erase(a, b - a);
                            caret = a;
                            clearSelection();
                        }
                        int cap = maxLength - (int)dst.size();
                        if (cap > 0) {
                            if ((int)paste.size() > cap) paste.resize(cap);
                            dst.insert(dst.begin() + clampi(caret, 0, (int)dst.size()), paste.begin(), paste.end());
                            caret += (int)paste.size();

                            TTF_Font* activeFont2 = font ? font : UIConfig::getDefaultFont();
                            SDL_Rect inner = innerRect(bounds, borderPx);
                            ensureCaretVisible(activeFont2, linkedText.get(), inputType == InputType::PASSWORD,
                                            caret, inner, 8, scrollX);
                        }
                    }
                    break;
                }
                break;

            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (onSubmit) onSubmit(s);
                break;

            case SDLK_ESCAPE:
                focused = false;
                SDL_StopTextInput();
                cursorVisible = false;
                clearSelection();
                break;
        }
        lastInputTicks = SDL_GetTicks();
        cursorVisible  = true;
        lastBlinkTicks = lastInputTicks;
        return;
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

    SDL_Rect clip = { dst.x + 4, dst.y + 2, dst.w - 8, dst.h - 4 };
    SDL_RenderSetClipRect(renderer, &clip);

    if (!toRender.empty()) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, toRender.c_str(), drawCol);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {
                dst.x + 8 - scrollX,
                dst.y + (dst.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };

            if (focused && hasSelection()) {
                auto [a, b] = selRange();
                if (b > a) {
                    const std::string& full = linkedText.get();

                    std::string left = (inputType == InputType::PASSWORD)
                        ? std::string(a, '*')
                        : full.substr(0, a);

                    std::string mid = (inputType == InputType::PASSWORD)
                        ? std::string(b - a, '*')
                        : full.substr(a, b - a);

                    int leftW = textWidth(activeFont, left);
                    int midW  = textWidth(activeFont, mid);

                    int selX = textRect.x + leftW;
                    int selY = textRect.y;
                    int selW = midW;
                    int selH = textSurface->h;

                    SDL_SetRenderDrawColor(renderer, 0, 120, 215, 120);
                    SDL_Rect selRect{ selX, selY, selW, selH };
                    SDL_RenderFillRect(renderer, &selRect);
                }
            }

            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);

            cursorH = textSurface->h;
            cursorY = textRect.y;

            SDL_FreeSurface(textSurface);
        }
    }

    {
        const std::string& full = linkedText.get();
        std::string prefixMeasure = (inputType == InputType::PASSWORD)
            ? std::string(caret, '*')
            : full.substr(0, clampi(caret, 0, (int)full.size()));

        int wPrefix = textWidth(activeFont, prefixMeasure);
        cursorX = dst.x + 8 + wPrefix - scrollX;
    }

    if (focused && cursorVisible) {
        SDL_SetRenderDrawColor(renderer, baseCursor.r, baseCursor.g, baseCursor.b, baseCursor.a);
        SDL_Rect cursorRect = { cursorX, cursorY, 1, cursorH };
        SDL_RenderFillRect(renderer, &cursorRect);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}



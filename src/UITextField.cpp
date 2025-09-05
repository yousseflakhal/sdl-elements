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
            return;
        }
        if (e.user.code == 0xF002) {
            if (focused) { focused = false; SDL_StopTextInput(); preedit.clear(); clearSelection(); cursorVisible = false; }
            return;
        }
    }
    auto activeFont = font ? font : UIConfig::getDefaultFont();
    auto& textRef = linkedText.get();

    auto isInside = [&](int x, int y) {
        return x >= bounds.x && x < bounds.x + bounds.w && y >= bounds.y && y < bounds.y + bounds.h;
    };
    auto innerR = [&]() {
        if (borderPx <= 0) return bounds;
        SDL_Rect r{bounds.x + borderPx, bounds.y + borderPx, bounds.w - 2 * borderPx, bounds.h - 2 * borderPx};
        if (r.w < 0) r.w = 0;
        if (r.h < 0) r.h = 0;
        return r;
    }();

    auto isCont = [](unsigned char c) { return (c & 0xC0) == 0x80; };
    auto nextCP = [&](const std::string& s, int i) {
        int n = (int)s.size();
        if (i < 0) return 0;
        if (i >= n) return n;
        i++;
        while (i < n && isCont((unsigned char)s[i])) i++;
        return i;
    };
    auto prevCP = [&](const std::string& s, int i) {
        if (i <= 0) return 0;
        i--;
        while (i > 0 && isCont((unsigned char)s[i])) i--;
        return i;
    };
    auto codepointCountUpTo = [&](const std::string& s, int byteIndex) {
        int i = 0, count = 0, n = (int)s.size();
        while (i < n && i < byteIndex) { i = nextCP(s, i); count++; }
        return count;
    };
    auto maskedPrefixForWidth = [&](int byteCount) {
        if (inputType == InputType::PASSWORD) {
            int cps = codepointCountUpTo(textRef, byteCount);
            return std::string(cps, '*');
        } else {
            return textRef.substr(0, byteCount);
        }
    };
    auto caretByteFromX = [&](int mx) {
        int pad = 8;
        int xLocal = mx - (innerR.x + pad) + scrollX;
        if (!activeFont || xLocal <= 0) return 0;
        int n = (int)textRef.size();
        int i = 0, lastGood = 0, w = 0, h = 0;
        while (i <= n) {
            std::string pref = maskedPrefixForWidth(i);
            TTF_SizeUTF8(activeFont, pref.c_str(), &w, &h);
            if (w > xLocal) break;
            lastGood = i;
            if (i == n) break;
            i = nextCP(textRef, i);
        }
        return lastGood;
    };
    auto ensureCaretVisibleLocal = [&]() {
        if (!activeFont) return;
        int pad = 8;
        int innerW = innerR.w - 2 * pad;
        if (innerW < 0) innerW = 0;
        std::string pref = maskedPrefixForWidth(caret);
        int w = 0, h = 0;
        if (!pref.empty()) TTF_SizeUTF8(activeFont, pref.c_str(), &w, &h);
        int left = scrollX;
        int right = scrollX + innerW;
        if (w < left) scrollX = w;
        else if (w > right) {
            scrollX = w - innerW;
            if (scrollX < 0) scrollX = 0;
        }
    };
    auto updateImeRect = [&]() {
        if (!activeFont) return;
        int pad = 8;
        int cursorH = TTF_FontHeight(activeFont);
        std::string pref = maskedPrefixForWidth(caret);
        int w = 0, h = 0;
        if (!pref.empty()) TTF_SizeUTF8(activeFont, pref.c_str(), &w, &h);
        SDL_Rect r{ innerR.x + pad + w - scrollX, innerR.y + (innerR.h - cursorH) / 2, 1, cursorH };
        SDL_SetTextInputRect(&r);
    };
    auto deleteSelection = [&]() {
        if (!hasSelection()) return false;
        auto [a, b] = selRange();
        if (b < a) std::swap(a, b);
        a = clampi(a, 0, (int)textRef.size());
        b = clampi(b, 0, (int)textRef.size());
        textRef.erase(a, b - a);
        caret = a;
        clearSelection();
        return true;
    };
    auto moveLeft = [&](bool word, bool withSel) {
        int oldCaret = caret;
        int c;
        if (!word) c = prevCP(textRef, caret);
        else {
            int i = prevCP(textRef, caret);
            while (i > 0 && std::isalnum((unsigned char)textRef[prevCP(textRef, i)])) i = prevCP(textRef, i);
            c = i;
        }
        if (withSel) { if (selAnchor < 0) selAnchor = oldCaret; }
        else { clearSelection(); }
        caret = c;
    };
    auto moveRight = [&](bool word, bool withSel) {
        int oldCaret = caret;
        int c;
        if (!word) c = nextCP(textRef, caret);
        else {
            int n = (int)textRef.size();
            int i = nextCP(textRef, caret);
            while (i < n && std::isalnum((unsigned char)textRef[i])) i = nextCP(textRef, i);
            c = i;
        }
        if (withSel) { if (selAnchor < 0) selAnchor = oldCaret; }
        else { clearSelection(); }
        caret = c;
    };
    auto insertTextAtCaret = [&](const char* utf8) {
        if (!utf8 || !*utf8) return;
        deleteSelection();
        textRef.insert(caret, utf8);
        caret += (int)strlen(utf8);
        preedit.clear();
        cursorVisible = true;
        lastBlinkTicks = SDL_GetTicks();
        ensureCaretVisibleLocal();
        updateImeRect();
        clearSelection();
    };

    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (isInside(e.button.x, e.button.y)) {
                    if (!focused) { focused = true; SDL_StartTextInput(); }
                    int oldCaret = caret;
                    caret = caretByteFromX(e.button.x);
                    const bool shiftHeld = (SDL_GetModState() & KMOD_SHIFT) != 0;
                    if (shiftHeld) { if (selAnchor < 0) selAnchor = oldCaret; }
                    else { clearSelection(); selAnchor = caret; }
                    selectingDrag = true;
                    ensureCaretVisibleLocal();
                    updateImeRect();
                    cursorVisible = true;
                    lastBlinkTicks = SDL_GetTicks();
                    return;
                } else {
                    if (focused) { focused = false; SDL_StopTextInput(); preedit.clear(); clearSelection(); }
                }
            }
        } break;

        case SDL_MOUSEMOTION: {
            if (focused && selectingDrag) {
                caret = caretByteFromX(e.motion.x);
                ensureCaretVisibleLocal();
                updateImeRect();
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
                return;
            }
        } break;

        case SDL_MOUSEBUTTONUP: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                selectingDrag = false;
                if (!hasSelection()) clearSelection();
                return;
            }
        } break;

        case SDL_KEYDOWN: {
            if (!focused) break;
            SDL_Keycode key = e.key.keysym.sym;
            bool ctrl  = (e.key.keysym.mod & KMOD_CTRL)  != 0;
            bool shift = (e.key.keysym.mod & KMOD_SHIFT) != 0;

            if (ctrl && (key == SDLK_a)) {
                selAnchor = 0;
                caret = (int)textRef.size();
                ensureCaretVisibleLocal();
                updateImeRect();
                return;
            }
            if (ctrl && (key == SDLK_c)) {
                if (hasSelection()) {
                    auto [a, b] = selRange();
                    if (b < a) std::swap(a, b);
                    SDL_SetClipboardText(textRef.substr(a, b - a).c_str());
                }
                return;
            }
            if (ctrl && (key == SDLK_x)) {
                if (hasSelection()) {
                    auto [a, b] = selRange();
                    if (b < a) std::swap(a, b);
                    SDL_SetClipboardText(textRef.substr(a, b - a).c_str());
                    deleteSelection();
                    ensureCaretVisibleLocal();
                    updateImeRect();
                }
                return;
            }
            if (ctrl && (key == SDLK_v)) {
                if (SDL_HasClipboardText()) {
                    char* clip = SDL_GetClipboardText();
                    if (clip) { insertTextAtCaret(clip); SDL_free(clip); }
                }
                return;
            }

            if (key == SDLK_LEFT)  { moveLeft(ctrl,  shift); ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return; }
            if (key == SDLK_RIGHT) { moveRight(ctrl, shift); ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return; }

            if (key == SDLK_HOME) {
                int before = caret;
                caret = 0;
                if (shift) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); selAnchor = caret; }
                ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return;
            }
            if (key == SDLK_END) {
                int before = caret;
                caret = (int)textRef.size();
                if (shift) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); selAnchor = caret; }
                ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return;
            }
            if (key == SDLK_BACKSPACE) {
                if (!deleteSelection()) {
                    if (caret > 0) {
                        int p = prevCP(textRef, caret);
                        textRef.erase(p, caret - p);
                        caret = p;
                    }
                }
                ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return;
            }
            if (key == SDLK_DELETE) {
                if (!deleteSelection()) {
                    if (caret < (int)textRef.size()) {
                        int n = nextCP(textRef, caret);
                        textRef.erase(caret, n - caret);
                    }
                }
                ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return;
            }
            if (key == SDLK_RETURN || key == SDLK_KP_ENTER) { if (onSubmit) onSubmit(textRef); return; }
        } break;

        case SDL_TEXTINPUT: {
            if (!focused) break;
            insertTextAtCaret(e.text.text);
            lastInputTicks = SDL_GetTicks();
            cursorVisible  = true;
            lastBlinkTicks = lastInputTicks;
            return;
        } break;

        case SDL_TEXTEDITING: {
            if (!focused) break;
            preedit.assign(e.edit.text ? e.edit.text : "");
            preeditCursor = e.edit.start;
            updateImeRect();
            lastInputTicks = SDL_GetTicks();
            cursorVisible  = true;
            lastBlinkTicks = lastInputTicks;
            return;
        } break;

#ifdef SDL_TEXTEDITING_EXT
        case SDL_TEXTEDITING_EXT: {
            if (!focused) break;
            preedit.assign(e.editExt.text ? e.editExt.text : "");
            preeditCursor = e.editExt.start;
            updateImeRect();
            lastInputTicks = SDL_GetTicks();
            cursorVisible  = true;
            lastBlinkTicks = lastInputTicks;
            return;
        } break;
#endif

        case SDL_WINDOWEVENT: {
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                if (focused) { focused = false; SDL_StopTextInput(); preedit.clear(); clearSelection(); }
            }
        } break;
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
        SDL_Surface* textSurface = TTF_RenderUTF8_Blended(activeFont, toRender.c_str(), drawCol);
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

    if (focused && !preedit.empty()) {
        std::string preToDraw = (inputType == InputType::PASSWORD)
            ? std::string(preedit.size(), '*')
            : preedit;

        const std::string& full = linkedText.get();
        std::string prefixMeasure = (inputType == InputType::PASSWORD)
            ? std::string(caret, '*')
            : full.substr(0, std::min<int>(caret, (int)full.size()));

        int prefixW = textWidth(activeFont, prefixMeasure);

        SDL_Color preCol = baseText;
        SDL_Surface* preSurf = TTF_RenderUTF8_Blended(activeFont, preToDraw.c_str(), preCol);
        if (preSurf) {
            SDL_Texture* preTex = SDL_CreateTextureFromSurface(renderer, preSurf);
            SDL_Rect preRect = {
                dst.x + 8 + prefixW - scrollX,
                dst.y + (dst.h - preSurf->h) / 2,
                preSurf->w,
                preSurf->h
            };
            SDL_SetRenderDrawColor(renderer, preCol.r, preCol.g, preCol.b, preCol.a);
            SDL_Rect underline = { preRect.x, preRect.y + preRect.h - 1, preRect.w, 1 };
            SDL_RenderFillRect(renderer, &underline);

            SDL_RenderCopy(renderer, preTex, nullptr, &preRect);
            SDL_DestroyTexture(preTex);
            SDL_FreeSurface(preSurf);
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



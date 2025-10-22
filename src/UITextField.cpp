#include "UITextField.hpp"

void UITextField::clearRedo() { redoStack.clear(); }

static inline bool hasSelRangeInt(const int a, const int b) { return b > a; }

void UITextField::applyReplaceNoHistory(size_t a, size_t b, std::string_view repl,
                                        size_t newCursor, int newSelA, int newSelB) {
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    txt.replace(a, b - a, repl);

    caret = (int)std::min(newCursor, txt.size());

    if (hasSelRangeInt((int)newSelA, (int)newSelB)) {
        selAnchor = (int)newSelA;
        caret     = (int)newSelB;
    } else {
        clearSelection();
    }

    cursorVisible   = true;
    lastBlinkTicks  = SDL_GetTicks();
}

void UITextField::pushEdit(EditRec e, bool tryCoalesce) {
    clearRedo();

    if (tryCoalesce && !undoStack.empty()) {
        EditRec& p = undoStack.back();

        if (e.kind == EditRec::Typing && p.kind == EditRec::Typing &&
            p.pos + p.after.size() == e.pos && p.before.empty() && e.before.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.after        += e.after;
            p.cursorAfter   = e.cursorAfter;
            p.selAAfter     = e.selAAfter;
            p.selBAfter     = e.selBAfter;
            p.time          = e.time;
            return;
        }

        if (e.kind == EditRec::Backspace && p.kind == EditRec::Backspace &&
            e.pos + e.before.size() == p.pos && e.after.empty() && p.after.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.pos         = e.pos;
            p.before      = e.before + p.before;
            p.cursorAfter = e.cursorAfter;
            p.selAAfter   = e.selAAfter;
            p.selBAfter   = e.selBAfter;
            p.time        = e.time;
            return;
        }
    }

    undoStack.push_back(std::move(e));
}

void UITextField::replaceRange(size_t a, size_t b, std::string_view repl, EditRec::Kind kind,
                               bool tryCoalesce) {
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    EditRec e;
    e.pos          = a;
    e.before       = txt.substr(a, b - a);
    e.after        = std::string(repl);
    e.cursorBefore = (size_t)caret;

    if (hasSelection()) {
        auto [sa, sb] = selRange();
        e.selABefore = sa; e.selBBefore = sb;
    } else {
        e.selABefore = caret; e.selBBefore = caret;
    }
    e.kind = kind;
    e.time = SDL_GetTicks();

    const size_t newCursor = a + e.after.size();

    applyReplaceNoHistory(a, b, repl, newCursor, newCursor, newCursor);

    e.cursorAfter = (size_t)caret;
    if (hasSelection()) {
        auto [sa, sb] = selRange();
        e.selAAfter = sa; e.selBAfter = sb;
    } else {
        e.selAAfter = caret; e.selBAfter = caret;
    }

    if (historyEnabled) pushEdit(std::move(e), tryCoalesce);
}

void UITextField::undo()
{
    if (undoStack.empty()) return;
    EditRec e = undoStack.back(); undoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.after.size();

    redoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.before, e.cursorBefore, e.selABefore, e.selBBefore);
}

void UITextField::redo()
{
    if (redoStack.empty()) return;
    EditRec e = redoStack.back(); redoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.before.size();

    undoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.after, e.cursorAfter, e.selAAfter, e.selBAfter);
}

static int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static int textWidth(TTF_Font* font, const std::string& s) {
    int w = 0, h = 0;
    if (font && !s.empty()){ TTF_SizeUTF8(font, s.c_str(), &w, &h);}
    return w;
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
    if (font != f) {
        font = f;
        cacheFont = nullptr;
        measuredTextCache.clear();
        glyphX.clear();
    }
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
        int i = 0, lastGood = 0;
        while (i <= n) {
            std::string pref = maskedPrefixForWidth(i);
            rebuildGlyphX(activeFont);
            int wPref = prefixWidth(pref.size());
            if (wPref > xLocal) break;
            lastGood = i;
            if (i == n) break;
            i = nextCP(textRef, i);
        }
        return lastGood;
    };
    auto ensureCaretVisibleLocal = [&]() {
        if (!activeFont) return;

        const int pad = 8;
        int innerW = innerR.w - 2 * pad;
        if (innerW < 0) innerW = 0;

        rebuildGlyphX(activeFont);
        const int w = prefixWidth(std::min<size_t>(caret, glyphX.size() ? glyphX.size()-1 : 0));

        const int margin = 2;
        const int left  = scrollX + margin;
        const int right = scrollX + innerW - margin;

        if (w < left) {
            scrollX = std::max(0, w - margin);
        } else if (w > right) {
            scrollX = std::max(0, w - innerW + margin);
        }

        const int contentW = glyphX.empty() ? 0 : glyphX.back();
        const int maxScroll = std::max(0, contentW - innerW);
        if (scrollX > maxScroll) scrollX = maxScroll;
    };
    auto updateImeRect = [&]() {
        if (!activeFont) return;
        const int pad = 8;
        const int cursorH = TTF_FontHeight(activeFont);

        rebuildGlyphX(activeFont);
        int wCaret = prefixWidth(std::min<size_t>(caret, glyphX.size() ? glyphX.size()-1 : 0));

        if (!preedit.empty()) {
            auto isContB = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            int preByte = 0, cpLeft = std::max(0, preeditCursor);
            while (preByte < (int)preedit.size() && cpLeft-- > 0) {
                preByte++;
                while (preByte < (int)preedit.size() && isContB((unsigned char)preedit[preByte])) preByte++;
            }

            std::string preSub = (inputType == InputType::PASSWORD)
                ? std::string((int)std::count_if(preedit.begin(), preedit.begin() + preByte,
                    [&](unsigned char ch){ return !isContB(ch); }), '*')
                : preedit.substr(0, preByte);

            int wPre = 0, h = 0;
            if (!preSub.empty()) TTF_SizeUTF8(activeFont, preSub.c_str(), &wPre, &h);
            wCaret += wPre;
        }

        SDL_Rect r{ innerR.x + pad + wCaret - scrollX, innerR.y + (innerR.h - cursorH) / 2, 1, cursorH };
        SDL_SetTextInputRect(&r);
    };
    auto postEditAdjust = [&](){
        ensureCaretVisibleLocal();
        updateImeRect();
        cursorVisible  = true;
        lastBlinkTicks = SDL_GetTicks();
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

    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (isInside(e.button.x, e.button.y)) {
                    if (!focused) { focused = true; SDL_StartTextInput(); }
                    int oldCaret = caret;
                    caret = caretByteFromX(e.button.x);
                    Uint32 now = SDL_GetTicks();
                    bool near = std::abs(e.button.x - lastClickX) <= 4 && std::abs(e.button.y - lastClickY) <= 4;
                    if (now - lastClickTicks <= 350 && near) clickCount++;
                    else clickCount = 1;
                    lastClickTicks = now; lastClickX = e.button.x; lastClickY = e.button.y;

                    if (clickCount == 2) {
                        int L = caret, R = caret;
                        auto isCont = [](unsigned char c){ return (c & 0xC0) == 0x80; };
                        auto prevCP = [&](const std::string& s, int i){ if(i<=0) return 0; i--; while(i>0&&isCont((unsigned char)s[i])) i--; return i; };
                        auto nextCP = [&](const std::string& s, int i){ int n=(int)s.size(); if(i>=n) return n; i++; while(i<n&&isCont((unsigned char)s[i])) i++; return i; };

                        while (L > 0) {
                            int p = prevCP(textRef, L);
                            unsigned char ch = (unsigned char)textRef[p];
                            if (!std::isalnum(ch) && ch != '_') break;
                            L = p;
                        }
                        while (R < (int)textRef.size()) {
                            unsigned char ch = (unsigned char)textRef[R];
                            if (!std::isalnum(ch) && ch != '_') break;
                            R = nextCP(textRef, R);
                        }
                        selAnchor = L;
                        caret     = R;
                        selectingDrag = false;
                        ensureCaretVisibleLocal();
                        updateImeRect();
                        return;
                    }

                    if (clickCount == 3) {
                        selAnchor = 0;
                        caret     = (int)textRef.size();
                        selectingDrag = false;
                        ensureCaretVisibleLocal();
                        updateImeRect();
                        return;
                    }
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
                int pad = 8;
                int leftEdge  = innerR.x + pad;
                int rightEdge = innerR.x + innerR.w - pad;

                const int scrollStep = 12;
                if (e.motion.x < leftEdge)  scrollX = std::max(0, scrollX - scrollStep);
                if (e.motion.x > rightEdge) scrollX = scrollX + scrollStep;

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
            bool ctrl  = (e.key.keysym.mod & KMOD_CTRL) != 0;
            bool gui   = (e.key.keysym.mod & KMOD_GUI)  != 0;
            bool shift = (e.key.keysym.mod & KMOD_SHIFT)!= 0;

            if ((ctrl && e.key.keysym.sym == SDLK_z && !shift) ||
                (gui  && e.key.keysym.sym == SDLK_z && !shift)) { undo(); postEditAdjust(); return; }

            if ((ctrl && (e.key.keysym.sym == SDLK_y || (shift && e.key.keysym.sym == SDLK_z))) ||
                (gui  && (shift && e.key.keysym.sym == SDLK_z))) { redo(); postEditAdjust(); return; }

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
            if (ctrl && e.key.keysym.sym == SDLK_x) {
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    SDL_SetClipboardText(linkedText.get().substr(a, b-a).c_str());
                    replaceRange((size_t)a, (size_t)b, "", EditRec::Cut, false);
                    postEditAdjust();
                }
                return;
            }
            if (ctrl && e.key.keysym.sym == SDLK_v) {
                if (SDL_HasClipboardText()) {
                    char* txt = SDL_GetClipboardText();
                    if (txt) {
                        std::string paste = txt; SDL_free(txt);

                        const auto& cur = linkedText.get();
                        size_t a = hasSelection() ? (size_t)selRange().first  : (size_t)caret;
                        size_t b = hasSelection() ? (size_t)selRange().second : (size_t)caret;

                        size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                        size_t room   = (cur.size() - (b - a) < maxLen) ? (maxLen - (cur.size() - (b - a))) : 0;

                        if (room > 0) {
                            if (paste.size() > room) paste.resize(room);
                            replaceRange(a, b, paste, EditRec::Paste, false);
                            postEditAdjust();
                        }
                    }
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
            if (e.key.keysym.sym == SDLK_BACKSPACE) {
                auto& s = linkedText.get();
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    replaceRange((size_t)a, (size_t)b, "", EditRec::Backspace, false);
                } else if (ctrl) {
                    auto prevCP = [&](int i){ if (i<=0) return 0; i--; while(i>0 && ((unsigned char)s[i]&0xC0)==0x80) i--; return i; };
                    int L = caret;
                    while (L > 0 && !std::isalnum((unsigned char)s[L-1]) && s[L-1] != '_') L = prevCP(L);
                    while (L > 0 && (std::isalnum((unsigned char)s[L-1]) || s[L-1] == '_')) L = prevCP(L);
                    if (L < caret) replaceRange((size_t)L, (size_t)caret, "", EditRec::Backspace, true);
                } else if (caret > 0) {
                    auto prevCP = [&](int i){ if (i<=0) return 0; i--; while(i>0 && ((unsigned char)s[i]&0xC0)==0x80) i--; return i; };
                    int p = prevCP(caret);
                    replaceRange((size_t)p, (size_t)caret, "", EditRec::Backspace, true);
                }
                postEditAdjust();
                return;
            }

            if (e.key.keysym.sym == SDLK_DELETE) {
                auto& s = linkedText.get();
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    replaceRange((size_t)a, (size_t)b, "", EditRec::DeleteKey, false);
                } else if (ctrl) {
                    auto nextCP = [&](int i){ int n=(int)s.size(); if (i>=n) return n; i++; while(i<n && ((unsigned char)s[i]&0xC0)==0x80) i++; return i; };
                    int R = caret, n = (int)s.size();
                    while (R < n && !std::isalnum((unsigned char)s[R]) && s[R] != '_') R = nextCP(R);
                    while (R < n && (std::isalnum((unsigned char)s[R]) || s[R] == '_')) R = nextCP(R);
                    if (R > caret) replaceRange((size_t)caret, (size_t)R, "", EditRec::DeleteKey, true);
                } else if (caret < (int)s.size()) {
                    auto nextCP = [&](int i){ int n=(int)s.size(); if (i>=n) return n; i++; while(i<n && ((unsigned char)s[i]&0xC0)==0x80) i++; return i; };
                    int n2 = nextCP(caret);
                    replaceRange((size_t)caret, (size_t)n2, "", EditRec::DeleteKey, true);
                }
                postEditAdjust();
                return;
            }
        } break;
        case SDL_TEXTINPUT: {
            if (!focused) break;

            std::string in = e.text.text;
            bool valid = true;
            switch (inputType) {
                case InputType::NUMERIC: valid = std::all_of(in.begin(), in.end(), ::isdigit); break;
                case InputType::EMAIL:   valid = std::all_of(in.begin(), in.end(), [](char c){ return std::isalnum((unsigned char)c) || c=='@'||c=='.'||c=='-'||c=='_';}); break;
                default: break;
            }
            if (valid && !in.empty()) {
                const auto& cur = linkedText.get();
                size_t a = hasSelection() ? (size_t)selRange().first  : (size_t)caret;
                size_t b = hasSelection() ? (size_t)selRange().second : (size_t)caret;

                size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                size_t room   = (cur.size() - (b - a) < maxLen) ? (maxLen - (cur.size() - (b - a))) : 0;
                if (room > 0) {
                    if (in.size() > room) in.resize(room);
                    replaceRange(a, b, in, EditRec::Typing, true);
                    postEditAdjust();
                }
            }
            return;
        }

        case SDL_TEXTEDITING: {
            if (!focused) break;
            std::string newText = e.edit.text;
            bool changed = (newText != preedit) || (preeditCursor != e.edit.start);
            preedit.assign(newText);
            preeditCursor = e.edit.start;
            updateImeRect();
            if (changed) {
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
            }
            return;
        } break;

#ifdef SDL_TEXTEDITING_EXT
        case SDL_TEXTEDITING_EXT: {
            if (!focused) break;
            std::string newText = e.edit.text ? e.edit.text : "";
            bool changed = (newText != preedit) || (preeditCursor != e.edit.start);
            preedit.assign(newText);
            preeditCursor = e.edit.start;
            updateImeRect();
            if (changed) {
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
            }
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

    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextFieldStyle(th, ds);

    const int effRadius   = (cornerRadius > 0 ? cornerRadius : st.radius);
    const int effBorderPx = (borderPx     > 0 ? borderPx     : st.borderPx);

    const SDL_Color borderNow = focused ? st.borderFocus : (hovered ? st.borderHover : st.border);

    if (focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, effRadius, effBorderPx + 1, th.focusRing, st.bg);
    }

    SDL_Rect dst = bounds;
    rebuildGlyphX(activeFont);

    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, borderNow);
        SDL_Rect inner = { dst.x + effBorderPx, dst.y + effBorderPx,
                           dst.w - 2*effBorderPx, dst.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h,
                                   std::max(0, effRadius - effBorderPx), st.bg);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, st.bg);
    }

    std::string toRender = (inputType == InputType::PASSWORD)
        ? std::string(linkedText.get().size(), '*')
        : linkedText.get();

    SDL_Color drawCol = st.fg;
    if (toRender.empty() && !focused && !placeholder.empty()) {
        toRender = placeholder;
        drawCol  = st.placeholder;
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
                    std::string left = (inputType == InputType::PASSWORD) ? std::string(a, '*') : full.substr(0, a);
                    std::string mid  = (inputType == InputType::PASSWORD) ? std::string(b - a, '*') : full.substr(a, b - a);
                    int leftW = prefixWidth(a);
                    int midW  = prefixWidth(b) - prefixWidth(a);

                    SDL_SetRenderDrawColor(renderer, st.selectionBg.r, st.selectionBg.g, st.selectionBg.b, st.selectionBg.a);
                    SDL_Rect selRect{ textRect.x + leftW, textRect.y, midW, textSurface->h };
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
        std::string preToDraw = (inputType == InputType::PASSWORD) ? std::string(preedit.size(), '*') : preedit;

        const std::string& full = linkedText.get();
        std::string prefixMeasure = (inputType == InputType::PASSWORD)
            ? std::string(caret, '*')
            : full.substr(0, std::min<int>(caret, (int)full.size()));

        int prefixW = textWidth(activeFont, prefixMeasure);

        SDL_Color preCol = st.fg;
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

            auto isContB = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            int preByte = 0, cpLeft = std::max(0, preeditCursor);
            while (preByte < (int)preedit.size() && cpLeft-- > 0) {
                preByte++;
                while (preByte < (int)preedit.size() && isContB((unsigned char)preedit[preByte])) preByte++;
            }
            std::string preCaretSub = (inputType == InputType::PASSWORD)
                ? std::string((int)std::count_if(preedit.begin(), preedit.begin() + preByte,
                    [&](unsigned char ch){ return ((ch & 0xC0) != 0x80); }), '*')
                : preedit.substr(0, preByte);

            int preCaretW = 0, preCaretH = 0;
            if (!preCaretSub.empty()) TTF_SizeUTF8(activeFont, preCaretSub.c_str(), &preCaretW, &preCaretH);

            if (cursorVisible && !hasSelection()) {
                SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
                SDL_Rect preCaret = { preRect.x + preCaretW, preRect.y, 1, preRect.h };
                SDL_RenderFillRect(renderer, &preCaret);
            }
        }
    }

    {
        const std::string& full = linkedText.get();
        std::string prefixMeasure = (inputType == InputType::PASSWORD)
            ? std::string(caret, '*')
            : full.substr(0, clampi(caret, 0, (int)full.size()));

        int wPrefix = prefixWidth(std::min<size_t>(caret, glyphX.size() ? glyphX.size()-1 : 0));
        cursorX = dst.x + 8 + wPrefix - scrollX;
    }

    if (focused && cursorVisible && preedit.empty() && !hasSelection()) {
        SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
        SDL_Rect cursorRect = { cursorX, cursorY, 1, cursorH };
        SDL_RenderFillRect(renderer, &cursorRect);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

void UITextField::rebuildGlyphX(TTF_Font* f) {
    const std::string& s = linkedText.get();
    
    if (measuredTextCache == s && !glyphX.empty() && cacheFont == f) {
        return;
    }
    
    cacheFont = f;
    measuredTextCache = s;
    
    glyphX.assign(s.size() + 1, 0);
    int w = 0, h = 0;
    for (size_t i = 1; i <= s.size(); ++i) {
        std::string sub = s.substr(0, i);
        TTF_SizeUTF8(f, sub.c_str(), &w, &h);
        glyphX[i] = w;
    }
}


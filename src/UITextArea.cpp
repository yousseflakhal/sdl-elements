#include "UITextArea.hpp"

void UITextArea::clearRedo() { redoStack.clear(); }

static inline bool hasSelRange(const size_t a, const size_t b) { return b > a; }

void UITextArea::applyReplaceNoHistory(size_t a, size_t b, std::string_view repl,
                                       size_t newCursor, size_t newSelA, size_t newSelB)
{
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    txt.replace(a, b - a, repl);
    cursorPos = std::min(newCursor, txt.size());

    if (hasSelRange(newSelA, newSelB)) {
        selStart = newSelA; selEnd = newSelB;
        selectionActive = true;
    } else {
        clearSelection();
    }
    selectAnchor = cursorPos;

    preferredXpx = -1; preferredColumn = -1;
    updateCursorPosition(); setIMERectAtCaret();
    lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
}

void UITextArea::pushEdit(EditRec e, bool tryCoalesce)
{
    clearRedo();

    if (tryCoalesce && !undoStack.empty()) {
        EditRec& p = undoStack.back();

        if (e.kind == EditRec::Typing && p.kind == EditRec::Typing &&
            p.pos + p.after.size() == e.pos && p.before.empty() && e.before.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.after += e.after;
            p.cursorAfter = e.cursorAfter;
            p.selAAfter   = e.selAAfter;
            p.selBAfter   = e.selBAfter;
            p.time        = e.time;
            return;
        }

        if (e.kind == EditRec::Backspace && p.kind == EditRec::Backspace &&
            e.pos + e.before.size() == p.pos && e.after.empty() && p.after.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.pos     = e.pos;
            p.before  = e.before + p.before;
            p.cursorAfter = e.cursorAfter;
            p.selAAfter   = e.selAAfter;
            p.selBAfter   = e.selBAfter;
            p.time        = e.time;
            return;
        }
    }

    undoStack.push_back(std::move(e));
}

void UITextArea::replaceRange(size_t a, size_t b, std::string_view repl, EditRec::Kind kind,
                              bool tryCoalesce)
{
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    EditRec e;
    e.pos          = a;
    e.before       = txt.substr(a, b - a);
    e.after        = std::string(repl);
    e.cursorBefore = cursorPos;
    e.selABefore   = hasSelection() ? selRange().first  : cursorPos;
    e.selBBefore   = hasSelection() ? selRange().second : cursorPos;
    e.kind         = kind;
    e.time         = SDL_GetTicks();

    size_t newCursor = a + e.after.size();
    size_t newSelA = newCursor, newSelB = newCursor;

    applyReplaceNoHistory(a, b, repl, newCursor, newSelA, newSelB);

    e.cursorAfter = cursorPos;
    e.selAAfter   = hasSelection() ? selRange().first  : cursorPos;
    e.selBAfter   = hasSelection() ? selRange().second : cursorPos;

    if (historyEnabled) pushEdit(std::move(e), tryCoalesce);
}

void UITextArea::undo()
{
    if (undoStack.empty()) return;
    EditRec e = undoStack.back(); undoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.after.size();
    redoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.before, e.cursorBefore, e.selABefore, e.selBBefore);
}

void UITextArea::redo()
{
    if (redoStack.empty()) return;
    EditRec e = redoStack.back(); redoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.before.size();
    undoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.after, e.cursorAfter, e.selAAfter, e.selBAfter);
}


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
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { if (!focused) { focused = true; SDL_StartTextInput();preferredXpx = -1; preferredColumn = -1; } return; }
        if (e.user.code == 0xF002) {
            if (focused) {
                focused = false;
                SDL_StopTextInput();
                clearSelection();
            }
            if (selectingMouse) {
                SDL_CaptureMouse(SDL_FALSE);
                selectingMouse = false;
            }
            imeText.clear(); imeStart = imeLength = 0; imeActive = false;
            preferredXpx = -1; preferredColumn = -1;
            return;
        }
    }

    if (focused && e.type == SDL_TEXTEDITING) {
        imeText = e.edit.text;
        imeStart  = e.edit.start;
        imeLength = e.edit.length;
        imeActive = !imeText.empty();
        setIMERectAtCaret();
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        bool wasFocused = focused;
        focused = SDL_PointInRect(&p, &bounds);

        if (focused) {
            size_t idx = indexFromMouse(e.button.x, e.button.y);
            const bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;

            Uint32 now = SDL_GetTicks();
            bool near = std::abs(e.button.x - lastClickX) <= 4 && std::abs(e.button.y - lastClickY) <= 4;
            if (now - lastClickTicks <= 350 && near) clickCount++;
            else clickCount = 1;
            lastClickTicks = now; lastClickX = e.button.x; lastClickY = e.button.y;

            auto& s = linkedText.get();
            auto isCont = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            auto prevCP = [&](int i){ if(i<=0) return 0; i--; while(i>0 && isCont((unsigned char)s[i])) i--; return i; };
            auto nextCP = [&](int i){ int n=(int)s.size(); if(i>=n) return n; i++; while(i<n && isCont((unsigned char)s[i])) i++; return i; };
            auto isWord = [&](unsigned char ch){ return std::isalnum(ch) || ch=='_'; };

            if (clickCount == 3) {
                selectAll();
                cursorPos = s.size();
                preferredColumn = -1;
                preferredXpx    = -1;
                updateCursorPosition();
                setIMERectAtCaret();
                SDL_StartTextInput();
                lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
                selectingMouse = false;
                return;
            }

            if (clickCount == 2) {
                int L = (int)idx, R = (int)idx, n=(int)s.size();
                while (L > 0) {
                    int pB = prevCP(L);
                    unsigned char ch = (unsigned char)s[pB];
                    if (!isWord(ch)) break;
                    L = pB;
                }
                while (R < n) {
                    unsigned char ch = (unsigned char)s[R];
                    if (!isWord(ch)) break;
                    R = nextCP(R);
                }
                setSelection((size_t)L, (size_t)R);
                cursorPos = (size_t)R;
                preferredColumn = -1;
                preferredXpx    = -1;
                updateCursorPosition(); setIMERectAtCaret();
                SDL_StartTextInput();
                lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
                selectingMouse = false;
                return;
            }

            if (shift) {
                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = idx;
                preferredColumn = -1;
                preferredXpx    = -1;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                cursorPos = idx;
                preferredColumn = -1;
                preferredXpx    = -1;
                clearSelection();
                selectAnchor = cursorPos;
                selectingMouse = true;
            }
            updateCursorPosition(); setIMERectAtCaret();
            SDL_StartTextInput();
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            selectingMouse = true;
            SDL_CaptureMouse(SDL_TRUE);
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
            clearSelection();
            if (selectingMouse) {
                SDL_CaptureMouse(SDL_FALSE);
                selectingMouse = false;
            }
            preferredXpx    = -1;
            preferredColumn = -1;
        }

        SDL_Rect sb = getScrollbarRect();
        if (contentHeight > bounds.h) {
            float vr = float(bounds.h) / contentHeight;
            int th = std::max(int(bounds.h * vr), 20);
            int maxThumb = bounds.h - th;
            int ty = sb.y + int((scrollOffsetY / (contentHeight - bounds.h)) * maxThumb);
            SDL_Rect thumb{ sb.x, ty, sb.w, th };
            if (SDL_PointInRect(&p, &thumb)) {
                scrollbarDragging = true;
                scrollbarDragStartY = e.button.y;
                scrollbarThumbStartOffset = scrollOffsetY;
            }
        }
        return;
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        scrollbarDragging = false;
        if (selectingMouse) {
            SDL_CaptureMouse(SDL_FALSE);
            selectingMouse = false;
        }
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        if (selectingMouse && !(e.motion.state & SDL_BUTTON_LMASK)) {
            SDL_CaptureMouse(SDL_FALSE);
            selectingMouse = false;
            return;
        }
        if (scrollbarDragging && !(e.motion.state & SDL_BUTTON_LMASK)) {
            scrollbarDragging = false;
        }
        if (scrollbarDragging) {
            int dy = e.motion.y - scrollbarDragStartY;
            const auto st = MakeTextAreaStyle(th, ds);
            const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
            const float maxScroll = std::max(0.0f, contentHeight - float(viewH));
            scrollOffsetY = scrollbarThumbStartOffset + dy * (maxScroll) / float(viewH);
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, maxScroll);
            if (focused) setIMERectAtCaret();
        }

        if (selectingMouse && focused) {
            const auto st = MakeTextAreaStyle(th, ds);
            const int borderPx = st.borderPx;
            const int innerY0  = bounds.y + borderPx + paddingPx;
            const int innerH   = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);

            int yForHit = std::clamp(e.motion.y, innerY0, innerY0 + innerH - 1);
            size_t idx  = indexFromMouse(e.motion.x, yForHit);

            if (idx != cursorPos) {
                cursorPos = idx;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
                preferredColumn = -1;
                preferredXpx    = -1;
                updateCursorPosition();
                setIMERectAtCaret();
                lastBlinkTime = SDL_GetTicks();
                cursorVisible = true;
            }
            return;
        }
        const auto st = MakeTextAreaStyle(th, ds);
        const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
        if (contentHeight > bounds.h) {
            SDL_Point p{ e.motion.x, e.motion.y };
            SDL_Rect sb = getScrollbarRect();
            float vr = float(viewH) / contentHeight;
            int th = std::max(int(viewH * vr), 20);
            int maxThumb = viewH - th;
            int ty = sb.y + int((scrollOffsetY / std::max(0.0f, contentHeight - float(viewH))) * maxThumb);
            SDL_Rect thumb{ sb.x, ty, sb.w, th };
            scrollbarHovered = SDL_PointInRect(&p, &thumb);
        } else {
            scrollbarHovered = false;
        }
        return;
    }

    if (e.type == SDL_MOUSEWHEEL) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        if (mx >= bounds.x && mx <= bounds.x + bounds.w && my >= bounds.y && my <= bounds.y + bounds.h) {
            int lh = TTF_FontHeight(font ? font : UIConfig::getDefaultFont());
            scrollOffsetY -= e.wheel.y * lh;
            const auto st = MakeTextAreaStyle(th, ds);
            const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(viewH)));
            if (focused) setIMERectAtCaret();
        }
        return;
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        imeText.clear(); imeStart = imeLength = 0; imeActive = false;

        std::string in = e.text.text;
        bool valid = true;
        switch (inputType) {
            case InputType::NUMERIC: valid = std::all_of(in.begin(), in.end(), ::isdigit); break;
            case InputType::EMAIL:   valid = std::all_of(in.begin(), in.end(), [](char c){ return std::isalnum(c) || c=='@' || c=='.' || c=='-' || c=='_'; }); break;
            default: break;
        }
        if (valid && !in.empty()) {
            size_t a = hasSelection() ? selRange().first  : cursorPos;
            size_t b = hasSelection() ? selRange().second : cursorPos;
            size_t curLen = linkedText.get().size();
            size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
            size_t room   = (curLen - (b - a) < maxLen) ? (maxLen - (curLen - (b - a))) : 0;
            if (room > 0) {
                if (in.size() > room) in.resize(room);
                replaceRange(a, b, in, EditRec::Typing, true);
            }
        }
        return;
    }

    if (focused && e.type == SDL_KEYDOWN) {
        const bool ctrl  = (e.key.keysym.mod & KMOD_CTRL)  != 0;
        const bool gui   = (e.key.keysym.mod & KMOD_GUI)   != 0;
        const bool shift = (e.key.keysym.mod & KMOD_SHIFT) != 0;

        if ( (ctrl && (e.key.keysym.sym == SDLK_y || (shift && e.key.keysym.sym == SDLK_z))) ||
            (gui  &&  shift && e.key.keysym.sym == SDLK_z) ) {
            redo();
            return;
        }

        if ( ((ctrl && !shift) || (gui && !shift)) && e.key.keysym.sym == SDLK_z ) {
            undo();
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_a) {
            selectAll();
            preferredColumn = -1; preferredXpx = -1;
            updateCursorPosition(); setIMERectAtCaret();
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_c) {
            if (hasSelection()) { auto [a,b] = selRange(); SDL_SetClipboardText(linkedText.get().substr(a,b-a).c_str()); }
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_x) {
            if (hasSelection()) { auto [a,b] = selRange(); SDL_SetClipboardText(linkedText.get().substr(a,b-a).c_str()); replaceRange(a,b,"", EditRec::Cut, false); }
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_v) {
            char* txt = SDL_GetClipboardText();
            if (txt) {
                std::string paste = txt; SDL_free(txt);
                size_t a = hasSelection() ? selRange().first  : cursorPos;
                size_t b = hasSelection() ? selRange().second : cursorPos;
                size_t curLen = linkedText.get().size();
                size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                size_t room   = (curLen - (b - a) < maxLen) ? (maxLen - (curLen - (b - a))) : 0;
                if (room > 0) {
                    if (paste.size() > room) paste.resize(room);
                    replaceRange(a, b, paste, EditRec::Paste, false);
                }
            }
            return;
        }

        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
            size_t a = hasSelection() ? selRange().first  : cursorPos;
            size_t b = hasSelection() ? selRange().second : cursorPos;
            size_t curLen = linkedText.get().size();
            size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
            if (curLen - (b - a) + 1 <= maxLen) { replaceRange(a, b, "\n", EditRec::Typing, true); }
            return;
        }

        if (e.key.keysym.sym == SDLK_BACKSPACE) {
            if (hasSelection()) { auto [a,b] = selRange(); replaceRange(a, b, "", EditRec::Backspace, false); }
            else if (cursorPos > 0) { replaceRange(cursorPos - 1, cursorPos, "", EditRec::Backspace, true); }
            return;
        }

        if (e.key.keysym.sym == SDLK_DELETE) {
            if (hasSelection()) { auto [a,b] = selRange(); replaceRange(a, b, "", EditRec::DeleteKey, false); }
            else if (cursorPos < linkedText.get().size()) { replaceRange(cursorPos, cursorPos + 1, "", EditRec::DeleteKey, true); }
            return;
        }

        if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT) {
            size_t newPos = cursorPos;
            if (e.key.keysym.sym == SDLK_LEFT)  { if (newPos > 0) newPos--; }
            else                                { newPos = std::min(newPos + 1, linkedText.get().size()); }

            if (shift) {
                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = newPos;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                cursorPos = newPos;
                clearSelection();
                selectAnchor = cursorPos;
            }
            preferredColumn = -1; preferredXpx = -1;
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            updateCursorPosition(); setIMERectAtCaret();
            return;
        }

        if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {
            const bool goDown = (e.key.keysym.sym == SDLK_DOWN);
            const bool shiftHeld = shift;

            const std::string& full = linkedText.get();
            const size_t N = full.size();
            size_t i = std::min(cursorPos, N);

            int currentLine = 0;
            size_t currentCol = 0;
            size_t charsCounted = 0;
            bool found = false;

            for (size_t li = 0; li < lines.size() && !found; ++li) {
                const auto& line = lines[li];
                size_t lineLength = line.size();
                if (i >= charsCounted && i <= charsCounted + lineLength) {
                    currentLine = (int)li;
                    currentCol = i - charsCounted;
                    found = true; break;
                }
                charsCounted += lineLength;
                if (charsCounted < N && full[charsCounted] == '\n') {
                    if (i == charsCounted) { currentLine = (int)li; currentCol = lineLength; found = true; break; }
                    charsCounted++;
                }
            }
            if (!found) { currentLine = (int)lines.size() - 1; currentCol = lines.back().size(); }

            if (preferredColumn < 0) preferredColumn = (int)currentCol;

            int targetLine = currentLine + (goDown ? 1 : -1);
            size_t newPos = cursorPos;

            if (targetLine >= 0 && targetLine < (int)lines.size()) {
                const std::string& targetLineText = lines[(size_t)targetLine];
                size_t targetCol = (size_t)preferredColumn;
                if (targetCol > targetLineText.size()) targetCol = targetLineText.size();

                charsCounted = 0;
                for (int li = 0; li < targetLine; ++li) {
                    charsCounted += lines[(size_t)li].size();
                    if (charsCounted < N && full[charsCounted] == '\n') charsCounted++;
                }
                newPos = std::min(charsCounted + targetCol, N);
            } else {
                newPos = (targetLine < 0) ? 0 : N;
            }

            if (shiftHeld) {
                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = newPos;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                cursorPos = newPos;
                clearSelection();
                selectAnchor = cursorPos;
            }

            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            updateCursorPosition(); setIMERectAtCaret();

            i = std::min(cursorPos, N);
            charsCounted = 0; found = false;
            for (size_t li = 0; li < lines.size() && !found; ++li) {
                const auto& line = lines[li];
                size_t lineLength = line.size();
                if (i >= charsCounted && i <= charsCounted + lineLength) {
                    preferredColumn = (int)(i - charsCounted);
                    found = true; break;
                }
                charsCounted += lineLength;
                if (charsCounted < N && full[charsCounted] == '\n') {
                    if (i == charsCounted) { preferredColumn = (int)lineLength; found = true; break; }
                    charsCounted++;
                }
            }
            if (!found && !lines.empty()) preferredColumn = (int)lines.back().size();

            return;
        }

        lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
        updateCursorPosition(); setIMERectAtCaret();
        return;
    }

}

void UITextArea::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point pt{ mx, my };
    hovered = SDL_PointInRect(&pt, &bounds);
    if (linkedText.get().length() > size_t(maxLength)) {
        linkedText.get().resize(maxLength);
        if (cursorPos > static_cast<size_t>(maxLength)) cursorPos = static_cast<size_t>(maxLength);
    }
    if (focused) {
        Uint32 now = SDL_GetTicks();
        if (now - lastBlinkTime >= 500) {
            cursorVisible = !cursorVisible;
            lastBlinkTime = now;
        }
    } else {
        cursorVisible = false;
        lastBlinkTime = SDL_GetTicks();
        preferredXpx    = -1;
        preferredColumn = -1;
    }
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int innerW = std::max(0, bounds.w - 2*st.borderPx - 2*paddingPx);

    rebuildLayout(fnt, innerW);
    const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
    contentHeight = float(std::max<size_t>(1, lines.size())) * float(TTF_FontHeight(fnt));
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(viewH)));
    if (focused) setIMERectAtCaret();

    {
        Uint32 btns = SDL_GetMouseState(nullptr, nullptr);
        if (selectingMouse && !(btns & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            SDL_CaptureMouse(SDL_FALSE);
            selectingMouse = false;
        }
        if (scrollbarDragging && !(btns & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            scrollbarDragging = false;
        }
    }

    if (focused && selectingMouse) {
        const int borderPx = st.borderPx;
        const int innerX0  = bounds.x + borderPx + paddingPx;
        const int innerY0  = bounds.y + borderPx + paddingPx;
        const int innerH   = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);
        const int lh       = TTF_FontHeight(fnt);

        if (my < innerY0 || my >= innerY0 + innerH) {
            int dist = (my < innerY0) ? (innerY0 - my)
                                      : (my - (innerY0 + innerH - 1));
            float step = std::max(1.0f, dist * 0.25f);
            float maxScroll = std::max(0.0f, contentHeight - float(innerH));
            if (my < innerY0)  scrollOffsetY = std::max(0.0f, scrollOffsetY - step);
            else               scrollOffsetY = std::min(maxScroll, scrollOffsetY + step);
        }

        int yForHit = std::clamp(my, innerY0, innerY0 + innerH - 1);

        size_t idx = indexFromMouse(mx, yForHit);
        cursorPos = idx;
        setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));

        preferredColumn = -1;
        preferredXpx    = -1;
        updateCursorPosition();
        setIMERectAtCaret();
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
    }
}



bool UITextArea::isHovered() const {
    return hovered;
}

int UITextArea::getWordCount() const {
    const std::string& text = linkedText.get();
    int count = 0;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) ++count;
    return count;
}

std::vector<std::string> UITextArea::wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) const  {
    std::vector<std::string> lines;
    if (!font) return lines;
    std::string currentLine, currentWord;
    for (char c : text) {
        if (c == '\n') {
            lines.push_back(currentLine + currentWord);
            currentLine.clear();
            currentWord.clear();
            continue;
        }
        currentWord += c;
        std::string temp = currentLine + currentWord;
        int w, h;
        TTF_SizeUTF8(font, temp.c_str(), &w, &h);
        if (w > maxWidth) {
            if (currentLine.empty()) {
                std::string part;
                for (size_t i = 0; i < currentWord.size(); ++i) {
                    std::string test = currentWord.substr(0, i+1);
                    TTF_SizeUTF8(font, test.c_str(), &w, &h);
                    if (w > maxWidth) break;
                    part = test;
                }
                if (part.empty()) part = currentWord.substr(0,1);
                lines.push_back(part);
                currentWord = currentWord.substr(part.size());
            } else {
                lines.push_back(currentLine);
                currentLine = currentWord;
                currentWord.clear();
            }
        }
        if (c == ' ') {
            currentLine += currentWord;
            currentWord.clear();
        }
    }
    lines.push_back(currentLine + currentWord);
    return lines;
}

void UITextArea::render(SDL_Renderer* renderer) {
    TTF_Font* fnt = font ? font : (getTheme().font ? getTheme().font : UIConfig::getDefaultFont());
    if (!fnt) return;
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);

    SDL_Rect dst = bounds;
    const int effRadius   = st.radius;
    const int effBorderPx = st.borderPx;

    if (focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, dst, effRadius, effBorderPx + 1, st.borderFocus, st.bg);
    }
    SDL_Color borderNow = focused ? st.borderFocus : st.border;
    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, borderNow);
        SDL_Rect inner = { dst.x + effBorderPx, dst.y + effBorderPx, dst.w - 2*effBorderPx, dst.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), st.bg);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, st.bg);
    }

    SDL_Rect clip = { dst.x + 2, dst.y + 2, dst.w - 4, dst.h - 4 };
    SDL_RenderSetClipRect(renderer, &clip);

    const bool showPlaceholder = linkedText.get().empty() && !focused && !placeholder.empty();
    const int lh = TTF_FontHeight(fnt);
    const int innerX = dst.x + paddingPx;
    const int innerY = dst.y + paddingPx;
    const int innerW = std::max(0, dst.w - 2*paddingPx);
    const int viewH = std::max(0, dst.h - 2*paddingPx);

    if (showPlaceholder) {
        SDL_Surface* s = TTF_RenderUTF8_Blended(fnt, placeholder.c_str(), st.placeholder);
        if (s) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_Rect tr{ innerX, innerY, s->w, s->h };
            SDL_RenderCopy(renderer, t, nullptr, &tr);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
        contentHeight = float(lh);
        scrollOffsetY = std::clamp(scrollOffsetY, 0.0f,  std::max(0.0f, contentHeight - float(viewH)));
        SDL_RenderSetClipRect(renderer, nullptr);
        if (contentHeight > dst.h) renderScrollbar(renderer);
        return;
    }

    rebuildLayout(fnt, innerW);
    contentHeight = float(std::max(1, (int)lines.size()) * lh);
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f,  std::max(0.0f, contentHeight - float(viewH)));

    const std::string& full = linkedText.get();

    size_t selA_orig = 0, selB_orig = 0;
    bool drawSelection = hasSelection();
    if (drawSelection) { std::tie(selA_orig, selB_orig) = selectionRange(); }

    const size_t N = full.size();
    size_t selA = mapOrigToNoNL[std::min(selA_orig, N)];
    size_t selB = mapOrigToNoNL[std::min(selB_orig, N)];

    int y = innerY - (int)scrollOffsetY;
    for (size_t li = 0; li < lines.size(); ++li) {
        const auto& line = lines[li];

        if (drawSelection) {
            size_t Lg = std::max(selA, lineStart[li]);
            size_t Rg = std::min(selB, lineStart[li] + line.size());
            if (Rg > Lg) {
                size_t Lcol = Lg - lineStart[li];
                size_t Rcol = Rg - lineStart[li];
                int wLeft = prefixX[li][(int)Lcol];
                int wMid  = prefixX[li][(int)Rcol] - prefixX[li][(int)Lcol];
                SDL_SetRenderDrawColor(renderer, th.selectionBg.r, th.selectionBg.g, th.selectionBg.b, th.selectionBg.a);
                SDL_Rect selR{ innerX + wLeft, y, wMid, lh };
                SDL_RenderFillRect(renderer, &selR);
            }
            
            if (drawSelection && line.empty()) {
                const size_t boundaryNoNL = lineStart[li];
                
                bool isLineSelected = false;
                
                if (boundaryNoNL < mapNoNLToOrig.size()) {
                    size_t newlinePos = mapNoNLToOrig[boundaryNoNL];
                    if (newlinePos < full.size() && full[newlinePos] == '\n') {
                        isLineSelected = (selA_orig <= newlinePos && newlinePos < selB_orig);
                    } else {
                        isLineSelected = (selA_orig <= newlinePos && newlinePos <= selB_orig);
                    }
                }
                
                if (isLineSelected) {
                    int tickW = std::max(2, lh / 8);
                    SDL_SetRenderDrawColor(renderer, th.selectionBg.r, th.selectionBg.g, th.selectionBg.b, th.selectionBg.a);
                    SDL_Rect tickRect{ innerX, y, tickW, lh };
                    SDL_RenderFillRect(renderer, &tickRect);
                }
            }
        }

        if (!line.empty()) {
            SDL_Surface* s = TTF_RenderUTF8_Blended(fnt, line.c_str(), st.fg);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect tr{ innerX, y, s->w, s->h };
                SDL_RenderCopy(renderer, t, nullptr, &tr);
                SDL_DestroyTexture(t);
                SDL_FreeSurface(s);
            }
        }
        y += lh;
    }

    if (focused && cursorVisible && !hasSelection()) {
        const size_t N = full.size();
        const size_t i = std::min(cursorPos, N);

        int visualLine = 0;
        size_t charsCounted = 0;
        bool found = false;
        
        for (size_t li = 0; li < lines.size() && !found; ++li) {
            const auto& line = lines[li];
            size_t lineLength = line.size();
            
            if (i >= charsCounted && i <= charsCounted + lineLength) {
                visualLine = li;
                found = true;
                break;
            }
            charsCounted += lineLength;
            
            if (charsCounted < N && full[charsCounted] == '\n') {
                if (i == charsCounted) {
                    visualLine = li;
                    found = true;
                    break;
                }
                charsCounted++;
            }
        }

        if (!found) {
            visualLine = (int)lines.size() - 1;
        }

        size_t visualCol = 0;
        if (visualLine < (int)lines.size()) {
            size_t lineStartPos = 0;
            charsCounted = 0;
            
            for (int li = 0; li < visualLine; ++li) {
                charsCounted += lines[li].size();
                if (charsCounted < N && full[charsCounted] == '\n') {
                    charsCounted++;
                }
            }
            
            lineStartPos = charsCounted;
            visualCol = i - lineStartPos;
            
            if (visualCol > lines[visualLine].size()) {
                visualCol = lines[visualLine].size();
            }
        }

        const int cx = innerX + prefixX[visualLine][visualCol];
        const int lh = TTF_FontHeight(fnt);
        int cy = innerY + visualLine * lh - (int)scrollOffsetY;

        const int minY = dst.y + paddingPx;
        const int maxY = dst.y + dst.h - paddingPx - lh;
        cy = std::clamp(cy, minY, maxY);

        SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
        SDL_Rect caretR{ cx, cy, 1, lh };
        SDL_RenderFillRect(renderer, &caretR);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
    if (contentHeight > dst.h) renderScrollbar(renderer);
}





void UITextArea::updateCursorPosition() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;

    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int borderPx = st.borderPx;
    const int innerX0  = bounds.x + borderPx + paddingPx;
    const int innerY0  = bounds.y + borderPx + paddingPx;
    const int innerW   = std::max(0, bounds.w - 2*borderPx - 2*paddingPx);
    const int viewH    = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);

    rebuildLayout(fnt, innerW);

    const std::string& full = linkedText.get();
    const size_t N = full.size();
    const int lh = TTF_FontHeight(fnt);

    size_t i = std::min(cursorPos, N);
    
    int visualLine = 0;
    size_t charsCounted = 0;
    bool found = false;
    
    for (size_t li = 0; li < lines.size() && !found; ++li) {
        const auto& line = lines[li];
        size_t lineLength = line.size();
        
        if (i >= charsCounted && i <= charsCounted + lineLength) {
            visualLine = li;
            found = true;
            break;
        }
        charsCounted += lineLength;
        
        if (charsCounted < N && full[charsCounted] == '\n') {
            if (i == charsCounted) {
                visualLine = li;
                found = true;
                break;
            }
            charsCounted++;
        }
    }

    if (!found) {
        visualLine = (int)lines.size() - 1;
    }

    size_t visualCol = 0;
    if (visualLine < (int)lines.size()) {
        size_t lineStartPos = 0;
        charsCounted = 0;
        
        for (int li = 0; li < visualLine; ++li) {
            charsCounted += lines[li].size();
            if (charsCounted < N && full[charsCounted] == '\n') {
                charsCounted++;
            }
        }
        
        lineStartPos = charsCounted;
        visualCol = i - lineStartPos;
        
        if (visualCol > lines[visualLine].size()) {
            visualCol = lines[visualLine].size();
        }
    }

    cursorX = innerX0 + prefixX[visualLine][visualCol];
    cursorY = innerY0 + visualLine * lh;

    const float layoutH = float(std::max<size_t>(1, lines.size())) * float(lh);
    contentHeight = layoutH;

    float top = float(cursorY - innerY0);
    float bot = top + lh;
    if (top < scrollOffsetY) scrollOffsetY = top;
    else if (bot > scrollOffsetY + viewH)  scrollOffsetY = bot - viewH;

    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, layoutH - float(viewH)));
}




SDL_Rect UITextArea::getScrollbarRect() const {
    const int W = 10;
    return { bounds.x + bounds.w - W - 2, bounds.y + 2, W, bounds.h - 4 };
}

void UITextArea::renderScrollbar(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    const UIStyle& ds = getStyle();
    SDL_Rect sb = getScrollbarRect();
    const auto st = MakeTextAreaStyle(theme, ds);
    const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
    float vr = float(viewH)/contentHeight;
    int th = std::max(int(viewH*vr), 20);
    float maxScroll = std::max(0.0f, contentHeight - float(viewH));
    int maxThumb = viewH - th;
    int ty = sb.y + (maxScroll>0 ? int(scrollOffsetY/maxScroll*maxThumb) : 0);
    SDL_SetRenderDrawColor(renderer, theme.sliderTrackColor.r,theme.sliderTrackColor.g,theme.sliderTrackColor.b,150);
    SDL_RenderFillRect(renderer, &sb);
    SDL_Rect thumb{ sb.x, ty, sb.w, th };
    SDL_SetRenderDrawColor(renderer, theme.sliderThumbColor.r,theme.sliderThumbColor.g,theme.sliderThumbColor.b,200);
    SDL_RenderFillRect(renderer, &thumb);
}

bool UITextArea::isScrollbarHovered() const { 
    return scrollbarHovered;
}

bool UITextArea::isScrollbarDragging() const {
    return scrollbarDragging;
}

size_t UITextArea::indexFromMouse(int mx, int my) const {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return cursorPos;
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int borderPx = st.borderPx;
    const int innerX0  = bounds.x + borderPx + paddingPx;
    const int innerY0  = bounds.y + borderPx + paddingPx;
    const int innerW   = std::max(0, bounds.w - 2*borderPx - 2*paddingPx);

    rebuildLayout(fnt, innerW);

    const int lh = TTF_FontHeight(fnt);
    const std::string& full = linkedText.get();

    int yLocal = my - innerY0 + (int)scrollOffsetY;
    int visualLine = std::clamp(yLocal / std::max(1, lh), 0, (int)lines.size() - 1);


    if (lines[visualLine].empty()) {
        size_t noNLPos = lineStart[visualLine];
        size_t result = (noNLPos < mapNoNLToOrig.size()) ? mapNoNLToOrig[noNLPos] : full.size();
        return result;
    }

    int xLocal = mx - innerX0;
    xLocal = std::clamp(xLocal, 0, std::max(0, innerW - 1));

    const auto& P = prefixX[visualLine];
    const auto& line = lines[visualLine];
    
    int bestCol = 0;
    if (!P.empty()) {
        if (xLocal >= P.back()) {
            bestCol = (int)line.size();
        } else {
            auto it = std::lower_bound(P.begin(), P.end(), xLocal);
            size_t hi = size_t(it - P.begin());
            size_t lo = (hi == 0) ? 0 : (hi - 1);
            bestCol = (xLocal - P[lo] <= P[hi] - xLocal) ? (int)lo : (int)hi;
        }
    }

    size_t noNLPos = lineStart[visualLine] + bestCol;
    
    if (noNLPos < mapNoNLToOrig.size()) {
        return mapNoNLToOrig[noNLPos];
    } else {
        return full.size();
    }
}


void UITextArea::setIMERectAtCaret() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int borderPx = st.borderPx;
    const int innerX0  = bounds.x + borderPx + paddingPx;
    const int innerY0  = bounds.y + borderPx + paddingPx;
    const int innerW   = std::max(0, bounds.w - 2*borderPx - 2*paddingPx);
    const int innerH   = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);
    const int lh = TTF_FontHeight(fnt);

    int x = cursorX;
    int y = cursorY - (int)scrollOffsetY;

    x = std::clamp(x, innerX0, innerX0 + innerW - 1);
    y = std::clamp(y, innerY0, innerY0 + innerH - lh);

    SDL_Rect r{ x, y, 1, lh };
    SDL_SetTextInputRect(&r);
}

void UITextArea::setSelection(size_t a, size_t b) {
    if (a > b) std::swap(a,b);

    if (a == b) {
        selStart = selEnd = std::string::npos;
    } else {
        selStart = a;
        selEnd   = b;
    }

    selectionStart  = (a == b) ? 0 : a;
    selectionEnd    = (a == b) ? 0 : b;
    selectionActive = (b > a);
}

void UITextArea::rebuildLayout(TTF_Font* fnt, int maxWidthPx) const {
    const std::string& full = linkedText.get();
    if (cacheFont == fnt && cacheWidthPx == maxWidthPx && cacheText == full && !lines.empty())
        return;

    cacheFont = fnt; cacheWidthPx = maxWidthPx; cacheText = full;

    lines.clear();
    lineStart.clear();
    prefixX.clear();

    mapOrigToNoNL.assign(full.size() + 1, 0);
    mapNoNLToOrig.clear();

    size_t noNLIndex = 0;

    std::string para;
    size_t paraStartOrig = 0;

    auto ensure_size = [&](size_t want){
        if (mapNoNLToOrig.size() <= want) mapNoNLToOrig.resize(want + 1);
    };

    auto flushPara = [&](size_t end_i){
        auto wrapped = wrapTextToLines(para, fnt, maxWidthPx);

        size_t offsetInPara = 0;
        if (wrapped.empty()) {
            lines.push_back("");
            lineStart.push_back(noNLIndex);
            prefixX.emplace_back(1, 0);

            ensure_size(noNLIndex);
            
            if (end_i < full.size() && full[end_i] == '\n') {
                mapOrigToNoNL[end_i] = noNLIndex;
                mapNoNLToOrig[noNLIndex] = end_i;
            } else {
                mapNoNLToOrig[noNLIndex] = end_i;
            }
            
            noNLIndex += 1;
            
        } else {
            for (const std::string& wline : wrapped) {
                const size_t L = wline.size();
                const size_t noNLLineStart = noNLIndex;

                lines.push_back(wline);
                lineStart.push_back(noNLLineStart);

                auto& P = prefixX.emplace_back();
                P.assign(L + 1, 0);
                int w=0,h=0;
                for (size_t j = 1; j <= L; ++j) {
                    std::string sub = wline.substr(0, j);
                    TTF_SizeUTF8(fnt, sub.c_str(), &w, &h);
                    P[j] = w;
                }

                for (size_t j = 0; j < L; ++j) {
                    size_t origPos = paraStartOrig + offsetInPara + j;
                    mapOrigToNoNL[origPos] = noNLLineStart + j;
                }

                ensure_size(noNLLineStart + L);
                for (size_t j = 0; j < L; ++j) {
                    size_t origPos = paraStartOrig + offsetInPara + j;
                    mapNoNLToOrig[noNLLineStart + j] = origPos;
                }

                ensure_size(noNLLineStart + L);
                mapNoNLToOrig[noNLLineStart + L] = paraStartOrig + offsetInPara + L;

                noNLIndex    += L;
                offsetInPara += L;
            }
            
            if (end_i < full.size() && full[end_i] == '\n') {
                mapOrigToNoNL[end_i] = noNLIndex;
                noNLIndex += 1;
            }
        }
    };

    for (size_t i = 0; i <= full.size(); ++i) {
        const bool atEnd = (i == full.size());
        const char c = atEnd ? '\n' : full[i];
        if (c == '\n') {
            flushPara(i);
            para.clear();
            paraStartOrig = i + 1;
        } else {
            if (para.empty()) paraStartOrig = i;
            para.push_back(c);
        }
    }

    mapOrigToNoNL[full.size()] = noNLIndex;
    if (mapNoNLToOrig.size() <= noNLIndex) mapNoNLToOrig.resize(noNLIndex + 1);
    mapNoNLToOrig[noNLIndex] = full.size();
}




int UITextArea::lineOfIndex(size_t pos) const {
    if (lines.empty() || lineStart.empty()) return 0;

    int lo = 0, hi = (int)lineStart.size() - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (pos < lineStart[mid]) {
            hi = mid - 1;
        } else if (mid + 1 < (int)lineStart.size() && pos >= lineStart[mid + 1]) {
            lo = mid + 1;
        } else {
            return mid;
        }
    }
    
    return std::clamp(lo, 0, (int)lineStart.size() - 1);
}



int UITextArea::xAtIndex(size_t pos) const {
    if (lines.empty()) return 0;
    int li = lineOfIndex(pos);
    size_t st = lineStart[li];
    size_t col = (pos > st) ? std::min(pos - st, lines[li].size()) : 0;
    return prefixX[li][col];
}
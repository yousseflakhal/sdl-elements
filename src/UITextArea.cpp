#include "UITextArea.hpp"



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
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { if (!focused) { focused = true; SDL_StartTextInput(); } return; }
        if (e.user.code == 0xF002) { if (focused) { focused = false; SDL_StopTextInput(); clearSelection(); } 
            imeText.clear(); imeStart = imeLength = 0; imeActive = false; 
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
                updateCursorPosition(); setIMERectAtCaret();
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
                updateCursorPosition(); setIMERectAtCaret();
                SDL_StartTextInput();
                lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
                selectingMouse = false;
                return;
            }

            if (shift && wasFocused) {
                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = idx;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                cursorPos = idx;
                clearSelection();
                selectAnchor = cursorPos;
                selectingMouse = true;
            }
            updateCursorPosition(); setIMERectAtCaret();
            SDL_StartTextInput();
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
            clearSelection();
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
        selectingMouse = false;
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        if (scrollbarDragging) {
            int dy = e.motion.y - scrollbarDragStartY;
            scrollOffsetY = scrollbarThumbStartOffset + dy * (contentHeight - bounds.h) / bounds.h;
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, contentHeight - float(bounds.h));
            if (focused) setIMERectAtCaret();
        }

        if (selectingMouse && focused) {
            size_t idx = indexFromMouse(e.motion.x, e.motion.y);
            cursorPos = idx;
            setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            updateCursorPosition();
            setIMERectAtCaret();
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        }

        if (contentHeight > bounds.h) {
            SDL_Point p{ e.motion.x, e.motion.y };
            SDL_Rect sb = getScrollbarRect();
            float vr = float(bounds.h) / contentHeight;
            int th = std::max(int(bounds.h * vr), 20);
            int maxThumb = bounds.h - th;
            int ty = sb.y + int((scrollOffsetY / (contentHeight - bounds.h)) * maxThumb);
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
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, contentHeight - float(bounds.h));
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
            size_t curLen = linkedText.get().size();
            size_t selLen = hasSelection() ? (selRange().second - selRange().first) : 0;
            size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
            size_t room   = (curLen - selLen < maxLen) ? (maxLen - (curLen - selLen)) : 0;
            if (hasSelection()) {
                auto [a,b] = selRange();
                linkedText.get().erase(a, b - a);
                cursorPos = a;
                clearSelection();
            }
            if (room > 0) {
                if (in.size() > room) in.resize(room);
                linkedText.get().insert(cursorPos, in);
                cursorPos += in.size();
            }
        }
        updateCursorPosition();
        setIMERectAtCaret();
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
        return;
    }

    if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && cursorPos > 0) {
            if (hasSelection()) {
                auto [a,b] = selRange();
                linkedText.get().erase(a, b - a);
                cursorPos = a;
                clearSelection();
            } else if (cursorPos > 0) {
                linkedText.get().erase(cursorPos - 1, 1);
                cursorPos--;
            }
        } else if (e.key.keysym.sym == SDLK_RETURN) {
            if (linkedText.get().length() < size_t(maxLength)) {
                linkedText.get().insert(cursorPos, "\n");
                cursorPos++;
            }
        }

        const bool ctrl = (e.key.keysym.mod & KMOD_CTRL) != 0;
        const bool shift = (e.key.keysym.mod & KMOD_SHIFT) != 0;
        if (e.key.keysym.sym == SDLK_LEFT && cursorPos > 0) {
        if (shift) {
            if (!hasSelection()) selectAnchor = cursorPos;
            cursorPos--;
            setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
        } else {
            cursorPos--;
            clearSelection();
        }
    }
    else if (e.key.keysym.sym == SDLK_RIGHT && cursorPos < linkedText.get().size()) {
        if (shift) {
            if (!hasSelection()) selectAnchor = cursorPos;
            cursorPos++;
            setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
        } else {
            cursorPos++;
            clearSelection();
        }
    }
        if (ctrl && e.key.keysym.sym == SDLK_a) {
            selectAll();
            updateCursorPosition(); setIMERectAtCaret();
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_c) {
            if (hasSelection()) {
                auto [a,b] = selRange();
                std::string clip = linkedText.get().substr(a, b - a);
                SDL_SetClipboardText(clip.c_str());
            }
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_x) {
            if (hasSelection()) {
                auto [a,b] = selRange();
                std::string clip = linkedText.get().substr(a, b - a);
                SDL_SetClipboardText(clip.c_str());
                linkedText.get().erase(a, b - a);
                cursorPos = a;
                clearSelection();
                updateCursorPosition(); setIMERectAtCaret();
                lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            }
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_v) {
            char* txt = SDL_GetClipboardText();
            if (txt) {
                std::string paste = txt; SDL_free(txt);
                size_t curLen = linkedText.get().size();
                size_t selLen = hasSelection() ? (selRange().second - selRange().first) : 0;
                size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                size_t room   = (curLen - selLen < maxLen) ? (maxLen - (curLen - selLen)) : 0;
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    linkedText.get().erase(a, b - a);
                    cursorPos = a;
                    clearSelection();
                }
                if (room > 0) {
                    if (paste.size() > room) paste.resize(room);
                    linkedText.get().insert(cursorPos, paste);
                    cursorPos += paste.size();
                    updateCursorPosition(); setIMERectAtCaret();
                    lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
                }
            }
            return;
        }

        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
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
    }
    TTF_Font* fnt = font?font:UIConfig::getDefaultFont();
    auto lines = wrapTextToLines(linkedText.get().empty()?placeholder:linkedText.get(), fnt, bounds.w-10);
    contentHeight = float(lines.size()*TTF_FontHeight(fnt));
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(bounds.h)));
    if (focused) setIMERectAtCaret();
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
    auto st = MakeTextAreaStyle(th);

    int effRadius = (cornerRadius > 0 ? cornerRadius : st.radius);
    int effBorderPx = (borderPx > 0 ? borderPx : st.borderPx);

    if (focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, effRadius, effBorderPx + 1, st.borderFocus, st.bg);
    }

    SDL_Rect dst = bounds;
    if (effBorderPx > 0) {
        SDL_Color borderNow = focused ? st.borderFocus : (hovered ? st.borderHover : st.border);
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
    std::string txt = showPlaceholder ? placeholder : linkedText.get();
    SDL_Color drawCol = showPlaceholder ? st.placeholder : st.fg;

    const int lh = TTF_FontHeight(fnt);
    const int innerX = dst.x + paddingPx;
    const int innerY = dst.y + paddingPx;

    auto lines = wrapTextToLines(txt, fnt, dst.w - 2*paddingPx);
    contentHeight = float(std::max(1, (int)lines.size()) * lh);
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(dst.h)));

    size_t selA = 0, selB = 0;
    bool drawSelection = hasSelection() && !showPlaceholder;
    if (drawSelection) { std::tie(selA, selB) = selectionRange(); }

    int y = innerY - (int)scrollOffsetY;
    size_t baseIndex = 0;
    for (const auto& line : lines) {
        if (drawSelection) {
            size_t L = (selA <= baseIndex) ? 0 : (selA - baseIndex);
            size_t R = (selB <= baseIndex) ? 0 : (selB - baseIndex);
            L = std::min(L, line.size());
            R = std::min(R, line.size());
            if (R > L) {
                int wLeft=0,h=0, wMid=0;
                std::string left = line.substr(0, L);
                std::string mid  = line.substr(L, R-L);
                if (!left.empty()) TTF_SizeUTF8(fnt, left.c_str(), &wLeft, &h);
                if (!mid.empty())  TTF_SizeUTF8(fnt, mid.c_str(),  &wMid,  &h);
                SDL_SetRenderDrawColor(renderer, st.selectionBg.r, st.selectionBg.g, st.selectionBg.b, st.selectionBg.a);
                SDL_Rect selR{ innerX + wLeft, y, wMid, lh };
                SDL_RenderFillRect(renderer, &selR);
            }
        }

        if (!line.empty()) {
            SDL_Surface* s = TTF_RenderUTF8_Blended(fnt, line.c_str(), drawCol);
            if (s) {
                SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
                SDL_Rect dstText{ innerX, y + (lh - s->h)/2, s->w, s->h };
                SDL_RenderCopy(renderer, t, nullptr, &dstText);
                SDL_DestroyTexture(t);
                SDL_FreeSurface(s);
            }
        }
        y += lh;
        baseIndex += line.size();
    }

    if (focused && imeActive && !imeText.empty()) {
        int onScreenY = cursorY - (int)scrollOffsetY;
        onScreenY = std::clamp(onScreenY, dst.y, dst.y + dst.h - lh);
        SDL_Surface* s = TTF_RenderUTF8_Blended(fnt, imeText.c_str(), st.fg);
        if (s) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_Rect compDst{ cursorX, onScreenY + (lh - s->h)/2, s->w, s->h };
            SDL_RenderCopy(renderer, t, nullptr, &compDst);
            SDL_SetRenderDrawColor(renderer, st.fg.r, st.fg.g, st.fg.b, st.fg.a);
            SDL_Rect ul{ compDst.x, compDst.y + compDst.h - 1, compDst.w, 1 };
            SDL_RenderFillRect(renderer, &ul);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
    }

    if (focused && cursorVisible && !showPlaceholder && !hasSelection()) {
        int onScreenY = cursorY - (int)scrollOffsetY;
        onScreenY = std::clamp(onScreenY, dst.y, dst.y + dst.h - lh);
        SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
        SDL_Rect caretR{ cursorX, onScreenY, 1, lh };
        SDL_RenderFillRect(renderer, &caretR);
    }

    SDL_RenderSetClipRect(renderer, nullptr);

    if (contentHeight > dst.h) renderScrollbar(renderer);
}




void UITextArea::updateCursorPosition() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    int lh = TTF_FontHeight(fnt);
    std::string before = linkedText.get().substr(0, cursorPos);
    auto lines = wrapTextToLines(before, fnt, bounds.w - 2*paddingPx);
    int caretLine = int(lines.size()) - 1;
    const std::string& last = lines.empty() ? "" : lines.back();
    int w = 0;
    if (!last.empty()) {
        TTF_SizeUTF8(fnt, last.c_str(), &w, nullptr);
    }
    cursorX = bounds.x + paddingPx + w;
    cursorY = bounds.y + paddingPx + caretLine * lh;
    float top = float(cursorY - bounds.y);
    float bottom = top + lh;
    if (top < scrollOffsetY) {
        scrollOffsetY = top;
    } else if (bottom > scrollOffsetY + bounds.h) {
        scrollOffsetY = bottom - bounds.h;
    }
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(bounds.h)));
}

SDL_Rect UITextArea::getScrollbarRect() const {
    const int W = 10;
    return { bounds.x + bounds.w - W - 2, bounds.y + 2, W, bounds.h - 4 };
}

void UITextArea::renderScrollbar(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    SDL_Rect sb = getScrollbarRect();
    float vr = float(bounds.h)/contentHeight;
    int th = std::max(int(bounds.h*vr), 20);
    float maxScroll = contentHeight - bounds.h;
    int maxThumb = bounds.h - th;
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

    int lh = TTF_FontHeight(fnt);
    const int innerX = bounds.x + paddingPx;
    const int innerY = bounds.y + paddingPx;

    int yLocal = my - innerY + (int)scrollOffsetY;
    int lineIdx = std::max(0, yLocal / std::max(1, lh));

    auto lines = wrapTextToLines(linkedText.get(), fnt, bounds.w - 2*paddingPx);

    if (lines.empty()) return 0;
    if (lineIdx >= (int)lines.size()) lineIdx = (int)lines.size() - 1;

    const std::string& line = lines[lineIdx];
    int xLocal = mx - innerX; if (xLocal < 0) xLocal = 0;

    size_t best = 0;
    int w=0, h=0;
    for (size_t i = 0; i <= line.size(); ++i) {
        std::string s = line.substr(0, i);
        TTF_SizeUTF8(fnt, s.c_str(), &w, &h);
        if (w <= xLocal) { best = i; }
        else break;
    }

    size_t idx = 0;
    for (int i = 0; i < lineIdx; ++i) idx += lines[i].size();
    idx += best;
    return idx;
}

void UITextArea::setIMERectAtCaret() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;

    const int lh = TTF_FontHeight(fnt);

    int onScreenY = cursorY - (int)scrollOffsetY;
    SDL_Rect r{ cursorX, onScreenY, 1, lh };

    r.x = std::max(bounds.x, std::min(bounds.x + bounds.w - 1, r.x));
    r.y = std::max(bounds.y, std::min(bounds.y + bounds.h - lh, r.y));

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
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
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        bool wasFocused = focused;
        focused = SDL_PointInRect(&p, &bounds);
        if (!wasFocused && focused) {
            SDL_StartTextInput();
            cursorPos = linkedText.get().size();
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
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
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        scrollbarDragging = false;
    } else if (e.type == SDL_MOUSEMOTION) {
        if (scrollbarDragging) {
            int dy = e.motion.y - scrollbarDragStartY;
            scrollOffsetY = scrollbarThumbStartOffset + dy * (contentHeight - bounds.h) / bounds.h;
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, contentHeight - float(bounds.h));
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
    } else if (focused && e.type == SDL_TEXTINPUT) {
        std::string in = e.text.text;
        bool valid = true;
        switch (inputType) {
            case InputType::NUMERIC:
                valid = std::all_of(in.begin(), in.end(), ::isdigit);
                break;
            case InputType::EMAIL:
                valid = std::all_of(in.begin(), in.end(), [](char c){ return std::isalnum(c) || c=='@' || c=='.' || c=='-' || c=='_'; });
                break;
            default:
                break;
        }
        size_t room = maxLength > 0 ? maxLength - linkedText.get().size() : in.size();
        if (room > 0 && valid) {
            if (in.size() > room) in.resize(room);
            linkedText.get().insert(cursorPos, in);
            cursorPos += in.size();
        }
        updateCursorPosition();
    } else if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && cursorPos > 0) {
            linkedText.get().erase(cursorPos - 1, 1);
            cursorPos--;
        } else if (e.key.keysym.sym == SDLK_RETURN) {
            if (linkedText.get().length() < size_t(maxLength)) {
                linkedText.get().insert(cursorPos, "\n");
                cursorPos++;
            }
        } else if (e.key.keysym.sym == SDLK_LEFT && cursorPos > 0) {
            cursorPos--;
        } else if (e.key.keysym.sym == SDLK_RIGHT && cursorPos < linkedText.get().size()) {
            cursorPos++;
        }
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
        updateCursorPosition();
    } else if (e.type == SDL_MOUSEWHEEL) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        if (mx >= bounds.x && mx <= bounds.x + bounds.w && my >= bounds.y && my <= bounds.y + bounds.h) {
            int lh = TTF_FontHeight(font ? font : UIConfig::getDefaultFont());
            scrollOffsetY -= e.wheel.y * lh;
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, contentHeight - float(bounds.h));
        }
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

std::vector<std::string> UITextArea::wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) {
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
        TTF_SizeText(font, temp.c_str(), &w, &h);
        if (w > maxWidth) {
            if (currentLine.empty()) {
                std::string part;
                for (size_t i = 0; i < currentWord.size(); ++i) {
                    std::string test = currentWord.substr(0, i+1);
                    TTF_SizeText(font, test.c_str(), &w, &h);
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
    const UITheme& theme = getTheme();
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;

    if (!label.empty()) {
        SDL_Surface* ls = TTF_RenderUTF8_Blended(fnt, label.c_str(), theme.textColor);
        SDL_Texture* lt = SDL_CreateTextureFromSurface(renderer, ls);
        SDL_Rect labelRect = {bounds.x, bounds.y - ls->h - 4, ls->w, ls->h};
        SDL_RenderCopy(renderer, lt, nullptr, &labelRect);
        SDL_FreeSurface(ls);
        SDL_DestroyTexture(lt);
    }

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);
    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    SDL_RenderSetClipRect(renderer, &bounds);
    std::string txt = linkedText.get().empty() ? placeholder : linkedText.get();
    SDL_Color col = linkedText.get().empty() ? theme.placeholderColor : theme.textColor;
    auto lines = wrapTextToLines(txt, fnt, bounds.w - 10);
    int lh = TTF_FontHeight(fnt);
    int y = bounds.y + 5 - static_cast<int>(scrollOffsetY);

    for (const auto& line : lines) {
        if (!line.empty()) {
            SDL_Surface* s = TTF_RenderUTF8_Blended(fnt, line.c_str(), col);
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_Rect dst = {bounds.x + 5, y, s->w, s->h};
            SDL_RenderCopy(renderer, t, nullptr, &dst);
            SDL_FreeSurface(s);
            SDL_DestroyTexture(t);
        }
        y += lh;
    }

    if (focused && cursorVisible) {
        int onScreenY = cursorY - static_cast<int>(scrollOffsetY);
        onScreenY = std::clamp(onScreenY, bounds.y, bounds.y + bounds.h - lh);
        SDL_SetRenderDrawColor(renderer, theme.cursorColor.r, theme.cursorColor.g, theme.cursorColor.b, theme.cursorColor.a);
        SDL_Rect cursorRect = {cursorX, onScreenY, 2, lh};
        SDL_RenderFillRect(renderer, &cursorRect);
    }

    if (focused || !linkedText.get().empty()) {
        int words = getWordCount();
        std::string wcLabel = std::to_string(words) + " words";
        SDL_Surface* wcSurface = TTF_RenderUTF8_Blended(fnt, wcLabel.c_str(), theme.placeholderColor);
        if (wcSurface) {
            SDL_Texture* wcTexture = SDL_CreateTextureFromSurface(renderer, wcSurface);
            SDL_Rect wcRect = {
                bounds.x,
                bounds.y + bounds.h + 4,
                wcSurface->w,
                wcSurface->h
            };
            SDL_RenderCopy(renderer, wcTexture, nullptr, &wcRect);
            SDL_FreeSurface(wcSurface);
            SDL_DestroyTexture(wcTexture);
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
    if (contentHeight > bounds.h) {
        renderScrollbar(renderer);
    }
}


void UITextArea::updateCursorPosition() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    int lh = TTF_FontHeight(fnt);
    std::string before = linkedText.get().substr(0, cursorPos);
    auto lines = wrapTextToLines(before, fnt, bounds.w - 10);
    int caretLine = int(lines.size()) - 1;
    const std::string& last = lines.empty() ? "" : lines.back();
    int w = 0;
    if (!last.empty()) {
        TTF_SizeText(fnt, last.c_str(), &w, nullptr);
    }
    cursorX = bounds.x + 5 + w;
    cursorY = bounds.y + 5 + caretLine * lh;
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
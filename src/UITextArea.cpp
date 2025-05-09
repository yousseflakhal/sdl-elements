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
        if (!wasFocused && focused) {
            SDL_StartTextInput();
            cursorPos = linkedText.get().size();
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        }
        else if (wasFocused && !focused) SDL_StopTextInput();
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        std::string input = e.text.text;
        std::string& text = linkedText.get();
        if (text.size() < static_cast<size_t>(maxLength)) {
            text.insert(cursorPos, input);
            cursorPos += input.size();
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        }
    }

    if (focused && e.type == SDL_KEYDOWN) {
        std::string& text = linkedText.get();
        if (e.key.keysym.sym == SDLK_BACKSPACE && cursorPos > 0) {
            text.erase(cursorPos - 1, 1);
            cursorPos--;
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        }
        else if (e.key.keysym.sym == SDLK_RETURN) {
            if (text.size() < static_cast<size_t>(maxLength)) {
                text.insert(cursorPos, "\n");
                cursorPos++;
                lastBlinkTime = SDL_GetTicks();
                cursorVisible = true;
            }
        }
        else if (e.key.keysym.sym == SDLK_LEFT && cursorPos > 0) {
            cursorPos--;
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        }
        else if (e.key.keysym.sym == SDLK_RIGHT && cursorPos < text.size()) {
            cursorPos++;
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        }
    }
}

void UITextArea::update(float) {
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

bool UITextArea::isHovered() const {
    return hovered;
}

static std::vector<std::string> wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) {
    std::vector<std::string> lines;
    std::string currentLine;
    std::string currentWord;

    for (char c : text) {
        if (c == '\n') {
            currentLine += currentWord;
            lines.push_back(currentLine);
            currentLine.clear();
            currentWord.clear();
            continue;
        }

        currentWord += c;
        std::string temp = currentLine + currentWord;
        int width, height;
        TTF_SizeText(font, temp.c_str(), &width, &height);

        if (width > maxWidth) {
            if (currentLine.empty()) {
                std::string part;
                for (size_t i = 0; i < currentWord.size(); ++i) {
                    std::string testPart = currentWord.substr(0, i+1);
                    TTF_SizeText(font, testPart.c_str(), &width, &height);
                    if (width > maxWidth) break;
                    part = testPart;
                }
                if (part.empty()) part = currentWord.substr(0, 1);
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

    currentLine += currentWord;
    if (!currentLine.empty()) lines.push_back(currentLine);
    return lines;
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

    auto lines = wrapTextToLines(content, activeFont, bounds.w - 10);
    int lineHeight = TTF_FontHeight(activeFont);
    int yOffset = bounds.y + 5;

    int cursorX = bounds.x + 5;
    int cursorY = yOffset;
    bool cursorDrawn = false;
    size_t textPosition = 0;

    for (size_t i = 0; i < lines.size(); ++i) {
        const std::string& line = lines[i];
        SDL_Surface* surf = TTF_RenderText_Blended(activeFont, line.c_str(), color);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect textRect = { bounds.x + 5, yOffset, surf->w, surf->h };
            SDL_RenderCopy(renderer, tex, nullptr, &textRect);

            if (focused && !cursorDrawn) {
                size_t lineEnd = textPosition + line.size();
                bool hasNewline = lineEnd < content.size() && content[lineEnd] == '\n';

                if (cursorPos >= textPosition && cursorPos <= lineEnd + (hasNewline ? 1 : 0)) {
                    if (hasNewline && cursorPos == lineEnd) {
                        cursorX = bounds.x + 5;
                        cursorY = yOffset + lineHeight;
                        cursorDrawn = true;
                    }
                    else if (cursorPos <= lineEnd) {
                        std::string beforeCursor = line.substr(0, cursorPos - textPosition);
                        int w, h;
                        TTF_SizeText(activeFont, beforeCursor.c_str(), &w, &h);
                        cursorX = bounds.x + 5 + w;
                        cursorY = yOffset;
                        cursorDrawn = true;
                    }
                }
            }

            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
        textPosition += line.size() + (textPosition + line.size() < content.size() && content[textPosition + line.size()] == '\n' ? 1 : 0);
        yOffset += lineHeight;
    }

    if (focused && cursorVisible && !cursorDrawn) {
        cursorX = bounds.x + 5;
        cursorY = yOffset;
    }

    if (focused && cursorVisible) {
        SDL_Rect cursorRect = { cursorX, cursorY, 2, lineHeight };
        SDL_SetRenderDrawColor(renderer, theme.cursorColor.r, theme.cursorColor.g, theme.cursorColor.b, theme.cursorColor.a);
        SDL_RenderFillRect(renderer, &cursorRect);
    }
}

void UITextArea::updateCursorPosition() {
    const std::string& content = linkedText.get();
    textLength = content.size();
    cursorPos = std::min(cursorPos, textLength);
}
#pragma once
#include "UIElement.hpp"
#include "UICommon.hpp"
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <SDL2/SDL_ttf.h>
#include "UIConfig.hpp"
#include <SDL2/SDL.h>

class UITextArea : public UIElement {
public:
    UITextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 512);

    void setFont(TTF_Font* f);
    void setPlaceholder(const std::string& text);
    void updateCursorPosition();
    SDL_Rect getScrollbarRect() const;
    void renderScrollbar(SDL_Renderer* renderer);
    bool isScrollbarHovered() const;
    bool isScrollbarDragging() const;

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;
    int getWordCount() const;

private:
    std::vector<std::string> wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth);
    std::string label;
    std::reference_wrapper<std::string> linkedText;
    std::string placeholder;
    int maxLength;
    bool hovered = false;
    bool focused = false;
    TTF_Font* font = nullptr;
    Uint32 lastBlinkTime = 0;
    bool cursorVisible = true;
    size_t cursorPos = 0;
    bool scrollbarHovered = false;
    bool scrollbarDragging = false;
    int scrollbarDragStartY = 0;
    int scrollbarThumbStartOffset = 0;
    InputType inputType = InputType::TEXT;
    float scrollOffsetY = 0.0f;
    float contentHeight = 0.0f;
    int cursorX = 0, cursorY = 0;
};

#pragma once
#include "UIElement.hpp"
#include "UICommon.hpp"
#include "UIHelpers.hpp"
#include "UIStyles.hpp"
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
    bool isFocusable() const override { return true; }

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;
    int getWordCount() const;
    void setSelection(size_t a, size_t b);
    inline std::pair<size_t,size_t> selRange() const {
        return selStart < selEnd ? std::make_pair(selStart, selEnd)
                                : std::make_pair(selEnd, selStart);
    }
    inline void selectAll() {
        selStart = 0;
        selEnd   = linkedText.get().size();
        cursorPos = selEnd;
    }
    bool hasSelection() const {
        return selStart != std::string::npos && selEnd != std::string::npos && selStart != selEnd;
    }
    void clearSelection() { selStart = selEnd = std::string::npos; }
    std::pair<size_t,size_t> selectionRange() const {
        if (!hasSelection()) return {0,0};
        return selStart < selEnd ? std::pair<size_t,size_t>(selStart, selEnd)
                                : std::pair<size_t,size_t>(selEnd, selStart);
    }

    size_t indexFromMouse(int mx, int my) const;
    void setIMERectAtCaret();

private:
    std::vector<std::string> wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) const ;
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
    int cornerRadius = 10;
    int borderPx     = 1;
    int paddingPx    = 8;
    size_t selStart = std::string::npos;
    size_t selEnd   = std::string::npos;
    bool   selectingDrag = false;
    std::string imeText;
    int imeStart = 0;
    int imeLength = 0;
    bool imeActive = false;
    bool selectingMouse = false;
    size_t selectAnchor = 0;
    size_t selectionStart = 0;
    size_t selectionEnd   = 0;
    bool   selectionActive = false;
    Uint32 lastClickTicks = 0;
    int clickCount = 0;
    int lastClickX = -10000;
    int lastClickY = -10000;
};

#pragma once
#include "UIElement.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class UITextField : public UIElement {
public:
    UITextField(const std::string& label, int x, int y, int w, int h, std::string* bind, int maxLen = 32);
    UITextField* setPlaceholder(const std::string& text);
    UITextField* setFont(TTF_Font* f);

    bool isHovered() const override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    

private:
    std::string label;
    SDL_Rect bounds;
    std::string* linkedText;
    int maxLength = 32;
    bool hovered = false;
    bool focused = false;
    Uint32 lastBlinkTime = 0;
    bool cursorVisible = true;
    std::string placeholder;
    SDL_Color placeholderColor = {160, 160, 160, 255};
    TTF_Font* font = nullptr;
};

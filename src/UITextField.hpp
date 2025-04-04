#pragma once
#include "UIElement.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class UITextField : public UIElement {
public:
    UITextField(const std::string& label, int x, int y, int w, int h, std::string* bind, int maxLen = 32);

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    bool isHovered() const override;

private:
    std::string label;
    SDL_Rect bounds;
    std::string* linkedText;
    int maxLength = 32;
    bool hovered = false;
    bool focused = false;
};

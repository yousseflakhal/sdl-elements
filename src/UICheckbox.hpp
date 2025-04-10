#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "UIElement.hpp"

class UICheckbox : public UIElement {
public:
    UICheckbox(const std::string& label, int x, int y, int w, int h, bool* bind, TTF_Font* f);

    void setFont(TTF_Font* f);

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    SDL_Rect bounds;
    bool hovered = false;
    bool* linkedValue = nullptr;
    TTF_Font* font = nullptr;
};

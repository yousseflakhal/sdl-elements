#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "UIElement.hpp"
#include "UIConfig.hpp"
#include "UIHelpers.hpp"

class UICheckbox : public UIElement {
public:
    UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f);

    void setFont(TTF_Font* f);

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    bool hovered = false;
    bool focused = false;
    bool focusable = true;
    std::reference_wrapper<bool> linkedValue;
    TTF_Font* font = nullptr;
};

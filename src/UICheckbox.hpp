#pragma once
#include "UIElement.hpp"
#include "UIConfig.hpp"
#include "UIHelpers.hpp"
#include "UIStyles.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <optional>
#include <algorithm>

class UICheckbox : public UIElement {
public:
    UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f);

    void setFont(TTF_Font* f);
     UICheckbox* setTextColor(SDL_Color c)    { customTextColor   = c; hasCustomTextColor   = true; return this; }
    UICheckbox* setCheckedColor(SDL_Color c) { customCheckedColor= c; hasCustomCheckedColor= true; return this; }
    UICheckbox* setBoxBackground(SDL_Color c){ customBoxBgColor   = c; hasCustomBoxBgColor   = true; return this; }
    UICheckbox* setBorderColor(SDL_Color c)   { customBorderColor = c; hasCustomBorderColor = true; return this; }
    UICheckbox* setBorderThickness(int px) { borderPx = std::max(0, px); return this; }

    bool isFocusable() const override { return focusable; }

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
    SDL_Color customTextColor{};    bool hasCustomTextColor    = false;
    SDL_Color customCheckedColor{}; bool hasCustomCheckedColor = false;
    SDL_Color customBoxBgColor{};   bool hasCustomBoxBgColor   = false;
    SDL_Color customBorderColor{};  bool hasCustomBorderColor  = false;
    int borderPx = 1;
};

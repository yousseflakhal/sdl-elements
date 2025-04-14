#pragma once
#include <SDL2/SDL.h>
#include "UITheme.hpp"
#include "UIConfig.hpp"

class UIElement {
public:
    SDL_Rect bounds;
    bool visible = true;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual bool isHovered() const { return false; }
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    void setTheme(const UITheme& theme) { customTheme = theme; hasCustomTheme = true; }
    const UITheme& getTheme() const { return hasCustomTheme ? customTheme : UIConfig::getTheme(); }

    virtual ~UIElement() = default;

private:
    UITheme customTheme;
    bool hasCustomTheme = false;
};
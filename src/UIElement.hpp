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
    virtual void setPosition(int x, int y) { bounds.x = x; bounds.y = y; }
    virtual void setSize(int w, int h) { bounds.w = w; bounds.h = h; }
    virtual void setBounds(int x, int y, int w, int h) { bounds = {x, y, w, h}; }
    void setTheme(const UITheme& theme) { customTheme = theme; hasCustomTheme = true; }
    const UITheme& getTheme() const { return hasCustomTheme ? customTheme : UIConfig::getTheme(); }
    SDL_Point getPosition() const { return { bounds.x, bounds.y }; }
    SDL_Point getSize() const { return { bounds.w, bounds.h }; }

    virtual ~UIElement() = default;

private:
    UITheme customTheme;
    bool hasCustomTheme = false;
};
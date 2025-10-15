#pragma once
#include <SDL2/SDL.h>
#include "UITheme.hpp"
#include "UIConfig.hpp"
#include "UIStyles.hpp"
#include "UIHelpers.hpp"

class UIElement {
public:
    SDL_Rect bounds;
    bool visible = true;
    bool enabled = true;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual bool isHovered() const { return false; }
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void setPosition(int x, int y) { bounds.x = x; bounds.y = y; }
    virtual void setSize(int w, int h) { bounds.w = w; bounds.h = h; }
    virtual void setBounds(int x, int y, int w, int h) { bounds = {x, y, w, h}; }
    virtual bool isFocusable() const { return false; }

    void setTheme(const UITheme& theme) { customTheme = theme; hasCustomTheme = true; }
    const UITheme& getTheme() const { return hasCustomTheme ? customTheme : UIConfig::getTheme(); }

    void setStyle(const UIStyle& style) { customStyle = style; hasCustomStyle = true; }
    const UIStyle& getStyle() const { return hasCustomStyle ? customStyle : UIConfig::getStyle(); }
    void clearThemeOverride() { hasCustomTheme = false; }
    void clearStyleOverride() { hasCustomStyle = false; }

    SDL_Point getPosition() const { return { bounds.x, bounds.y }; }
    SDL_Point getSize() const { return { bounds.w, bounds.h }; }
    virtual bool isInside(int x, int y) const {
        return x >= bounds.x && x <= bounds.x + bounds.w &&
               y >= bounds.y && y <= bounds.y + bounds.h;
    }
    void setEnabled(bool e) { enabled = e; }
    bool isEnabled() const { return enabled; }

    virtual ~UIElement() = default;

private:
    UITheme customTheme;
    UIStyle customStyle;
    bool hasCustomTheme = false;
    bool hasCustomStyle = false;
};
#pragma once
#include "UIElement.hpp"
#include "UIHelpers.hpp"
#include <functional>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <optional>
#include "UIConfig.hpp"

class UIButton : public UIElement {
public:
    UIButton(const std::string& text, int x, int y, int w, int h, TTF_Font* f = nullptr);
    void setOnClick(std::function<void()> callback);
    void setText(const std::string& newText);
    const std::string& getText() const;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isFocusable() const override { return focusable; }
    void setFont(TTF_Font* f);
    bool isHovered() const;
    UIButton* setTextColor(SDL_Color c);
    UIButton* setBackgroundColor(SDL_Color c);
    UIButton* setBorderColor(SDL_Color c);
    UIButton* setCornerRadius(int r) { cornerRadius = (r < 0 ? 0 : r); return this; }
    UIButton* setBorderThickness(int px) { borderPx = (px < 0 ? 0 : px); return this; }
    UIButton* setFocusable(bool f) { focusable = f; return this; }
    bool isFocused() const { return focused; }

private:
    std::string label;
    std::function<void()> onClick;
    bool hovered = false;
    bool pressed = false;
    TTF_Font* font = nullptr;
    std::optional<SDL_Color> customTextColor;
    std::optional<SDL_Color> customBgColor;
    std::optional<SDL_Color> customBorderColor;
    int cornerRadius = 4;
    int borderPx     = 1;
    bool focused = false;
    bool focusable = true;
    int pressOffset = 1;
};

#pragma once
#include "UIElement.hpp"
#include <string>
#include <SDL2/SDL_ttf.h>

class UITextArea : public UIElement {
public:
    UITextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 512);

    void setFont(TTF_Font* f);
    void setPlaceholder(const std::string& text);
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;

private:
    std::string label;
    std::reference_wrapper<std::string> linkedText;
    std::string placeholder;
    int maxLength;
    bool hovered = false;
    bool focused = false;
    TTF_Font* font = nullptr;
};

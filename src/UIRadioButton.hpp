#pragma once
#include "UIElement.hpp"
#include <string>
#include "UIRadioGroup.hpp"
#include "UIHelpers.hpp"
#include <cmath>

class UIRadioGroup;

class UIRadioButton : public UIElement {
public:
    UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font = nullptr);

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;
    bool isFocusable() const override { return true; }
    void setFont(TTF_Font* font);
    int getID() const;


    UIRadioButton* setFocusable(bool f) { focusable = f; return this; }
    bool isFocused() const { return focused; }

private:
    std::string label;
    int id;
    UIRadioGroup* group = nullptr;
    TTF_Font* font = nullptr;
    bool hovered = false;
    bool pressed = false;
    bool focused = false;
    bool focusable = true;
};

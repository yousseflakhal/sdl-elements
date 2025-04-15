#pragma once
#include "UIElement.hpp"
#include <string>
#include "UIRadioGroup.hpp"

class UIRadioGroup;

class UIRadioButton : public UIElement {
    public:
        UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font = nullptr);
    
        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render(SDL_Renderer* renderer) override;
        bool isHovered() const override;
        void setFont(TTF_Font* font);
        int getID() const;
    
    private:
        std::string label;
        int id;
        UIRadioGroup* group = nullptr;
        TTF_Font* font = nullptr;
        bool hovered = false;
    };
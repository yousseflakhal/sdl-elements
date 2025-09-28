#pragma once
#include "UIElement.hpp"
#include <vector>
#include <memory>
#include <SDL2/SDL.h>

class UIPopup : public UIElement {
public:
    UIPopup(int x, int y, int w, int h);
    void addChild(std::shared_ptr<UIElement> el);
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    int getPadFromTheme() const {
        return MakePopupStyle(getTheme()).pad;
    }

    std::vector<std::shared_ptr<UIElement>> children;
    
};

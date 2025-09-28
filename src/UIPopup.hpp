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
    void centerInRenderer(SDL_Renderer* r) {
        if (!r) return;
        int rw = 0, rh = 0;
        if (SDL_GetRendererOutputSize(r, &rw, &rh) == 0) {
            setBounds((rw - bounds.w) / 2, (rh - bounds.h) / 2, bounds.w, bounds.h);
        }
    }

    std::vector<std::shared_ptr<UIElement>> children;
    
};

#pragma once
#include "UIElement.hpp"
#include <functional>

class UISpinner : public UIElement {
    public:
        UISpinner(int x, int y, int w, int h, int& bind, int min = 0, int max = 100, int step = 1);
    
        void setFont(TTF_Font* f);
        void setOnChange(std::function<void(int)> callback);
    
        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render(SDL_Renderer* renderer) override;
    
    private:
        std::reference_wrapper<int> value;
        int minValue;
        int maxValue;
        int step;
        bool hoveredMinus = false;
        bool hoveredPlus = false;
        TTF_Font* font = nullptr;
        std::function<void(int)> onChange;
        enum class HeldButton { NONE, INCREMENT, DECREMENT };
        HeldButton heldButton = HeldButton::NONE;
        Uint32 pressStartTime = 0;
        Uint32 lastStepTime = 0;
};
    
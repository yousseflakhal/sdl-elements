#pragma once
#include "UIElement.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iomanip>

class UISlider : public UIElement {
public:
    UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max);

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    SDL_Rect bounds;
    std::reference_wrapper<float> linkedValue;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    bool hovered = false;
    bool dragging = false;
};
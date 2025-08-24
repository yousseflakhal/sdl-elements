#pragma once
#include "UIElement.hpp"
#include "UIHelpers.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

class UISlider : public UIElement {
public:
    UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max);

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    std::reference_wrapper<float> linkedValue;
    float minVal = 0.0f, maxVal = 100.0f;

    bool hovered = false;
    bool focused = false;
    bool focusable = true;
    bool dragging = false;

    int thumbRadius = 9;
};
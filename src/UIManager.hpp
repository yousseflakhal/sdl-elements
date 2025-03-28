#pragma once
#include <vector>
#include <memory>
#include "UIElement.hpp"

class UIManager {
public:
    void addElement(std::shared_ptr<UIElement> el);
    void handleEvent(const SDL_Event& e);
    void update(float dt);
    void render(SDL_Renderer* renderer);

private:
    std::vector<std::shared_ptr<UIElement>> elements;
};

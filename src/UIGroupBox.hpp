#pragma once
#include "UIElement.hpp"
#include <vector>
#include <memory>
#include <string>

class UIGroupBox : public UIElement {
public:
    UIGroupBox(const std::string& title, int x, int y, int w, int h);

    void addChild(std::shared_ptr<UIElement> child);

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string title;
    std::vector<std::shared_ptr<UIElement>> children;
    TTF_Font* font = nullptr;
};
#include "UIPopup.hpp"

UIPopup::UIPopup(int x, int y, int w, int h) {
    bounds = { x, y, w, h };
}

void UIPopup::addChild(std::shared_ptr<UIElement> el) {
    children.push_back(el);
}

void UIPopup::handleEvent(const SDL_Event& e) {
    for (auto& child : children) {
        child->handleEvent(e);
    }
}

void UIPopup::update(float dt) {
    for (auto& child : children) {
        child->update(dt);
    }
}

void UIPopup::render(SDL_Renderer* renderer) {
    for (auto& child : children) {
        child->render(renderer);
    }
}
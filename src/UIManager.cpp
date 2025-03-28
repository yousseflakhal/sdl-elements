#include "UIManager.hpp"

void UIManager::addElement(std::shared_ptr<UIElement> el) {
    elements.push_back(el);
}

void UIManager::handleEvent(const SDL_Event& e) {
    for (auto& el : elements) {
        if (el->visible)
            el->handleEvent(e);
    }
}

void UIManager::update(float dt) {
    for (auto& el : elements) {
        if (el->visible)
            el->update(dt);
    }
}

void UIManager::render(SDL_Renderer* renderer) {
    for (auto& el : elements) {
        if (el->visible)
            el->render(renderer);
    }
}

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
    bool hoveringButton = false;

    for (auto& el : elements) {
        if (el->visible) {
            el->update(dt);

            if (auto* button = dynamic_cast<UIButton*>(el.get())) {
                if (button->isHovered()) {
                    hoveringButton = true;
                }
            }
        }
    }

    if (hoveringButton && !handCursorActive) {
        SDL_SetCursor(handCursor);
        handCursorActive = true;
    } else if (!hoveringButton && handCursorActive) {
        SDL_SetCursor(arrowCursor);
        handCursorActive = false;
    }
}


void UIManager::render(SDL_Renderer* renderer) {
    for (auto& el : elements) {
        if (el->visible)
            el->render(renderer);
    }
}

void UIManager::initCursors() {
    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    SDL_SetCursor(arrowCursor);
}

void UIManager::cleanupCursors() {
    SDL_FreeCursor(arrowCursor);
    SDL_FreeCursor(handCursor);
}
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
    SDL_Cursor* cursorToUse = arrowCursor;

    for (auto& el : elements) {
        if (!el->visible) continue;

        el->update(dt);

        if (el->isHovered()) {
            if (dynamic_cast<UITextField*>(el.get())) {
                cursorToUse = ibeamCursor;
            } else if (dynamic_cast<UIButton*>(el.get()) || dynamic_cast<UICheckbox*>(el.get())) {
                cursorToUse = handCursor;
            }
        }
    }

    if (SDL_GetCursor() != cursorToUse) {
        SDL_SetCursor(cursorToUse);
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
    ibeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    SDL_SetCursor(arrowCursor);
}

void UIManager::cleanupCursors() {
    SDL_FreeCursor(arrowCursor);
    SDL_FreeCursor(handCursor);
    SDL_FreeCursor(ibeamCursor);
}
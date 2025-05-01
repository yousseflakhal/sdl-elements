#include "UIManager.hpp"

void UIManager::addElement(std::shared_ptr<UIElement> el) {
    elements.push_back(el);
}

void UIManager::showPopup(std::shared_ptr<UIPopup> popup) {
    activePopup = popup;
}

void UIManager::closePopup() {
    activePopup = nullptr;
}

void UIManager::checkCursorForElement(const std::shared_ptr<UIElement>& el, SDL_Cursor*& cursorToUse) {
    if (!el->visible) return;

    el->update(0.0f);

    if (el->isHovered()) {
        if (dynamic_cast<UITextField*>(el.get())) {
            cursorToUse = ibeamCursor;
        } else if (
            dynamic_cast<UIButton*>(el.get()) ||
            dynamic_cast<UICheckbox*>(el.get()) ||
            dynamic_cast<UIRadioButton*>(el.get()) ||
            dynamic_cast<UISlider*>(el.get()) ||
            dynamic_cast<UIComboBox*>(el.get())
        ) {
            cursorToUse = handCursor;
        }
    }

    if (auto group = dynamic_cast<UIGroupBox*>(el.get())) {
        for (auto& child : group->getChildren()) {
            checkCursorForElement(child, cursorToUse);
        }
    }

    if (auto combo = dynamic_cast<UIComboBox*>(el.get())) {
        if (combo->isExpanded()) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            int itemHeight = combo->getItemHeight();
            int baseY = combo->getBounds().y;
            int itemCount = combo->getItemCount();
            for (int i = 0; i < itemCount; ++i) {
                SDL_Rect itemRect = {
                    combo->getBounds().x,
                    baseY + (i + 1) * itemHeight,
                    combo->getBounds().w,
                    itemHeight
                };
                SDL_Point pt = { mx, my };
                if (SDL_PointInRect(&pt, &itemRect)) {
                    cursorToUse = handCursor;
                    return;
                }
            }
        }
    }
}

void UIManager::handleEvent(const SDL_Event& e) {
    auto popup = activePopup;
    
    if (popup && popup->visible) {
        popup->handleEvent(e);
    } else {
        for (auto& el : elements) {
            if (el->visible) el->handleEvent(e);
        }
    }
}

void UIManager::update(float dt) {
    auto popup = activePopup;
    SDL_Cursor* cursorToUse = arrowCursor;

    if (popup && popup->visible) {
        popup->update(dt);

        for (const auto& child : popup->children) {
            if (child->isHovered()) {
                if (dynamic_cast<UITextField*>(child.get())) {
                    cursorToUse = ibeamCursor;
                } else if (
                    dynamic_cast<UIButton*>(child.get()) || 
                    dynamic_cast<UICheckbox*>(child.get()) ||
                    dynamic_cast<UIRadioButton*>(child.get()) ||
                    dynamic_cast<UISlider*>(child.get())
                ) {
                    cursorToUse = handCursor;
                }
                break;
            }
        }
    } else {
        for (auto& el : elements) {
            checkCursorForElement(el, cursorToUse);
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
    if (activePopup && activePopup->visible) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_Rect fullscreen = { 0, 0, 800, 600 };
        SDL_RenderFillRect(renderer, &fullscreen);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        activePopup->render(renderer);
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
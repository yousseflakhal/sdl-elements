#pragma once
#include <vector>
#include <memory>
#include "UIElement.hpp"
#include "UIButton.hpp"
#include "UITextField.hpp"
#include "UICheckbox.hpp"
#include "UISlider.hpp"
class UIManager {
    public:
        void initCursors();
        void cleanupCursors();
        void addElement(std::shared_ptr<UIElement> el);
        void handleEvent(const SDL_Event& e);
        void update(float dt);
        void render(SDL_Renderer* renderer);
    
    private:
        std::vector<std::shared_ptr<UIElement>> elements;
        SDL_Cursor* arrowCursor = nullptr;
        SDL_Cursor* handCursor = nullptr;
        SDL_Cursor* ibeamCursor = nullptr;
        bool handCursorActive = false;
    };
#pragma once
#include <vector>
#include <memory>
#include "UIElement.hpp"
#include "UIButton.hpp"
#include "UITextField.hpp"
#include "UICheckbox.hpp"
#include "UISlider.hpp"
#include "UIRadioButton.hpp"
#include "UIPopup.hpp"
#include "UIGroupBox.hpp"
#include "UIComboBox.hpp"
#include "UISpinner.hpp"
#include "UITextArea.hpp"
class UIManager {
    public:
        void initCursors();
        void cleanupCursors();
        void addElement(std::shared_ptr<UIElement> el);
        void showPopup(std::shared_ptr<UIPopup> popup);
        std::shared_ptr<UIPopup> GetActivePopup();
        void closePopup();
        void checkCursorForElement(const std::shared_ptr<UIElement>& el, SDL_Cursor*& cursorToUse);
        void handleEvent(const SDL_Event& e);
        void update(float dt);
        void render(SDL_Renderer* renderer);
        
    
    private:
        std::vector<std::shared_ptr<UIElement>> elements;
        SDL_Cursor* arrowCursor = nullptr;
        SDL_Cursor* handCursor = nullptr;
        SDL_Cursor* ibeamCursor = nullptr;
        bool handCursorActive = false;
        std::shared_ptr<UIPopup> activePopup;
    };
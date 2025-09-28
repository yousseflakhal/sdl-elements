#pragma once
#include <vector>
#include <memory>
#include <functional>
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
    enum ShortcutScope { Global=0, WhenNoTextEditing=1, ModalOnly=2 };
    ~UIManager() noexcept;

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

    void registerElement(UIElement* e, bool focusable);
    void setFocusOrder(const std::vector<UIElement*>& order);
    void focusNext();
    void focusPrev();
    void clearFocus();
    void captureMouse(UIElement* e);
    void releaseMouse();
    void setActiveModal(UIElement* m);
    UIElement* activeModal() const;
    void registerShortcut(SDL_Keycode key, Uint16 mods, ShortcutScope scope, std::function<void()> cb);

private:
    bool tryShortcuts_(const SDL_Event& e);
    UIElement* hitTestTopMost_(int x, int y);
    int  findFocusIndex_(UIElement* e);
    void setFocusedIndex_(int idx);
    std::vector<std::shared_ptr<UIElement>> elements;
    SDL_Cursor* arrowCursor = nullptr;
    SDL_Cursor* handCursor = nullptr;
    SDL_Cursor* ibeamCursor = nullptr;
    bool handCursorActive = false;
    std::shared_ptr<UIPopup> activePopup;

    std::vector<UIElement*> focusOrder_;
    int focusedIndex_ = -1;
    UIElement* mouseCaptured_ = nullptr;
    UIElement* activeModal_ = nullptr;

    struct Shortcut {
        SDL_Keycode key;
        Uint16 mods;
        int scope;
        std::function<void()> cb;
    };
    std::vector<Shortcut> shortcuts_;
    bool cursorsReady = false;

    bool pendingPopupClose = false;
    void ensureCursorsInit_();
    void cleanupCursors_();
    std::vector<UIElement*> savedFocusOrder_;
    int savedFocusedIndex_ = -1;
};

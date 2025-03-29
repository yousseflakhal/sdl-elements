#include "FormUI.hpp"

namespace FormUI {
    static UIManager uiManager;

    void Init(TTF_Font* defaultFont) {
        if (defaultFont) {
            UIConfig::setDefaultFont(defaultFont);
        }
        uiManager.initCursors();
    }

    void Shutdown() {
        uiManager.cleanupCursors();
    }

    std::shared_ptr<UIButton> Button(const std::string& label, int x, int y, int w, int h, std::function<void()> onClick) {
        auto btn = std::make_shared<UIButton>(label, x, y, w, h);
        if (onClick) {
            btn->setOnClick(onClick);
        }
        uiManager.addElement(btn);
        return btn;
    }

    std::shared_ptr<UICheckbox> Checkbox(const std::string& label, int x, int y, int w, int h, bool* bind) {
        auto box = std::make_shared<UICheckbox>(label, x, y, w, h, bind);
        uiManager.addElement(box);
        return box;
    }
    
    std::shared_ptr<UILabel> Label(const std::string& text, int x, int y, int w, int h, TTF_Font* font) {
        auto label = std::make_shared<UILabel>(text, x, y, w, h, font);
        uiManager.addElement(label);
        return label;
    }

    void HandleEvent(const SDL_Event& e) {
        uiManager.handleEvent(e);
    }

    void Update(float dt) {
        uiManager.update(dt);
    }

    void Render(SDL_Renderer* renderer) {
        uiManager.render(renderer);
    }
}
